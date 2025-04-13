
#include "../include/Database/Serializer/ValueSerializationHelper.h"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>

// Helper function to append a size_t value to the buffer.
void ValueSerializationHelper::appendToBuffer(std::vector<uint8_t> &buffer, size_t val)
{
    uint8_t *p = reinterpret_cast<uint8_t *>(&val);
    buffer.insert(buffer.end(), p, p + sizeof(size_t));
}

// Helper function to append a std::string to the buffer.
void ValueSerializationHelper::appendToBuffer(std::vector<uint8_t> &buffer, const std::string &str)
{
    size_t len = str.size();
    appendToBuffer(buffer, len);
    buffer.insert(buffer.end(), str.begin(), str.end());
}

// Helper function to append a hash value (unordered_map) to the buffer.
void ValueSerializationHelper::appendToBuffer(std::vector<uint8_t> &buffer, const std::unordered_map<std::string, std::string> &hash)
{
    size_t hashSize = hash.size();
    appendToBuffer(buffer, hashSize);
    for (const auto &pair : hash)
    {
        appendToBuffer(buffer, pair.first);
        appendToBuffer(buffer, pair.second);
    }
}

// Helper to read a size_t value from the buffer.
bool ValueSerializationHelper::readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, size_t &val)
{
    if (offset + sizeof(size_t) > buffer.size())
        return false;
    memcpy(&val, &buffer[offset], sizeof(size_t));
    offset += sizeof(size_t);
    return true;
}

// Helper to read a std::string from the buffer.
bool ValueSerializationHelper::readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::string &str)
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

// Helper to read a hash value (unordered_map) from the buffer.
bool ValueSerializationHelper::readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::unordered_map<std::string, std::string> &hash)
{
    size_t hashSize = 0;
    if (!readFromBuffer(buffer, offset, hashSize))
        return false;
    for (size_t i = 0; i < hashSize; ++i)
    {
        std::string key, value;
        if (!readFromBuffer(buffer, offset, key) || !readFromBuffer(buffer, offset, value))
            return false;
        hash[key] = value;
    }
    return true;
}