#ifndef IDBSERIALIZER_H
#define IDBSERIALIZER_H

#include <string>
#include <vector>
#include <cstdint>
#include "../IDatabase.h"

class IDBSerializer
{
public:
    virtual ~IDBSerializer() = default;

    // Serializes the database state into a binary buffer.
    virtual std::vector<uint8_t> serialize(const IDatabase *db) = 0;

    // Deserializes the binary data into the database.
    virtual void deserialize(const std::vector<uint8_t> &data, IDatabase *db) = 0;
};

#endif // IDBSERIALIZER_H