#ifndef STRING_VALUE_H
#define STRING_VALUE_H

#include "../IValue.h"
#include "../Serializer/ValueSerializationHelper.h"
#include <string>

class StringValue : public IValue
{
public:
    explicit StringValue(const std::string &val) : value(val) {}
    StringValue() : value("") {}

    std::string getType() const override
    {
        return "string";
    }

    std::string toString() const override
    {
        return value;
    }

    std::any get() const override
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

    size_t sizeInBytes() const override
    {
        // String capacity + actual string data + small overhead
        return sizeof(StringValue) + value.capacity() + 1; // +1 for null terminator
    }

private:
    std::string value;
};

#endif // STRING_VALUE_H
