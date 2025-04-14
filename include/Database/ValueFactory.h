#ifndef VALUE_FACTORY_H
#define VALUE_FACTORY_H

#include "IValue.h"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
// Include other value type headers when implemented.

class ValueType
{
public:
    static const std::string STRING;
    static const std::string HASH;
    static const std::string LIST;
    static const std::string SET;
    static const std::string SORTED_SET;
    static const std::string HYPER_LOG_LOG;
    static const std::string STREAM;
};

class ValueFactory
{
public:
    // Create an IValue instance from a type string and a binary buffer.
    static std::unique_ptr<IValue> createValue(const std::string &type, const std::vector<uint8_t> &data);

    // Alternatively, create a value from a raw string. This is useful when commands insert new values.
    static std::unique_ptr<IValue> createValue(const std::string &type, const std::any &rawValue);
};

#endif // VALUE_FACTORY_H
