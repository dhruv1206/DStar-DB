#ifndef IVALUE_H
#define IVALUE_H

#include <any>
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>

class IValue
{
public:
    virtual ~IValue() = default;

    // Returns a string identifying the type (e.g., "string", "list", "set", etc.)
    virtual std::string getType() const = 0;
    // Returns a string representation of the value.
    virtual std::string toString() const = 0;

    // Returns the value as a std::any object.
    virtual std::any get() const = 0;

    // Serialize the value into a binary buffer.
    virtual std::vector<uint8_t> serialize() const
    {
        throw std::runtime_error("Serialization not implemented for this type.");
    }
    // Reconstruct the value from a binary buffer.
    virtual void deserialize(const std::vector<uint8_t> &data)
    {
        throw std::runtime_error("Deserialization not implemented for this type.");
    }

    // Approximate memory usage of this value in bytes, including container overhead.
    virtual size_t sizeInBytes() const = 0;
};

#endif // IVALUE_H
