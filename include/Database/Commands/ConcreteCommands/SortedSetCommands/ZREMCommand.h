#ifndef ZREM_COMMAND_H
#define ZREM_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISortedSetValue.h"

class ZREMCommand : public ICommand
{
public:
    // Expects: ZREM <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of ZREM command. Expected ZREM <key> <value> [<value> ...]\n";
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
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                std::string member = tokens[i];
                setValue->srem(member);
            }
            return "OK\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
    }
};

#endif // ZREM_COMMAND_H