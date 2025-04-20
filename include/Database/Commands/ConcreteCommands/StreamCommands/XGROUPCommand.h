#ifndef XGROUP_COMMAND_H
#define XGROUP_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <vector>
#include <memory>
#include "XGROUPCREATECommand.h"

class XGROUPCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 2)
        {
            return "ERR wrong usage of XGROUP command, expected XGROUP <subcommand> <args>\n";
        }
        std::string subCommand = tokens[1];
        if (subCommand == "CREATE")
        {
            return XGROUPCREATECommand().execute(tokens, command, db, client);
        }
        else
        {
            return "ERR unknown subcommand " + subCommand + "\n";
        }
    }
};

#endif // XGROUP_COMMAND_H
