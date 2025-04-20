#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Record.h"
#include "../MemoryManager/MemoryManager.h"
#include "Observer/IDatabaseObserver.h"
#include "IValue.h"
#include <mutex>
#include <cstdint>

class Client;

class IDatabase
{
public:
    virtual ~IDatabase() = default;
    // Create a new record with a unique id.
    virtual void insertRecord(const std::string &id, std::unique_ptr<IValue> value, std::shared_ptr<Client> client) = 0;

    // Create a new record with a unique id and ttl.
    virtual void insertRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds, std::shared_ptr<Client> client) = 0;

    // Update a record with a unique id.
    virtual void updateRecord(const std::string &id, std::unique_ptr<IValue> value, std::shared_ptr<Client> client) = 0;

    // Update a record with a unique id and ttl.
    virtual void updateRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds, std::shared_ptr<Client> client) = 0;

    // Set TTL for a record.
    virtual void setTTL(const std::string &id, int ttlSeconds, std::shared_ptr<Client> client) = 0;

    // Remove TTL for a record.
    virtual void removeTTL(const std::string &id, std::shared_ptr<Client> client) = 0;

    // Get a record by id. 
    virtual std::shared_ptr<Record> getRecord(const std::string &id, std::shared_ptr<Client> client) = 0;

    // Delete a record by id.
    virtual void deleteRecord(const std::string &id, std::shared_ptr<Client> client) = 0;

    // Get the current cache size in MB.
    virtual size_t getCacheSize() const = 0;

    // Get all records in the database.
    virtual std::unordered_map<std::string, std::shared_ptr<Record>> getAllRecords() const = 0;

    // Return the current version of the record.
    virtual uint64_t getRecordVersion(const std::string &id) const = 0;

    // Observer registration.
    virtual void registerObserver(IDatabaseObserver *observer) = 0;

    // Record specific observer registration.
    virtual void registerRecordObserver(const std::string &key, IDatabaseObserver *obs) = 0;
    // Record specific observer registration.
    virtual void unregisterRecordObserver(const std::string &key, IDatabaseObserver *obs) = 0;

    // Get entire db lock
    virtual std::unique_lock<std::shared_mutex> lockDatabase() = 0;
};
