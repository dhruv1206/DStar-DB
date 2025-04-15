#ifndef STREAM_VALUE_H
#define STREAM_VALUE_H

#include "../IStreamValue.h"
#include "../Serializer/ValueSerializationHelper.h"
#include "Stream/StreamMessage.h"
#include "Stream/StreamConsumerGroup.h"
#include <chrono>
#include <map>
#include <cstdint>
#include <atomic>
#include <memory>
#include <string>

class StreamValue : public IStreamValue
{
public:
    StreamValue() = default;

    // Returns the type of the value as a string.
    std::string getType() const override
    {
        return "stream";
    }

    // Returns a string representation of the value.
    std::string toString() const override
    {
        std::string result = "StreamValue: ";
        for (const auto &pair : messages)
        {
            result += "Message ID: " + std::to_string(pair.first.milliseconds) + "-" + std::to_string(pair.first.sequenceNumber) + "\n";
        }
        return result;
    }

    // Returns the value as a std::any object.
    std::any get() const override
    {
        return messages;
    }

    // Serialize the value into a binary buffer.
    std::vector<uint8_t> serialize() const override
    {
        std::vector<uint8_t> buffer;
        size_t count = messages.size();
        ValueSerializationHelper::appendToBuffer(buffer, count);
        for (const auto &p : messages)
        {
            ValueSerializationHelper::appendToBuffer(buffer, std::to_string(p.first.milliseconds) + "-" + std::to_string(p.first.sequenceNumber));
            size_t fieldCount = p.second.fields.size();
            ValueSerializationHelper::appendToBuffer(buffer, fieldCount);
            for (const auto &f : p.second.fields)
            {
                ValueSerializationHelper::appendToBuffer(buffer, f.first);
                ValueSerializationHelper::appendToBuffer(buffer, f.second);
            }
        }
        return buffer;
    }

    // Reconstruct the value from a binary buffer.
    void deserialize(const std::vector<uint8_t> &data) override
    {
        size_t offset = 0;
        size_t count = 0;
        if (!ValueSerializationHelper::readFromBuffer(data, offset, count))
            throw std::runtime_error("Failed to deserialize stream message count");
        messages.clear();
        for (size_t i = 0; i < count; i++)
        {
            std::string id;
            if (!ValueSerializationHelper::readFromBuffer(data, offset, id))
                throw std::runtime_error("Failed to deserialize message id");
            StreamId streamId;
            size_t pos = id.find('-');
            if (pos == std::string::npos)
                throw std::runtime_error("Invalid message id format");
            streamId.milliseconds = std::stoull(id.substr(0, pos));
            streamId.sequenceNumber = std::stoull(id.substr(pos + 1));
            StreamMessage msg;
            msg.id = streamId;
            size_t fieldCount = 0;
            if (!ValueSerializationHelper::readFromBuffer(data, offset, fieldCount))
                throw std::runtime_error("Failed to deserialize field count");
            for (size_t j = 0; j < fieldCount; j++)
            {
                std::string key, value;
                if (!ValueSerializationHelper::readFromBuffer(data, offset, key))
                    throw std::runtime_error("Failed to deserialize field key");
                if (!ValueSerializationHelper::readFromBuffer(data, offset, value))
                    throw std::runtime_error("Failed to deserialize field value");
                msg.fields[key] = value;
            }
            messages[streamId] = msg;
        }
    }

    // Append a new message. Returns the message ID.
    StreamId xadd(const std::unordered_map<std::string, std::string> &fields) override
    {
        StreamId id = generateMessageId();
        StreamMessage msg;
        msg.id = id;
        msg.fields = fields;
        messages[id] = msg;
        return id;
    }

    // Return messages in the specified ID range [start, end] (inclusive).
    std::vector<StreamMessage> xrange(const StreamId &start, const StreamId &end) const override
    {
        std::vector<StreamMessage> result;
        auto it = messages.lower_bound(start);
        while (it != messages.end() && it->first <= end)
        {
            result.push_back(it->second);
            ++it;
        }
        return result;
    }

    // Return at most count messages with IDs greater than last_id.
    std::vector<StreamMessage> xread(const StreamId &last_id, int count) const override
    {
        std::vector<StreamMessage> result;
        auto it = messages.upper_bound(last_id);
        while (it != messages.end() && count-- > 0)
        {
            result.push_back(it->second);
            ++it;
        }
        return result;
    }

    void createConsumerGroup(const std::string &groupName, const StreamId &startId = {0, 0}) override
    {
        std::lock_guard<std::mutex> lock(cgMutex);
        if (consumerGroups.find(groupName) != consumerGroups.end())
            throw std::runtime_error("Consumer group already exists");
        consumerGroups.emplace(groupName, std::make_unique<ConsumerGroup>(groupName, startId));
    }

    ConsumerGroup *getConsumerGroup(const std::string &groupName) override
    {
        std::lock_guard<std::mutex> lock(cgMutex);
        auto it = consumerGroups.find(groupName);
        if (it == consumerGroups.end())
            throw std::runtime_error("Consumer group not found");
        return it->second.get();
    }

    std::vector<StreamMessage> xreadGroup(const std::string &groupName, const std::string &consumerName, const StreamId &lastId, int count) override
    {
        ConsumerGroup *group = getConsumerGroup(groupName);
        group->addConsumer(consumerName);
        std::vector<StreamMessage> msgs = xread(lastId, count);
        for (const auto &msg : msgs)
        {
            group->addPending(msg.id, consumerName);
        }
        if (!msgs.empty())
        {
            group->updateLastDeliveredId(msgs.back().id);
        }
        return msgs;
    }

private:
    // Use ordered map to maintain the order of insertion.
    std::map<StreamId, StreamMessage> messages;
    std::unordered_map<std::string, std::unique_ptr<ConsumerGroup>> consumerGroups;
    mutable std::mutex cgMutex;

    // Generate a new message ID.
    StreamId generateMessageId() const
    {
        // Get the current timestamp in milliseconds.
        auto now = std::chrono::steady_clock::now();
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        static std::atomic<unsigned long> seq{0};
        uint64_t sequence = seq.fetch_add(1);
        return {ms, sequence};
    }
};

#endif // STREAM_VALUE_H