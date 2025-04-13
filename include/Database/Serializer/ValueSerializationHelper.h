
#ifndef VALUE_SERIALIZATION_HELPER_H
#define VALUE_SERIALIZATION_HELPER_H

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>

class ValueFactory;

class ValueSerializationHelper
{

public:
    // Helper function to append a size_t value to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, size_t val);

    // Helper function to append a std::string to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, const std::string &str);

    // Helper function to append a hash value (unordered_map) to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, const std::unordered_map<std::string, std::string> &hash);

    // Helper function to append a hash value (vector) to the buffer.
    static void appendToBuffer(std::vector<uint8_t> &buffer, const std::vector<std::string> &hash);

    // Helper to read a size_t value from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, size_t &val);

    // Helper to read a std::string from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::string &str);

    // Helper to read a hash value (unordered_map) from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::unordered_map<std::string, std::string> &hash);

    // Helper to read a hash value (vector) from the buffer.
    static bool readFromBuffer(const std::vector<uint8_t> &buffer, size_t &offset, std::vector<std::string> &hash);
};

#endif // VALUE_SERIALIZATION_HELPER_H
