#include "../include/MemoryManager/MemoryManager.h"
#include "../include/Database/Record.h"

MemoryManager::MemoryManager(size_t cacheLimitMB)
    : capacityBytes(cacheLimitMB * 1024 * 1024), minFreq(0)
{
}

std::atomic<size_t> MemoryManager::currentUsageBytes = 0;

void MemoryManager::insertKey(const std::string &id, size_t size)
{
    // New key starts with frequency 1.
    CacheNode node{1, size};
    cacheNodes[id] = node;
    // Insert into frequency list for frequency 1.
    freqList[1].push_back(id);
    keyIterators[id] = --freqList[1].end();
    // Set minFreq to 1 since we just inserted a key with freq 1.
    minFreq = 1;
}

void MemoryManager::touch(const std::string &id)
{
    // Ensure the key exists.
    if (cacheNodes.find(id) == cacheNodes.end())
        return;

    size_t oldFreq = cacheNodes[id].freq;
    size_t newFreq = oldFreq + 1;
    cacheNodes[id].freq = newFreq;

    // Remove key from the old frequency list.
    auto it = keyIterators[id];
    freqList[oldFreq].erase(it);
    // If the old frequency list is empty and it was the minimum frequency, update minFreq.
    if (freqList[oldFreq].empty())
    {
        freqList.erase(oldFreq);
        if (minFreq == oldFreq)
            minFreq = newFreq;
    }

    // Add key to the new frequency list.
    freqList[newFreq].push_back(id);
    keyIterators[id] = --freqList[newFreq].end();
}

void MemoryManager::removeKey(const std::string &id)
{
    auto nodeIt = cacheNodes.find(id);
    if (nodeIt == cacheNodes.end())
        return;

    size_t freq = nodeIt->second.freq;
    size_t size = nodeIt->second.size;

    // Remove from frequency list.
    auto it = keyIterators[id];
    freqList[freq].erase(it);
    if (freqList[freq].empty())
        freqList.erase(freq);

    // Erase from maps.
    cacheNodes.erase(id);
    keyIterators.erase(id);
    if (minFreq == freq && freqList.find(freq) == freqList.end())
        minFreq = (freqList.empty() ? 0 : freqList.begin()->first);
}

size_t MemoryManager::getCurrentUsage() const
{
    return currentUsageBytes;
}

void MemoryManager::evictIfNeeded(std::unordered_map<std::string, std::shared_ptr<Record>> &records)
{
    // While we exceed capacity, evict keys with the lowest frequency.
    while (currentUsageBytes > capacityBytes && !freqList.empty())
    {
        // Get the list of keys with the minimum frequency.
        auto &keysList = freqList[minFreq];
        if (keysList.empty())
            break;
        // Evict the least recently used key from this frequency list (front of the list).
        std::string evictKey = keysList.front();
        keysList.pop_front();
        if (keysList.empty())
            freqList.erase(minFreq);
        // Remove from internal LFU structures.
        auto nodeIt = cacheNodes.find(evictKey);
        if (nodeIt != cacheNodes.end())
        {
            size_t recSize = nodeIt->second.size;
            cacheNodes.erase(evictKey);
            keyIterators.erase(evictKey);
        }
        // Also remove from the database records.
        records.erase(evictKey);
        // After eviction, update minFreq if needed.
        if (freqList.empty())
            minFreq = 0;
        else
            minFreq = freqList.begin()->first;
    }
}
