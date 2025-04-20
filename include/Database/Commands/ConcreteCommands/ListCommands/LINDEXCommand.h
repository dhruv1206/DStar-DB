#ifndef LINDEX_COMMAND_H
#define LINDEX_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IListValue.h"
#include <string>
#include <vector>
#include <exception>

class LINDEXCommand : public ICommand
{
public:
    // Expects: LINDEX <key>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 3)
        {
            return "ERR wrong usage of LINDEX command. Expected LINDEX <key> <index>\n";
        }
        std::string key = tokens[1];
        std::string index = tokens[2];
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
                return "ERR internal type conversion error in LINDEX\n";
            }
            size_t idx = std::stoul(index);
            return listValue->get(idx) + "\n";
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // LINDEX_COMMAND_H