#ifndef SCARD_COMMAND_H
#define SCARD_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISetValue.h"
#include <string>

class SCARDCommand : public ICommand
{
public:
    // Expects: SCARD <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of SCARD command. Expected SCARD <key>\n";
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
            return std::to_string(setValue->size()) + "\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + key + " does not exist\n";
        }
};

#endif // SCARD_COMMAND_H