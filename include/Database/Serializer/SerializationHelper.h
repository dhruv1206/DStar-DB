#include <vector>
#include <cstdint>
#include <string>

#ifndef SERIALIZATION_HELPER_H
#define SERIALIZATION_HELPER_H

class SerializationHelper
{
public:
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
