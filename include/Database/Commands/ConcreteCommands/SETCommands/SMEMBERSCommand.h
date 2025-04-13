#ifndef SMEMBERS_COMMAND_H
#define SMEMBERS_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISetValue.h"

class SMEMBERSCommand : public ICommand
{
public:
    // Expects: SMEMBERS <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of SMEMBERS command. Expected SMEMBERS <key>\n";
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
            return setValue->toString() + "\n"; // Return the string representation of the set
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
};

#endif // SMEMBERS_COMMAND_H