#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Record.h"
#include "../MemoryManager/MemoryManager.h"
#include "IDatabase.h"
#include <stdexcept>
#include <shared_mutex>
#include <any>

// Structure to hold expiration info for a record.
struct ExpirationEntry
{
    std::string id;
    std::chrono::steady_clock::time_point expirationTime;
    bool operator<(const ExpirationEntry &other) const
    {
        if (expirationTime == other.expirationTime)
            return id < other.id;
        return expirationTime < other.expirationTime;
    }
};

class Database : public IDatabase
{
public:
    // Constructor accepting cache size in MB.
    Database(size_t cacheSizeMB) : memoryManager(std::make_unique<MemoryManager>(cacheSizeMB)) {};

    // Create a new record with a unique id.
    void insertRecord(const std::string &id, std::any &value) override
    {
        if (records.find(id) != records.end())
        {
            throw std::runtime_error("Record with id '" + id + "' already exists.");
        }
        auto record = std::make_shared<Record>(id);
        record->setField("value", value);
        size_t recSize = record->approximateSize();
        // Insert into memory manager LFU structure.
        memoryManager->insertKey(id, recSize);
        records[id] = record;
        // Evict records if needed.
        memoryManager->evictIfNeeded(records);
    }

    // Update a record with a unique id.
    void updateRecord(const std::string &id, std::any &value) override
    {
        auto it = records.find(id);
        if (it != records.end())
        {
            auto record = it->second;
            record->setField("value", value);
            // Update LFU frequency.
            memoryManager->touch(id);
        }
        else
        {
            throw std::runtime_error("Record with id '" + id + "' not found.");
        }
    }

    // Get a record by id.
    std::shared_ptr<Record> getRecord(const std::string &id) override
    {
        auto it = records.find(id);
        if (it != records.end())
        {
            // Update LFU frequency.
            memoryManager->touch(id);
            return it->second;
        }
        throw std::runtime_error("Record with id '" + id + "' not found.");
    }

    // Delete a record by id.
    void deleteRecord(const std::string &id) override
    {
        auto it = records.find(id);
        if (it != records.end())
        {
            // Remove from LFU structure.
            memoryManager->removeKey(id);
            records.erase(it);
        }
        else
        {
            throw std::runtime_error("Record with id '" + id + "' not found.");
        }
    }

    // Get all records in the database.
    std::unordered_map<std::string, std::shared_ptr<Record>> getAllRecords() const override
    {
        return records;
    }

    // Get the current cache size in MB.
    size_t getCacheSize() const override
    {
        return memoryManager->getCurrentUsage() / (1024.0 * 1024.0); // Convert to MB
    };

private:
    std::unordered_map<std::string, std::shared_ptr<Record>> records;
    std::unique_ptr<MemoryManager> memoryManager;
};

#endif // DATABASE_H
