#ifndef SETCOMMAND_H
#define SETCOMMAND_H

#include "../../IDatabase.h"
#include "../ICommand.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <any>

class SETCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of SET command, expected SET <key> <value>\n";
        }
        std::string key = tokens[1];
        std::any value = std::any_cast<std::string>(command.substr(command.find(key) + key.length() + 1));
        try
        {
            db->insertRecord(key, value);
        }
        catch (const std::exception &e)
        {
            db->updateRecord(key, value);
        }

        return "OK\n";
    }
};

#endif // SETCOMMAND_H