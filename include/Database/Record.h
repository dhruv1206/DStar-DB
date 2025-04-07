#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <any>
#include <shared_mutex>
#include <mutex>
#include <atomic>

class Record
{
public:
    // Constructor: pass a unique id for the record.
    explicit Record(const std::string id);

    // Getter for record ID.
    const std::string &getId() const;

    // Set a field with any type of value.
    template <typename T>
    void setField(const std::string fieldName, T value)
    {
        std::unique_lock lock(record_mutex);
        fields[fieldName] = std::any(value);
        // For simplicity, update frequency on modification.
        frequency++;
    }

    // Get a field value by name. Throws if field not found or wrong type.
    template <typename T>
    T getField(const std::string fieldName) const
    {
        std::shared_lock lock(record_mutex);
        auto it = fields.find(fieldName);
        if (it != fields.end())
        {
            try
            {
                // Increment frequency on access.
                frequency++;
                return std::any_cast<T>(it->second);
            }
            catch (const std::bad_any_cast &e)
            {
                throw std::runtime_error("Bad any_cast for field: " + fieldName);
            }
        }
        throw std::runtime_error("Field not found: " + fieldName);
    }

    // Remove a field by name.
    void removeField(const std::string fieldName);

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
    std::unordered_map<std::string, std::any> fields;
    mutable std::shared_mutex record_mutex;
    mutable std::atomic<size_t> frequency{0};
    std::chrono::steady_clock::time_point expirationTime;
    bool hasTTL = false;
};

#endif // RECORD_H
