#ifndef GETCOMMAND_H
#define GETCOMMAND_H

#include "../../IDatabase.h"
#include <string>
#include "../ICommand.h"
#include <sstream>
#include <vector>
#include <iostream>

class GETCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of GET command, expected GET <key>\n";
        }

        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            std::string value = record->getField<std::string>("value");
            return "VALUE " + value + "\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
    }
};

#endif // GETCOMMAND_H
