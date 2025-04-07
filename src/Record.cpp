#include "../include/Database/Record.h"
#include <shared_mutex>
#include <mutex>

Record::Record(const std::string id) : id(id) {}

const std::string &Record::getId() const
{
    return id;
}

void Record::removeField(const std::string fieldName)
{
    std::unique_lock lock(record_mutex);
    auto it = fields.find(fieldName);
    if (it != fields.end())
    {
        fields.erase(it);
    }
    else
    {
        throw std::runtime_error("Field not found: " + fieldName);
    }
}

void Record::incrementFrequency()
{
    frequency++;
}

size_t Record::getFrequency() const
{
    return frequency.load();
}

size_t Record::approximateSize() const
{
    // A rough approximation:
    // Base size is size of Record plus the id string size.
    size_t size = sizeof(Record) + id.size();
    // Add size of each field: field name length and a constant per field for the value.
    for (const auto &kv : fields)
    {
        size += kv.first.size() + 32; // assume 32 bytes per value as approximation
    }
    return size;
}
