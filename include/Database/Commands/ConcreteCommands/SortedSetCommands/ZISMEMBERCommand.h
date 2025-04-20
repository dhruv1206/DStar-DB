#ifndef ZISMEMBER_COMMAND_H
#define ZISMEMBER_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISortedSetValue.h"

class ZISMEMBERCommand : public ICommand
{
public:
    // Expects: ZISMEMBER <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 3)
        {
            return "ERR wrong usage of ZISMEMBER command. Expected ZISMEMBER <key> <value>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            auto setValue = dynamic_cast<ISortedSetValue *>(record->getValue());
            if (!setValue)
            {
                return "ERR " + key + " is not a sorted set\n";
            }
            std::string member = tokens[2];
            if (setValue->sismember(member))
            {
                return "1\n"; // Member exists
            }
            else
            {
                return "0\n"; // Member does not exist
            }
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
    }
};

#endif // ZISMEMBER_COMMAND_H