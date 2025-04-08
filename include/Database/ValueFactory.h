#ifndef VALUE_FACTORY_H
#define VALUE_FACTORY_H

#include "IValue.h"
#include "ConcreteValues/StringValue.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <stdexcept>
// Include other value type headers when implemented.

class ValueType
{
public:
    static const std::string STRING;
    static const std::string LIST;
    static const std::string SET;
};

// Outside the class definition
const std::string ValueType::STRING = "string";
const std::string ValueType::LIST = "list";
const std::string ValueType::SET = "set";

class ValueFactory
{
public:
    // Create an IValue instance from a type string and a binary buffer.
    static std::unique_ptr<IValue> createValue(std::string type, std::vector<uint8_t> &data)
    {
        if (type == ValueType::STRING)
        {
            std::unique_ptr<IValue> val = std::make_unique<StringValue>();
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
    static std::unique_ptr<IValue> createValue(std::string type, const std::string &rawValue)
    {
        if (type == ValueType::STRING)
        {
            return std::make_unique<StringValue>(rawValue);
        }
        // TODO: Handle other types like LIST and SET when implemented.
        else
        {
            throw std::invalid_argument("Unknown value type: " + type);
        }
    }
};

#endif // VALUE_FACTORY_H
