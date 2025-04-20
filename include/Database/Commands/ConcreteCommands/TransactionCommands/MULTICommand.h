#pragma once

#include "../../../../ClientManager/Client.h"
#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <vector>
#include <memory>

class MULTICommand : public ICommand
{
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 1)
        {
            return "ERR wrong number of arguments for 'MULTI' command\n";
        }
        if (client->transactionContext->inTransaction)
        {
            return "ERR MULTI calls can not be nested\n";
        }
        // Start a transaction
        client->transactionContext->commands = std::queue<std::function<std::string()>>();
        client->transactionContext->inTransaction = true;

        return "OK\n";
    }
};