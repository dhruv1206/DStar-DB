#ifndef IDATABASE_H
#define IDATABASE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Record.h"
#include "../MemoryManager/MemoryManager.h"
#include "Observer/IDatabaseObserver.h"
#include <vector>
#include <any>

class IDatabase
{
public:
    virtual ~IDatabase() = default;
    // Create a new record with a unique id.
    virtual void insertRecord(const std::string &id, std::any &value) = 0;

    // Create a new record with a unique id and ttl.
    virtual void insertRecord(const std::string &id, std::any &value, int ttlSeconds) = 0;

    // Update a record with a unique id.
    virtual void updateRecord(const std::string &id, std::any &value) = 0;

    // Update a record with a unique id and ttl.
    virtual void updateRecord(const std::string &id, std::any &value, int ttlSeconds) = 0;

    // Set TTL for a record.
    virtual void setTTL(const std::string &id, int ttlSeconds) = 0;

    // Remove TTL for a record.
    virtual void removeTTL(const std::string &id) = 0;

    // Get a record by id.
    virtual std::shared_ptr<Record> getRecord(const std::string &id) = 0;

    // Delete a record by id.
    virtual void deleteRecord(const std::string &id) = 0;

    // Get the current cache size in MB.
    virtual size_t getCacheSize() const = 0;

    // Get all records in the database.
    virtual std::unordered_map<std::string, std::shared_ptr<Record>> getAllRecords() const = 0;

    // Observer registration.
    void registerObserver(IDatabaseObserver *observer)
    {
        observers.push_back(observer);
    }

    // Observer registration.
    void notifyObservers(const std::string &operation, const std::string &recordId)
    {
        for (auto observer : observers)
            observer->onDatabaseModified(operation, recordId);
    }
    // Observer registration.
    template <typename T>
    void notifyObservers(const std::string &operation, const std::string &recordId, const T &value)
    {
        for (auto observer : observers)
            observer->onDatabaseModified(operation, recordId, value);
    }

private:
    std::vector<IDatabaseObserver *> observers;
};

#endif // DATABASE_H
