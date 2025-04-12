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
        std::string result = "{";
        bool first = true;
        for (const auto &p : value)
        {
            if (!first)
                result += ", ";
            result += "\"" + p.first + "\": \"" + p.second + "\"";
            first = false;
        }
        result += "}";
        return result;
    }

    const std::any &get() const override
    {
        return value;
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
            throw std::runtime_error("Failed to deserialize StringValue");
        }
    }

private:
    std::unordered_map<std::string, std::string> value;
};

#endif // HASHVALUE_H