#ifndef TTL_DATABASE_DECORATOR_H
#define TTL_DATABASE_DECORATOR_H

#include "../IDatabase.h"
#include "../Record.h"
#include "../IValue.h"
#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <stdexcept>

// TTLDatabaseDecorator: A decorator that adds TTL functionality to an existing IDatabase.
class TTLDatabaseDecorator : public IDatabase
{
public:
    // The constructor takes ownership of an existing database instance.
    TTLDatabaseDecorator(std::unique_ptr<IDatabase> db_)
        : db(std::move(db_)), running(true)
    {
        // Start the background expiration thread.
        expirationThread = std::thread(&TTLDatabaseDecorator::expirationWorker, this);
    }

    ~TTLDatabaseDecorator()
    {
        running = false;
        if (expirationThread.joinable())
        {
            expirationThread.join();
        }
    }

    void insertRecord(const std::string &id, std::unique_ptr<IValue> value) override
    {
        // Delegate to the underlying database.
        db->insertRecord(id, std::move(value));
        // No TTL provided, so no action needed.
    }

    // Insert a record with an optional TTL (in seconds). TTL = 0 means no expiration.
    void insertRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds) override
    {
        // Delegate to the underlying database.
        db->insertRecord(id, std::move(value));
        if (ttlSeconds > 0)
        {
            auto record = db->getRecord(id);
            if (record)
            {
                record->setTTL(ttlSeconds);
            }
            auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
            {
                std::lock_guard<std::mutex> lock(ttlMutex);
                ttlMap[id] = expireTime;
            }
        }
    }

    void updateRecord(const std::string &id, std::unique_ptr<IValue> value) override
    {
        // Delegate to the underlying database.
        db->updateRecord(id, std::move(value));
        // No TTL provided, so no action needed.
    }

    // Update a record and optionally update its TTL.
    void updateRecord(const std::string &id, std::unique_ptr<IValue> value, size_t ttlSeconds) override
    {
        db->updateRecord(id, std::move(value));
        if (ttlSeconds > 0)
        {
            auto record = db->getRecord(id);
            if (record)
            {
                record->setTTL(ttlSeconds);
            }
            auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
            {
                std::lock_guard<std::mutex> lock(ttlMutex);
                ttlMap[id] = expireTime;
            }
        }
        else
        {
            // If no TTL provided, remove any existing TTL.
            std::lock_guard<std::mutex> lock(ttlMutex);
            ttlMap.erase(id);
        }
    }

    void setTTL(const std::string &id, int ttlSeconds) override
    {
        auto record = db->getRecord(id);
        if (record)
        {
            record->setTTL(ttlSeconds);
            auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
            std::lock_guard<std::mutex> lock(ttlMutex);
            ttlMap[id] = expireTime;
        }
        else
        {
            throw std::runtime_error("Record with id '" + id + "' not found.");
        }
    }

    void removeTTL(const std::string &id) override
    {
        auto record = db->getRecord(id);
        if (record)
        {
            record->clearTTL();
            std::lock_guard<std::mutex> lock(ttlMutex);
            ttlMap.erase(id);
        }
        else
        {
            throw std::runtime_error("Record with id '" + id + "' not found.");
        }
    }

    // Get a record by id, checking for lazy expiration.
    std::shared_ptr<Record> getRecord(const std::string &id) override
    {
        {
            std::lock_guard<std::mutex> lock(ttlMutex);
            auto it = ttlMap.find(id);
            if (it != ttlMap.end() && std::chrono::steady_clock::now() > it->second)
            {
                // Expired: remove the record.
                db->deleteRecord(id);
                ttlMap.erase(it);
                throw std::runtime_error("Record with id '" + id + "' has expired.");
            }
        }
        return db->getRecord(id);
    }

    // Delete a record.
    void deleteRecord(const std::string &id) override
    {
        db->deleteRecord(id);
        std::lock_guard<std::mutex> lock(ttlMutex);
        ttlMap.erase(id);
    }

    // Get current cache size (delegated).
    size_t getCacheSize() const override
    {
        return db->getCacheSize();
    }

    // For snapshot and persistence purposes, allow access to all records.
    std::unordered_map<std::string, std::shared_ptr<Record>> getAllRecords() const override
    {
        return db->getAllRecords();
    }

    // Active expiration: remove expired keys. (Called by the expiration worker.)
    void removeExpiredKeys()
    {
        std::vector<std::string> expired;
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lock(ttlMutex);
            for (const auto &entry : ttlMap)
            {
                if (now > entry.second)
                {
                    expired.push_back(entry.first);
                }
            }
            for (const auto &id : expired)
            {
                std::cout << "Key expired: " << id << "\n";
                ttlMap.erase(id);
            }
        }
        // Delete expired keys from the underlying database.
        for (const auto &id : expired)
        {
            try
            {
                db->deleteRecord(id);
            }
            catch (...)
            {
                // Ignore if record already removed.
            }
        }
    }

private:
    std::unique_ptr<IDatabase> db;

    // TTL map: maps record id to expiration time.
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> ttlMap;
    mutable std::mutex ttlMutex;

    // Background expiration thread.
    std::thread expirationThread;
    std::atomic<bool> running;

    const int expirationInterval = 10; // Check for expired keys every second.

    // The worker function for the expiration thread.
    void expirationWorker()
    {
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(expirationInterval));
            removeExpiredKeys();
        }
    }
};

#endif // TTL_DATABASE_DECORATOR_H
