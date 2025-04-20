#pragma once

#include <functional>
#include <queue>
#include <unordered_set>
#include <string>

struct TransactionContext
{
    bool inTransaction = false;
    bool dirty = false;
    std::queue<std::function<std::string()>> commands;
    std::unordered_set<std::string> watchedRecords;

    void clear()
    {
        inTransaction = false;
        dirty = false;
        commands = std::queue<std::function<std::string()>>();
        watchedRecords.clear();
    }
};