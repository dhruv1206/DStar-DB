#ifndef SREM_COMMAND_H
#define SREM_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISetValue.h"

class SREMCommand : public ICommand
{
public:
    // Expects: SREM <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of SREM command. Expected SREM <key> <value> [<value> ...]\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            auto setValue = dynamic_cast<ISetValue *>(record->getValue());
            if (!setValue)
            {
                return "ERR " + key + " is not a set\n";
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

#endif // SREM_COMMAND_H