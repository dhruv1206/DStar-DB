#ifndef EXISTSCOMMAND_H
#define EXISTSCOMMAND_H

#include "../../IDatabase.h"
#include <string>
#include "../ICommand.h"
#include <sstream>
#include <vector>
#include <iostream>

class EXISTSCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of EXISTS command, expected EXISTS <key>\n";
        }

        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            return record ? "1\n" : "0\n"; // Record found, return 1; else return 0.
        }
        catch (const std::exception &e)
        {
            return "0\n"; // Record not found, return 0.
        }
    }
};

#endif // EXISTSCOMMAND_H
