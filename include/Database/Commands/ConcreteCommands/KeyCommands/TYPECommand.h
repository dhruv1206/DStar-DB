#ifndef TYPECOMMAND_H
#define TYPECOMMAND_H

#include "../../../IDatabase.h"
#include <string>
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include <iostream>

class TYPECommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of TYPE command, expected TYPE <key>\n";
        }

        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            if (!record)
            {
                return "ERR key does not exist\n"; // Return an error message if the key does not exist.
            }
            return record->getValue()->getType() + "\n"; // Return the type of the record.
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n"; // Handle the exception and return an error message.
        }
    }
};

#endif // TYPECOMMAND_H
