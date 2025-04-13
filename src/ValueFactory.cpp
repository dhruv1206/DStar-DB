#include "../include/Database/ValueFactory.h"
#include "../include/Database/ConcreteValues/StringValue.h"
#include "../include/Database/ConcreteValues/HashValue.h"
#include "../include/Database/ConcreteValues/ListValue.h"
#include "../include/Database/ConcreteValues/SetValue.h"
#include "../include/Database/ConcreteValues/SortedSetValue.h"
#include <unordered_set>

const std::string ValueType::STRING = "string";
const std::string ValueType::HASH = "hash";
const std::string ValueType::LIST = "list";
const std::string ValueType::SET = "set";
const std::string ValueType::SORTED_SET = "sorted_set";

std::unique_ptr<IValue> ValueFactory::createValue(const std::string &type, const std::vector<uint8_t> &data)
{
    std::unique_ptr<IValue> val;
    if (type == ValueType::STRING)
    {
        val = std::make_unique<StringValue>();
        val->deserialize(data);
        return val;
    }
    else if (type == ValueType::HASH)
    {
        val = std::make_unique<HashValue>();
        val->deserialize(data);
        return val;
    }
    else if (type == ValueType::LIST)
    {
        val = std::make_unique<ListValue>();
        val->deserialize(data);
        return val;
    }
    else if (type == ValueType::SET)
    {
        val = std::make_unique<SetValue>();
        val->deserialize(data);
        return val;
    }
    else if (type == ValueType::SORTED_SET)
    {
        val = std::make_unique<SortedSetValue>();
        val->deserialize(data);
        return val;
    }
    // TODO: Handle other types like LIST and SET when implemented.
    else
    {
        throw std::invalid_argument("Unknown value type: " + type);
    }
}

// Alternatively, create a value from a raw string. This is useful when commands insert new values.
std::unique_ptr<IValue> ValueFactory::createValue(const std::string &type, const std::any &rawValue)
{
    if (type == ValueType::STRING)
    {
        if (rawValue.type() != typeid(std::string))
        {
            throw std::invalid_argument("Expected a string for STRING type, but got a different type.");
        }
        return std::make_unique<StringValue>(std::any_cast<std::string>(rawValue));
    }
    else if (type == ValueType::HASH)
    {
        if (rawValue.type() != typeid(std::unordered_map<std::string, std::string>))
        {
            throw std::invalid_argument("Expected a map for HASH type, but got a different type.");
        }
        return std::make_unique<HashValue>(std::any_cast<std::unordered_map<std::string, std::string>>(rawValue));
    }
    else if (type == ValueType::LIST)
    {
        if (rawValue.type() != typeid(std::vector<std::string>))
        {
            throw std::invalid_argument("Expected a vector for LIST type, but got a different type.");
        }
        return std::make_unique<ListValue>(std::any_cast<std::vector<std::string>>(rawValue));
    }
    else if (type == ValueType::SET)
    {
        if (rawValue.type() != typeid(std::unordered_set<std::string>))
        {
            throw std::invalid_argument("Expected a set for SET type, but got a different type.");
        }
        return std::make_unique<SetValue>(std::any_cast<std::unordered_set<std::string>>(rawValue));
    }
    else if (type == ValueType::SORTED_SET)
    {
        if (rawValue.type() != typeid(std::set<std::string>))
        {
            throw std::invalid_argument("Expected a sorted set for SORTED_SET type, but got a different type.");
        }
        return std::make_unique<SortedSetValue>(std::any_cast<std::set<std::string>>(rawValue));
    }
    // TODO: Handle other types like LIST and SET when implemented.
    else
    {
        throw std::invalid_argument("Unknown value type: " + type);
    }
}