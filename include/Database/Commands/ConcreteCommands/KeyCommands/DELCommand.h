#ifndef DELCOMMAND_H
#define DELCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <any>

class DELCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 2)
        {
            return "ERR wrong usage of DEL command, expected DEL <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            db->deleteRecord(key);
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        return "OK\n";
    }
};

#endif // DELCOMMAND_H