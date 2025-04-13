#ifndef LISTVALUE_H
#define LISTVALUE_H

#include "../IListValue.h"
#include "../Serializer/ValueSerializationHelper.h"
#include <vector>
#include <bits/algorithmfwd.h>

class ListValue : public IListValue
{
public:
    explicit ListValue(const std::vector<std::string> &val) : value(val) {}
    ListValue() {}

    std::string getType() const override
    {
        return "list";
    }

    std::string toString() const override
    {
        if (value.empty())
            return "[]";
        std::string result = "[";
        bool first = true;
        for (const auto &item : value)
        {
            if (!first)
                result += ", ";
            result += "\"" + item + "\"";
            first = false;
        }
        result += "]";
        return result;
    }

    void lpush(const std::string &element) override
    {
        value.insert(value.begin(), element);
    }

    void rpush(const std::string &element) override
    {
        value.push_back(element);
    }

    std::string lpop() override
    {
        if (value.empty())
        {
            throw std::runtime_error("List is empty, cannot pop from left");
        }
        std::string element = value.front();
        value.erase(value.begin());
        return element;
    }

    std::string rpop() override
    {
        if (value.empty())
        {
            throw std::runtime_error("List is empty, cannot pop from right");
        }
        std::string element = value.back();
        value.pop_back();
        return element;
    }

    std::vector<std::string> range(int start, int stop) const override
    {
        if (start < 0)
            start += value.size();
        if (stop < 0)
            stop += value.size();
        if (start < 0 || stop < 0 || start >= value.size() || stop >= value.size())
        {
            throw std::out_of_range("Range out of bounds");
        }
        return std::vector<std::string>(value.begin() + start, value.begin() + stop + 1);
    }

    std::any get() const override
    {
        return value;
    }

    std::string get(const size_t index) const override
    {
        if (index >= value.size())
        {
            throw std::out_of_range("Index out of range for list value");
        }
        return value[index];
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
            throw std::runtime_error("Failed to deserialize ListValue");
        }
    }

    size_t size() const override
    {
        return value.size();
    }

private:
    std::vector<std::string> value;
};

#endif // LISTVALUE_H