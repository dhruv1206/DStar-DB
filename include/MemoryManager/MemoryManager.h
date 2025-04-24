#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <cstddef>
#include <string>
#include <list>
#include <unordered_map>
#include <memory>

// Forward declaration of Record.
class Record;

class MemoryManager
{
public:
    // cacheLimitMB is provided by the user.
    MemoryManager(size_t cacheLimitMB);

    // Called when a record is inserted.
    void insertKey(const std::string &id, size_t size);

    // Called when a record is accessed (to update its frequency).
    void touch(const std::string &id);

    // Called when a record is removed.
    void removeKey(const std::string &id);

    // Evict records until memory usage is within capacity.
    // 'records' is a reference to the database's record map.
    void evictIfNeeded(std::unordered_map<std::string, std::shared_ptr<Record>> &records);

    // Get current memory usage (in bytes)
    size_t getCurrentUsage() const;

    static size_t currentUsageBytes;
private:
    struct CacheNode
    {
        size_t freq;
        size_t size; // approximate size of the record
    };

    size_t capacityBytes;
    size_t minFreq;

    // Mapping from record id to its frequency and size.
    std::unordered_map<std::string, CacheNode> cacheNodes;
    // Mapping from frequency to list of keys with that frequency.
    std::unordered_map<size_t, std::list<std::string>> freqList;
    // Mapping from key to its iterator in the corresponding freqList.
    std::unordered_map<std::string, std::list<std::string>::iterator> keyIterators;
};

#endif // MEMORYMANAGER_H
