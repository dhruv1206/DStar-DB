#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Record.h"
#include "../MemoryManager/MemoryManager.h"
#include "IDatabase.h"
#include <stdexcept>
#include <unordered_set>
#include "IValue.h"
#include "DbOperations.h"
#include "../ClientManager/Client.h"

class Database : public IDatabase
{
public:
    // Constructor accepting cache size in MB.
    Database(size_t cacheSizeMB) : memoryManager(std::make_unique<MemoryManager>(cacheSizeMB)) {};

    // Create a new record with a unique id.
    void insertRecord(const std::string &id, std::unique_ptr<IValue> value, std::shared_ptr<Client> client) override
    {
        if (!isClientInTransaction(client))
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
        }
        if (records.find(id) != records.end())
        {
            throw std::runtime_error("Record with id '" + id + "' already exists.");
        }
        auto record = std::make_shared<Record>(id);
        record->setValue(std::move(value));
        size_t recSize = record->approximateSize();
        // Insert into memory manager LFU structure.
        memoryManager->insertKey(id, recSize);
        records[id] = record;
        // Evict records if needed.
        memoryManager->evictIfNeeded(records);
        notifyRecordObservers(std::string(DbOperations::INSERT), id, record->getValue());
    }

    void insertRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds, std::shared_ptr<Client> client) override
    {
        throw std::runtime_error("Use TTLDatabaseDecorator for inserting records with TTL.");
    }

    // Update a record with a unique id.
    void updateRecord(const std::string &id, std::unique_ptr<IValue> value, std::shared_ptr<Client> client) override
    {
        if (!isClientInTransaction(client))
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
        }
        auto it = records.find(id);
        if (it != records.end())
        {
            auto record = it->second;
            record->setValue(std::move(value));
            // Update LFU frequency.
            memoryManager->touch(id);
        }
        else
        {
            insertRecord(id, std::move(value), client); // If not found, insert as a new record.
        }
        notifyRecordObservers(std::string(DbOperations::UPDATE), id, value.get());
    }

    void updateRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds, std::shared_ptr<Client> client) override

    {
        throw std::runtime_error("Use TTLDatabaseDecorator for updating records with TTL.");
    }

    void setTTL(const std::string &id, int ttlSeconds, std::shared_ptr<Client> client) override
    {
        throw std::runtime_error("Use TTLDatabaseDecorator for setting TTL.");
    }

    void removeTTL(const std::string &id, std::shared_ptr<Client> client) override
    {
        throw std::runtime_error("Use TTLDatabaseDecorator for removing TTL.");
    }

    // Get a record by id.
    std::shared_ptr<Record> getRecord(const std::string &id, std::shared_ptr<Client> client) override
    {
        if (!isClientInTransaction(client))
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
        }
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
    void deleteRecord(const std::string &id, std::shared_ptr<Client> client) override
    {
        if (!isClientInTransaction(client))
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
        }
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
        notifyRecordObservers(std::string(DbOperations::DELETE_OPERATION), id, nullptr);
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

    // Return the current version of the record.
    virtual uint64_t getRecordVersion(const std::string &id) const override
    {
        auto it = records.find(id);
        if (it != records.end())
        {
            return it->second->getVersion();
        }
        throw std::runtime_error("Record with id '" + id + "' not found.");
    }

    // Observer registration.
    void registerObserver(IDatabaseObserver *observer) override
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        observers.push_back(observer);
    }

    // Record specific observer registration.
    void registerRecordObserver(const std::string &key, IDatabaseObserver *obs) override
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        auto it = records.find(key);
        if (it == records.end())
        {
            throw std::runtime_error("Record with id '" + key + "' not found.");
        }
        recordDependencies[key].insert(obs);
    }

    // Record specific observer registration.
    void unregisterRecordObserver(const std::string &key, IDatabaseObserver *obs) override
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        auto it = recordDependencies.find(key);
        if (it != recordDependencies.end())
        {
            auto &observersSet = it->second;
            observersSet.erase(obs);
            if (observersSet.empty())
            {
                recordDependencies.erase(it); // Remove the entry if no observers left
            }
        }
    }

    // Get entire db lock
    std::unique_lock<std::shared_mutex> lockDatabase() override
    {
        return std::unique_lock<std::shared_mutex>(_mutex);
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Record>> records;
    std::unique_ptr<MemoryManager> memoryManager;
    std::vector<IDatabaseObserver *> observers;
    std::unordered_map<std::string, std::unordered_set<IDatabaseObserver *>> recordDependencies;
    std::mutex _obsMutex;
    std::shared_mutex _mutex; // For thread-safe access to the database

    void notifyObservers(const std::string &operation, const std::string &recordId)
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        for (const auto &observer : observers)
            observer->onDatabaseModified(operation, recordId);
    }
    void notifyObservers(const std::string &operation, const std::string &recordId, IValue *value)
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        for (const auto &observer : observers)
            observer->onDatabaseModified(operation, recordId, value);
    }
    void notifyRecordObservers(const std::string &operation, const std::string &recordId)
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        auto it = recordDependencies.find(recordId);
        if (it != recordDependencies.end())
        {
            for (const auto &observer : it->second)
            {
                observer->onRecordModified(operation, recordId);
            }
        }
    }
    void notifyRecordObservers(const std::string &operation, const std::string &recordId, IValue *value)
    {
        std::lock_guard<std::mutex> lock(_obsMutex);
        auto it = recordDependencies.find(recordId);
        if (it != recordDependencies.end())
        {
            for (const auto &observer : it->second)
            {
                observer->onRecordModified(operation, recordId, value);
            }
        }
    }

    void bumpVersion(const std::string &key)
    {
        auto it = records.find(key);
        if (it != records.end())
        {
            it->second->incrementVersion();
        }
        else
        {
            throw std::runtime_error("Record with id '" + key + "' not found.");
        }
    }

    bool isClientInTransaction(const std::shared_ptr<Client> &client) const
    {
        if (client == nullptr)
        {
            return false;
        }
        return client->transactionContext->inTransaction;
    }
};

#endif // DATABASE_H
