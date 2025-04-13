#ifndef LLEN_COMMAND_H
#define LLEN_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IListValue.h"
#include <string>
#include <vector>
#include <exception>

class LLENCommand : public ICommand
{
public:
    // Expects: LLEN <key>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of LLEN command. Expected LLEN <key>\n";
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
                return "ERR internal type conversion error in LLEN\n";
            }
            size_t value = listValue->size();
            return std::to_string(value) + "\n";
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // LLEN_COMMAND_H