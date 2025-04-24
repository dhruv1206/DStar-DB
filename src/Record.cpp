#include "../include/Database/Record.h"

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
    // Base size of the Record object itself
    size_t totalSize = sizeof(Record);

    // Size of the id string (capacity + actual string data + null terminator)
    totalSize += id.capacity() + 1;

    // Size of the unique_ptr itself is already counted in sizeof(Record)
    // But we need to add the size of the pointed-to value if it exists
    if (value)
    {
        // Add the size of the actual value object using its size() method
        totalSize += value->sizeInBytes();
    }

    // Other members like atomic variables, mutex, etc. are already
    // counted in sizeof(Record)

    return totalSize;
}