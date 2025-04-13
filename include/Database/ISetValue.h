#ifndef ISET_VALUE_H
#define ISET_VALUE_H

#include "IValue.h"
#include <vector>
#include <string>

class ISetValue : public IValue
{
public:
    virtual ~ISetValue() = default;
    // Add a member to the set.
    virtual void sadd(const std::string &member) = 0;
    // Remove a member from the set.
    virtual bool srem(const std::string &member) = 0;
    // Check if a member exists in the set.
    virtual bool sismember(const std::string &member) const = 0;
    // Return all members of the set.
    virtual std::vector<std::string> smembers() const = 0;
    // Returns number of members
    virtual size_t size() const = 0;
};

#endif // ISET_VALUE_H
