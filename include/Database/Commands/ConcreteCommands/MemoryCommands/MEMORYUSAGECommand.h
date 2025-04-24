#pragma once

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../../MemoryManager/MemoryManager.h"
#include <vector>
#include <memory>

class MEMORYUSAGECommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        try
        {
            if (tokens.size() < 2)
            {
                return "ERR wrong usage of MEMORY USAGE command, expected MEMORY USAGE [key]\n";
            }
            if (tokens.size() == 2)
            {
                return formatSize(MemoryManager::currentUsageBytes.load()) + '\n'; // Use load() to get the current value
            }
            std::string key = tokens[2];
            std::shared_ptr<Record> value = db->getRecord(key, client);
            size_t sizeInBytes = value->approximateSize();
            return formatSize(sizeInBytes) + "\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        catch (...)
        {
            return "ERR unknown error\n";
        }
    }

private:
    std::string formatSize(size_t sizeInBytes)
    {
        if (sizeInBytes < 1024)
        {
            return std::to_string(sizeInBytes) + " bytes";
        }
        else if (sizeInBytes < 1024 * 1024)
        {
            return std::to_string(sizeInBytes / 1024.0) + " KB" + " (" + std::to_string(sizeInBytes) + " bytes)";
        }
        else if (sizeInBytes < 1024 * 1024 * 1024)
        {
            return std::to_string(sizeInBytes / (1024.0 * 1024.0)) + " MB" + " (" + std::to_string(sizeInBytes) + " bytes)";
        }
        else
        {
            return std::to_string(sizeInBytes / (1024.0 * 1024.0 * 1024.0)) + " GB" + " (" + std::to_string(sizeInBytes) + " bytes)";
        }
    }
};