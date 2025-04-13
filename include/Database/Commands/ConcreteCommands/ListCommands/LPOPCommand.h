#ifndef LPOP_COMMAND_H
#define LPOP_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IListValue.h"
#include <string>
#include <vector>
#include <exception>

class LPOPCommand : public ICommand
{
public:
    // Expects: LPOP <key>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of LPOP command. Expected LPOP <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            if (!record->getValue() || record->getValue()->getType() != "list")
            {
                return "ERR record is not a list\n";
            }
            auto listValue = dynamic_cast<IListValue *>(record->getValue());
            if (!listValue)
            {
                return "ERR internal type conversion error in LPOP\n";
            }
            std::string value = listValue->lpop();
            return value + "\n";
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // LPOP_COMMAND_H