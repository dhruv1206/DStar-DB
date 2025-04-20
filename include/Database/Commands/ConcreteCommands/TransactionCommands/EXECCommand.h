#pragma once

#include "../../../../ClientManager/Client.h"
#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <vector>
#include <memory>

class EXECCommand : public ICommand
{
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 1)
        {
            return "ERR wrong number of arguments for 'EXEC' command\n";
        }
        if (!client->transactionContext->inTransaction)
        {
            return "ERR EXEC without MULTI\n";
        }
        if (client->transactionContext->dirty)
        {
            client->clearTransaction(db);
            return "(nil)\n";
        }
        else
        {
            std::string response;
            {
                auto dbMutex = db->lockDatabase();
                while (!client->transactionContext->commands.empty())
                {
                    auto command = client->transactionContext->commands.front();
                    response += command() + "\n\r";
                    client->transactionContext->commands.pop();
                }
            }
            client->clearTransaction(db);
            return response.empty() ? "OK\n" : response;
        }
    }
};