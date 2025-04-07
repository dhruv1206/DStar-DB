#ifndef BINARY_SERIALIZER_H
#define BINARY_SERIALIZER_H

#include "../IDBSerializer.h"
#include "../SerializationHelper.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

class BinarySerializer : public IDBSerializer
{
public:
    BinarySerializer()
    {
        std::cout << "BinarySerializer initialized.\n";
    }

    // Serialize the database into a binary buffer
    std::vector<uint8_t> serialize(const IDatabase *db) override
    {
        if (!db)
        {
            std::cerr << "Database is null, cannot serialize.\n";
            return {};
        }
        std::vector<uint8_t> buffer;
        const std::unordered_map<std::string, std::shared_ptr<Record>> &records = db->getAllRecords();
        size_t records_count = records.size();
        std::cout << "Serializing database with " << records_count << " records...\n";
        SerializationHelper::appendToBuffer(buffer, records_count); // Store the number of records
        for (const auto &pair : records)
        {
            const std::string &id = pair.first;
            const std::shared_ptr<Record> &record = pair.second;
            SerializationHelper::appendToBuffer(buffer, id); // Store the record ID
            std::string value;
            try
            {
                value = pair.second->getField<std::string>("value");
            }
            catch (const std::bad_any_cast &e)
            {
                value = "";
            }
            SerializationHelper::appendToBuffer(buffer, value); // Store the record value
        }
        return buffer;
    }

    // Deserialize the binary data into the database
    void deserialize(const std::vector<uint8_t> &data, IDatabase *db) override
    {
        size_t offset = 0;
        size_t records_count = 0;

        if (!SerializationHelper::readFromBuffer(data, offset, records_count))
        {
            return;
        }
        std::cout << "Deserializing database with " << records_count << " records...\n";
        for (size_t i = 0; i < records_count; i++)
        {
            std::string record_id;
            if (!SerializationHelper::readFromBuffer(data, offset, record_id))
            {
                return;
            }
            std::string record_value;
            if (!SerializationHelper::readFromBuffer(data, offset, record_value))
            {
                return;
            }
            std::any value = record_value; // Convert to std::any
            try
            {
                std::cout << "Inserting record: " << record_id << " with value: " << record_value << "\n";
                db->insertRecord(record_id, value); // Insert the record into the database
            }
            catch (const std::exception &ex)
            {
                std::cerr << "Error inserting record: " << ex.what() << "\n";
            }
            catch (...)
            {
                std::cerr << "Unknown error inserting record\n";
            }
        }
    }

private:
};

#endif // BINARY_SERIALIZER_H