#ifndef SISMEMBER_COMMAND_H
#define SISMEMBER_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISetValue.h"
#include "../../../ValueFactory.h"

class SISMEMBERCommand : public ICommand
{
public:
    // Expects: SISMEMBER <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 3)
        {
            return "ERR wrong usage of SISMEMBER command. Expected SISMEMBER <key> <value>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            auto setValue = dynamic_cast<ISetValue *>(record->getValue());
            if (!setValue)
            {
                return "ERR " + key + " is not a set\n";
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
};

#endif // SISMEMBER_COMMAND_H