#ifndef SET_VALUE_H
#define SET_VALUE_H

#include "../ISetValue.h"
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include "../Serializer/ValueSerializationHelper.h"

class SetValue : public ISetValue
{
public:
    explicit SetValue(const std::unordered_set<std::string> &val) : data(val) {}
    SetValue() {}

    std::string getType() const override
    {
        return "set";
    }

    // Return a string representation of the set (for debugging).
    std::string toString() const override
    {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto &member : data)
        {
            if (!first)
                oss << ", ";
            oss << member;
            first = false;
        }
        oss << "}";
        return oss.str();
    }

    // Add a member to the set.
    void sadd(const std::string &member) override
    {
        data.insert(member);
    }

    // Get the value as std::any.
    std::any get() const override
    {
        return data;
    }

    // Remove a member from the set.
    bool srem(const std::string &member) override
    {
        return data.erase(member) > 0;
    }

    // Check if a member exists.
    bool sismember(const std::string &member) const override
    {
        return data.find(member) != data.end();
    }

    // Return all members.
    std::vector<std::string> smembers() const override
    {
        return std::vector<std::string>(data.begin(), data.end());
    }

    // Serialize the set.
    // Format: [number_of_members][member1_length][member1][member2_length][member2]...
    std::vector<uint8_t> serialize() const override
    {
        std::vector<uint8_t> buffer;
        ValueSerializationHelper::appendToBuffer(buffer, smembers());
        return buffer;
    }

    // Deserialize the set.
    void deserialize(const std::vector<uint8_t> &buffer) override
    {
        std::vector<std::string> members;
        size_t offset = 0;
        if (!ValueSerializationHelper::readFromBuffer(buffer, offset, members))
        {
            throw std::runtime_error("Failed to deserialize set value");
        }
        data.clear();
        for (const auto &member : members)
        {
            data.insert(member);
        }
    }

    size_t size() const override
    {
        return data.size();
    }

    size_t sizeInBytes() const override
    {
        // Base size of the unordered_set
        size_t totalSize = sizeof(SetValue);

        // Add size of all elements plus node overhead
        // Unordered_set has roughly 2 pointers per entry overhead
        constexpr size_t NODE_OVERHEAD = 2 * sizeof(void *);

        for (const auto &str : data)
        {
            // String capacity + actual string data + small overhead
            totalSize += str.capacity() + 1; // +1 for null terminator

            // Node overhead for this entry
            totalSize += NODE_OVERHEAD;
        }

        // Hashtable overhead (buckets array)
        totalSize += data.bucket_count() * sizeof(void *);

        return totalSize;
    }

private:
    std::unordered_set<std::string> data;
};

#endif // SET_VALUE_H
