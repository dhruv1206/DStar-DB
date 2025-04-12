#include "../include/Database/ValueFactory.h"
#include "../include/Database/ConcreteValues/StringValue.h"
#include "../include/Database/ConcreteValues/HashValue.h"

const std::string ValueType::STRING = "string";
const std::string ValueType::HASH = "hash";
const std::string ValueType::LIST = "list";
const std::string ValueType::SET = "set";

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
    // TODO: Handle other types like LIST and SET when implemented.
    else
    {
        throw std::invalid_argument("Unknown value type: " + type);
    }
}

// Alternatively, create a value from a raw string. This is useful when commands insert new values.
std::unique_ptr<IValue> ValueFactory::createValue(const std::string &type, const std::string &rawValue)
{
    if (type == ValueType::STRING)
    {
        return std::make_unique<StringValue>(rawValue);
    }
    else if (type == ValueType::HASH)
    {
        return std::make_unique<HashValue>();
    }
    // TODO: Handle other types like LIST and SET when implemented.
    else
    {
        throw std::invalid_argument("Unknown value type: " + type);
    }
}