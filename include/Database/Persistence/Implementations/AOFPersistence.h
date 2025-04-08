#ifndef AOFPERSISTENCE_H
#define AOFPERSISTENCE_H

#include "../IPersistenceManager.h"
#include "../../Observer/IDatabaseObserver.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <any>

class AOFPersistence : public IPersistenceManager, public IDatabaseObserver
{
public:
    AOFPersistence()
    {
        aofStream.open(filename, std::ios::app);
        if (!aofStream.is_open())
        {
            std::cerr << "Failed to open AOF file: " << filename << "\n";
        }
    }

    ~AOFPersistence()
    {
        if (aofStream.is_open())
        {
            aofStream.close();
        }
    }

    // IPersistenceManager methods.
    void load(IDatabase *db) override
    {
        // Replay the AOF log to rebuild the database.
        // (Pseudo-code, depends on how commands are stored.)
        std::ifstream in(filename);
        if (!in)
        {
            std::cerr << "No AOF log found, starting with empty database.\n";
            return;
        }
        std::string command;
        while (std::getline(in, command))
        {
            // Parse and execute command on db.
            // e.g., if command is "INSERT record1", then db.insertRecord("record1");
        }
        in.close();
    }

    void save(const IDatabase *db) override
    {
        // For AOF, saving is done per modification,
        // so we might not need to do anything here.
    }

    // IDatabaseObserver method.
    void onDatabaseModified(const std::string &operation, const std::string &recordId) override
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        if (aofStream.is_open())
        {
            // Write the operation and record ID to the file.
            aofStream << operation << " " << recordId << "\n";
            aofStream.flush();
        }
    }
    void onDatabaseModified(const std::string &operation, const std::string &recordId, std::shared_ptr<IValue> value) override
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        if (aofStream.is_open())
        {
            // Write the operation and record ID to the file.
            aofStream << operation << " " << recordId << "\n";
            aofStream.flush();
        }
    }

    void initialize(IDatabase *db) override
    {
        db->registerObserver(this); // Register this observer to receive updates
    }

private:
    std::ofstream aofStream;
    std::mutex writeMutex;
};

#endif // AOFPERSISTENCE_H