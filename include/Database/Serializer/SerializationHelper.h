
#ifndef SERIALIZATION_HELPER_H
#define SERIALIZATION_HELPER_H

#include "../ValueFactory.h"
#include <vector>
#include <cstdint>
#include <string>

class SerializationHelper
{

public:
    // Helper function to append a std::string to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, IValue *value)
    {
        std::vector<uint8_t> serializedValue = value->serialize();
        std::string type = value->getType();
        size_t typeLen = type.size();
        appendToBuffer(buffer, typeLen);                       // Store the length of the type string
        buffer.insert(buffer.end(), type.begin(), type.end()); // Append the type string
        size_t len = serializedValue.size();
        appendToBuffer(buffer, len);                                                 // Store the length of the serialized value
        buffer.insert(buffer.end(), serializedValue.begin(), serializedValue.end()); // Append the serialized value
    }

    // Helper to read a std::string from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::unique_ptr<IValue> &value)
    {
        std::string type;
        if (!readFromBuffer(buffer, offset, type))
            return false;
        size_t len = 0;
        if (!readFromBuffer(buffer, offset, len))
            return false;
        std::vector<uint8_t> serializedValue(buffer.begin() + offset, buffer.begin() + offset + len);
        std::unique_ptr<IValue> valPtr = ValueFactory::createValue(type, serializedValue);
        value = std::move(valPtr); // Create the value from the type and serialized data
        offset += len;                                                       // Update the offset
        return true;
    }
    // Helper function to append a size_t value to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, size_t val)
    {
        uint8_t *p = reinterpret_cast<uint8_t *>(&val);
        buffer.insert(buffer.end(), p, p + sizeof(size_t));
    }

    // Helper function to append a std::string to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, const std::string &str)
    {
        size_t len = str.size();
        appendToBuffer(buffer, len);
        buffer.insert(buffer.end(), str.begin(), str.end());
    }

    // Helper to read a size_t value from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, size_t &val)
    {
        if (offset + sizeof(size_t) > buffer.size())
            return false;
        val = *reinterpret_cast<const size_t *>(&buffer[offset]);
        offset += sizeof(size_t);
        return true;
    }

    // Helper to read a std::string from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::string &str)
    {
        size_t len = 0;
        if (!readFromBuffer(buffer, offset, len))
            return false;
        if (offset + len > buffer.size())
            return false;
        str = std::string(buffer.begin() + offset, buffer.begin() + offset + len);
        offset += len;
        return true;
    }
};

#endif // SERIALIZATION_HELPER_H
