#ifndef RECORD_H
#define RECORD_H

#include <memory>
#include <string>
#include <stdexcept>
#include <shared_mutex>
#include <mutex>
#include <atomic>
#include "IValue.h"

class Record
{
public:
    // Constructor: pass a unique id for the record.
    explicit Record(const std::string id);

    // Getter for record ID.
    const std::string &getId() const;

    // Set a field with any type of value.
    void setValue(std::unique_ptr<IValue> val)
    {
        std::unique_lock<std::shared_mutex> lock(record_mutex);
        value = std::move(val);
        frequency++;
    }

    // Get the stored value.
    IValue *getValue() const
    {
        std::shared_lock<std::shared_mutex> lock(record_mutex);
        frequency++;
        if (!value)
            throw std::runtime_error("Value not set for this record.");
        return value.get();
    }

    // Increase access frequency.
    void incrementFrequency();

    // Get current access frequency.
    size_t getFrequency() const;

    // Approximate memory usage of the record.
    size_t approximateSize() const;

    // Set TTL (in seconds) by computing the expiration time.
    void setTTL(size_t ttlSeconds)
    {
        std::unique_lock<std::shared_mutex> lock(record_mutex);
        expirationTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
        hasTTL = true;
    }

    // Clear TTL.
    void clearTTL()
    {
        std::unique_lock<std::shared_mutex> lock(record_mutex);
        hasTTL = false;
    }

    // Check if the record has expired.
    bool isExpired() const
    {
        std::shared_lock<std::shared_mutex> lock(record_mutex);
        if (!hasTTL)
            return false;
        return std::chrono::steady_clock::now() > expirationTime;
    }

    // Return expiration time (only valid if hasTTL is true)
    std::chrono::steady_clock::time_point getExpirationTime() const
    {
        std::shared_lock<std::shared_mutex> lock(record_mutex);
        return expirationTime;
    }

private:
    std::string id;
    std::unique_ptr<IValue> value;
    mutable std::shared_mutex record_mutex;
    mutable std::atomic<size_t> frequency{0};
    std::chrono::steady_clock::time_point expirationTime;
    bool hasTTL = false;
};

#endif // RECORD_H
