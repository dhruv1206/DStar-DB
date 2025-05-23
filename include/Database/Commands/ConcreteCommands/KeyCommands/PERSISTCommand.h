#ifndef PERSISTCOMMAND_H
#define PERSISTCOMMAND_H

#include "../../../IDatabase.h"
#include <string>
#include "../../ICommand.h"
#include <vector>

class PERSISTCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of PERSIST command, expected PERSIST <key>\n";
        }

        std::string key = tokens[1];
        try
        {
            db->removeTTL(key, client);
            return "OK\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
    }
};

#endif // PERSISTCOMMAND_H
