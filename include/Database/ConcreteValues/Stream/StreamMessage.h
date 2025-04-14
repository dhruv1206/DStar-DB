#ifndef STREAMMESSAGE_H
#define STREAMMESSAGE_H

#include <stdexcept>
#include <unordered_map>
#include <string>
#include <cstdint>

struct StreamId
{
    uint64_t milliseconds;
    uint64_t sequenceNumber;

    bool operator==(const StreamId &other) const
    {
        return milliseconds == other.milliseconds && sequenceNumber == other.sequenceNumber;
    }
    bool operator<(const StreamId &other) const
    {
        return milliseconds < other.milliseconds || (milliseconds == other.milliseconds && sequenceNumber < other.sequenceNumber);
    }
    bool operator>(const StreamId &other) const
    {
        return milliseconds > other.milliseconds || (milliseconds == other.milliseconds && sequenceNumber > other.sequenceNumber);
    }

    bool operator<=(const StreamId &other) const
    {
        return !(*this > other);
    }
    bool operator>=(const StreamId &other) const
    {
        return !(*this < other);
    }
    bool operator!=(const StreamId &other) const
    {
        return !(*this == other);
    }
    std::string toString() const
    {
        return std::to_string(milliseconds) + "-" + std::to_string(sequenceNumber);
    }

    static StreamId parse(const std::string &str)
    {
        size_t pos = str.find('-');
        if (pos == std::string::npos)
        {
            throw std::invalid_argument("Invalid StreamId format");
        }
        uint64_t milliseconds = std::stoull(str.substr(0, pos));
        uint64_t sequenceNumber = std::stoull(str.substr(pos + 1));
        return {milliseconds, sequenceNumber};
    }
};

namespace std
{
    template <>
    struct hash<StreamId>
    {
        std::size_t operator()(const StreamId &id) const
        {
            return std::hash<uint64_t>()(id.milliseconds) ^ (std::hash<uint64_t>()(id.sequenceNumber) << 1);
        }
    };
}

struct StreamMessage
{
    StreamId id;
    std::unordered_map<std::string, std::string> fields;
};

#endif // STREAMMESSAGE_H