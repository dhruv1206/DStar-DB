#ifndef ISTREAM_VALUE_H
#define ISTREAM_VALUE_H

#include "IValue.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "ConcreteValues/Stream/StreamMessage.h"

class IStreamValue : public IValue
{
public:
    virtual ~IStreamValue() = default;

    // Append a new message to the stream.
    virtual StreamId xadd(const std::unordered_map<std::string, std::string> &fields) = 0;

    // Return messages in the specified ID range [start, end] (inclusive).
    virtual std::vector<StreamMessage> xrange(const StreamId &start, const StreamId &end) const = 0;

    // Read messages starting after a given ID (for consumer groups, etc.).
    virtual std::vector<StreamMessage> xread(const StreamId &last_id, int count) const = 0;
};

#endif // ISTREAM_VALUE_H
