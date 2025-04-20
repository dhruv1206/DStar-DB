#pragma once

#include "../../../../ClientManager/Client.h"
#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <vector>
#include <memory>

class DISCARDCommand : public ICommand
{
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 1)
        {
            return "ERR wrong number of arguments for 'DISCARD' command\n";
        }
        if (!client->transactionContext->inTransaction)
        {
            return "ERR DISCARD without MULTI\n";
        }

        client->clearTransaction(db);
        return "OK\n";
    }
};