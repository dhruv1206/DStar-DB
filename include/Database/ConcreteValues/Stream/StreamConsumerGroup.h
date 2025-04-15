#ifndef STREAM_CONSUMER_GROUP_H
#define STREAM_CONSUMER_GROUP_H

#include <string>
#include <chrono>
#include "StreamMessage.h"
#include <mutex>
#include <vector>

// A structure representing a pending stream message delivery.
struct PendingEntry
{
    StreamId messageId;
    std::string consumer; // Consumer name that received it.
    std::chrono::steady_clock::time_point deliveryTime;
    int deliveryCount; // How many times this message was delivered.
};

class Consumer
{
public:
    explicit Consumer(const std::string &name) : name(name) {}
    const std::string &getName() const { return name; }

private:
    std::string name;
};

class ConsumerGroup
{
public:
    ConsumerGroup(const std::string &groupName, const StreamId &lastDeliveredId = {0, 0})
        : groupName(groupName), lastDeliveredId(lastDeliveredId) {}

    // Add a consumer to the group (if not already present)
    void addConsumer(const std::string &consumerName)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (consumers.find(consumerName) == consumers.end())
        {
            consumers.emplace(consumerName, Consumer(consumerName));
        }
    }

    // Add a pending message for a consumer.
    void addPending(const StreamId &messageId, const std::string &consumerName)
    {
        std::lock_guard<std::mutex> lock(mtx);
        PendingEntry entry{messageId, consumerName, std::chrono::steady_clock::now(), 1};
        pending[messageId] = entry;
    }

    // Acknowledge a message. Returns true if the message was pending and now removed.
    bool ack(const StreamId &messageId)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return pending.erase(messageId) > 0;
    }

    // Get all pending entries (optionally filter by consumer).
    std::vector<PendingEntry> getPending(const std::string &consumerName = "")
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<PendingEntry> result;
        for (const auto &p : pending)
        {
            if (consumerName.empty() || p.second.consumer == consumerName)
            {
                result.push_back(p.second);
            }
        }
        return result;
    }

    // Update the last delivered message ID.
    void updateLastDeliveredId(const StreamId &id)
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastDeliveredId = id;
    }

    const StreamId &getLastDeliveredId() const
    {
        return lastDeliveredId;
    }

    const std::string &getGroupName() const
    {
        return groupName;
    }

private:
    std::string groupName;
    StreamId lastDeliveredId;
    std::unordered_map<std::string, Consumer> consumers;
    std::unordered_map<StreamId, PendingEntry> pending;
    mutable std::mutex mtx;
};

#endif // STREAM_CONSUMER_GROUP_H