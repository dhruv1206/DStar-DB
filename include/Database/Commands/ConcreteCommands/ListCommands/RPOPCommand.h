#ifndef RPOP_COMMAND_H
#define RPOP_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IListValue.h"
#include <string>
#include <vector>
#include <exception>

class RPOPCommand : public ICommand
{
public:
    // Expects: RPOP <key>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of RPOP command. Expected RPOP <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            if (!record->getValue() || record->getValue()->getType() != "list")
            {
                return "ERR record is not a list\n";
            }
            auto listValue = dynamic_cast<IListValue *>(record->getValue());
            if (!listValue)
            {
                return "ERR internal type conversion error in RPOP\n";
            }
            std::string value = listValue->rpop();
            return value + "\n";
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // RPOP_COMMAND_H