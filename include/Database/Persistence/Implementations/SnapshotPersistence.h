#ifndef SNAPSHOT_PERSISTENCE_H
#define SNAPSHOT_PERSISTENCE_H

#include "../IPersistenceManager.h"
#include "../../IDatabase.h"
#include "../../Serializer/IDBSerializer.h"
#include "../../Serializer/Strategies/BinarySerializer.h"
#include <fstream>
#include <iostream>
#include <memory>

class SnapshotPersistence : public IPersistenceManager
{
public:
    SnapshotPersistence(
        std::unique_ptr<IDBSerializer> serializer) : serializer(std::move(serializer))
    {
    }
    ~SnapshotPersistence()
    {
        isRunning = false; // Stop the snapshot thread
        if (snapshotThread.joinable())
        {
            snapshotThread.join(); // Wait for the thread to finish
        }
    }

    // Load database state from snapshot file
    void load(IDatabase *db)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in)
        {
            std::cerr << "No snapshot found at " << filename << ". Starting with an empty database.\n";
            return;
        }
        std::cout << "Loading database from snapshot: " << filename << "\n";
        std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();
        serializer->deserialize(buffer, db); // Deserialize the data into the database
        std::cout << "Database loaded successfully from snapshot.\n";
    }

    void save(const IDatabase *db)
    {
        try
        {
            std::cout << "Saving database snapshot to " << filename << "\n";
            // Serialize the database into a binary buffer.
            std::vector<uint8_t> buffer = serializer->serialize(db);
            // Write atomically by writing to a temporary file first.
            std::string tempFilename = filename + ".tmp";
            std::ofstream out(tempFilename, std::ios::binary);
            if (!out)
            {
                std::cerr << "Failed to open " << tempFilename << " for writing.\n";
                return;
            }
            out.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
            out.flush();
            out.close();
            // Replace the old file with the new snapshot.
            if (std::rename(tempFilename.c_str(), filename.c_str()) != 0)
            {
                std::cerr << "Failed to rename temporary snapshot file to " << filename << "\n";
            }
            std::cout << "Database snapshot saved successfully.\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error saving snapshot: " << e.what() << "\n";
        }
        catch (...)
        {
            std::cerr << "Unknown error occurred while saving snapshot.\n";
        }
    }
    void initialize(IDatabase *db) override
    {
        snapshotThread = std::thread(&SnapshotPersistence::snapshotWorker, this, db, snapshotIntervalSeconds); // Save every intervalSeconds
        snapshotThread.detach();                                                                               // Detach the thread to run independently
    }

private:
    const size_t snapshotIntervalSeconds = 60; // Interval for saving the snapshot
    std::thread snapshotThread;
    std::unique_ptr<IDBSerializer> serializer;
    bool isRunning = true;
    void snapshotWorker(const IDatabase *db, int intervalSeconds)
    {
        while (isRunning)
        {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            save(db);
        }
    }
};

#endif // SNAPSHOT_PERSISTENCE_H