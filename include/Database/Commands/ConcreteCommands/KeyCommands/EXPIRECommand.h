#ifndef EXPIRECOMMAND_H
#define EXPIRECOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <vector>

class EXPIRECommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of EXPIRE command, expected EXPIRE <key> <seconds>\n";
        }
        std::string key = tokens[1];
        int ttlSeconds = std::stoi(tokens[2]);
        try
        {
            db->setTTL(key, ttlSeconds, client);
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        return "OK\n";
    }
};

#endif // EXPIRECOMMAND_H
