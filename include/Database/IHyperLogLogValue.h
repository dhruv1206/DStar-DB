#ifndef IHYPERLOGLOG_VALUE_H
#define IHYPERLOGLOG_VALUE_H

#include "IValue.h"
#include <string>

class IHyperLogLogValue : public IValue
{
public:
    virtual ~IHyperLogLogValue() = default;
    // Add a member to the HyperLogLog set.
    virtual void add(const std::string &member) = 0;
    // Estimate the cardinality of the HyperLogLog set.
    virtual uint64_t estimate() const = 0;
};

#endif // IHYPERLOGLOG_VALUE_H
