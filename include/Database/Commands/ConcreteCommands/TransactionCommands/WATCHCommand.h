#pragma once

#include "../../../../ClientManager/Client.h"
#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <vector>
#include <memory>

class WATCHCommand : public ICommand
{
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 2)
        {
            return "ERR wrong number of arguments for 'WATCH' command\n";
        }
        if (client->transactionContext->inTransaction)
        {
            return "ERR WATCH calls can not be nested\n";
        }
        client->transactionContext->clear();
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            client->transactionContext->watchedRecords.insert(tokens[i]);
            db->registerRecordObserver(tokens[i], client.get());
        }
        return "OK\n";
    }
};