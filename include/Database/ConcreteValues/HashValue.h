#ifndef HASHVALUE_H
#define HASHVALUE_H

#include "../IValue.h"
#include "../Serializer/ValueSerializationHelper.h"
#include <unordered_map>

class HashValue : public IValue
{
public:
    explicit HashValue(const std::unordered_map<std::string, std::string> &val) : value(val) {}
    HashValue() {}

    std::string getType() const override
    {
        return "hash";
    }

    std::string toString() const override
    {
        if (value.empty())
            return "{}";
        std::string result = "{";
        bool first = true;
        for (const auto &pair : value)
        {
            if (!first)
                result += ", ";
            result += "\"" + pair.first + "\": \"" + pair.second + "\"";
            first = false;
        }
        result += "}";
        return result;
    }

    std::any get() const override
    {
        return value;
    }

    std::string get(const std::string &key) const
    {
        auto it = value.find(key);
        if (it != value.end())
        {
            return it->second;
        }
        else
        {
            throw std::runtime_error("Field not found in hash value");
        }
    }

    bool exists(const std::string &key) const
    {
        return value.find(key) != value.end();
    }

    void set(const std::string &key, const std::string &val)
    {
        value[key] = val;
    }

    void deleteField(const std::string &key)
    {
        if (value.find(key) != value.end())
        {
            value.erase(key);
        }
        else
        {
            throw std::runtime_error("Field not found in hash value");
        }
    }

    std::vector<uint8_t> serialize() const override
    {
        std::vector<uint8_t> buffer;
        // Use the helper to write the string to the buffer.
        ValueSerializationHelper::appendToBuffer(buffer, value);
        return buffer;
    }

    void deserialize(const std::vector<uint8_t> &data) override
    {
        size_t offset = 0;
        if (!ValueSerializationHelper::readFromBuffer(data, offset, value))
        {
            throw std::runtime_error("Failed to deserialize HashValue");
        }
    }

    size_t sizeInBytes() const override
    {
        // Base size of the unordered_map
        size_t totalSize = sizeof(HashValue);

        // Add size of all keys and values, plus node overhead
        // Unordered_map has roughly 3 pointers per entry overhead plus the entry itself
        constexpr size_t NODE_OVERHEAD = 3 * sizeof(void *);

        for (const auto &pair : value)
        {
            // Key string capacity + actual string data + small string overhead
            totalSize += pair.first.capacity() + 1; // +1 for null terminator

            // Value string capacity + actual string data + small string overhead
            totalSize += pair.second.capacity() + 1; // +1 for null terminator

            // Node overhead for this entry
            totalSize += NODE_OVERHEAD;
        }

        // Hashtable overhead (buckets array)
        totalSize += value.bucket_count() * sizeof(void *);

        return totalSize;
    }

private:
    std::unordered_map<std::string, std::string> value;
};

#endif // HASHVALUE_H