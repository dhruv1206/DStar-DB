#ifndef ZMEMBERS_COMMAND_H
#define ZMEMBERS_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISortedSetValue.h"

class ZMEMBERSCommand : public ICommand
{
public:
    // Expects: ZMEMBERS <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of ZMEMBERS command. Expected ZMEMBERS <key>\n";
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
            return setValue->toString() + "\n"; // Return the string representation of the sorted set
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
    }
};

#endif // ZMEMBERS_COMMAND_H