#ifndef ZCARD_COMMAND_H
#define ZCARD_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISortedSetValue.h"
#include <string>

class ZCARDCommand : public ICommand
{
public:
    // Expects: ZCARD <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of ZCARD command. Expected ZCARD <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            auto setValue = dynamic_cast<ISortedSetValue *>(record->getValue());
            if (!setValue)
            {
                return "ERR " + key + " is not a sorted set\n";
            }
            return std::to_string(setValue->size()) + "\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
};

#endif // ZCARD_COMMAND_H