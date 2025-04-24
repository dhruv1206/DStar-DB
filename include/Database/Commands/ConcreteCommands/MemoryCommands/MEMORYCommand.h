#pragma once

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <vector>
#include <memory>
#include "MEMORYUSAGECommand.h"

class MEMORYCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 2)
        {
            return "ERR wrong usage of MEMORY command, expected MEMORY <subcommand> <args>\n";
        }
        std::string subCommand = tokens[1];
        if (subCommand == "USAGE")
        {
            return MEMORYUSAGECommand().execute(tokens, command, db, client);
        }
        else
        {
            return "ERR unknown subcommand " + subCommand + "\n";
        }
    }
};