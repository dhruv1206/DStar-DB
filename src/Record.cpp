#include "../include/Database/Record.h"
#include <shared_mutex>
#include <mutex>

Record::Record(const std::string id) : id(id) {}

const std::string &Record::getId() const
{
    return id;
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
    // A simple estimation: base size plus value size.
    size_t size = sizeof(Record) + id.size();
    if (value)
    {
        size += value->toString().size();
    }
    return size;
}