#ifndef HGETALLCOMMAND_H
#define HGETALLCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IValue.h"
#include <iostream>
#include <string>
#include <vector>

class HGETALLCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of HGETALL command, expected HGETALL <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            if (record)
            {
                std::string ret = record->getValue()->toString();
                if (ret == "{}")
                {
                    return "Empty\n";
                }
                else
                {
                    return ret + "\n";
                }
            }
            else
            {
                return "ERR key does not exist\n";
            }
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        return "\n";
    }
};

#endif // HGETALLCOMMAND_H