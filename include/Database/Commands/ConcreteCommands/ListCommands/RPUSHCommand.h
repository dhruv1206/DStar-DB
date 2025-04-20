#ifndef RPUSH_COMMAND_H
#define RPUSH_COMMAND_H

#include "../../../IDatabase.h"
#include "../../../ValueFactory.h"
#include "../../ICommand.h"
#include "../../../IListValue.h"
#include <string>
#include <vector>

class RPUSHCommand : public ICommand
{
public:
    // Expects: RPUSH <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of RPUSH command. Expected RPUSH <key> <value> [<value> ...]\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                auto listValue = dynamic_cast<IListValue *>(record->getValue());
                if (!listValue)
                {
                    return "ERR " + key + " is not a list\n";
                }
                listValue->rpush(tokens[i]);
            }
        }
        catch (const std::exception &e)
        {
            std::unique_ptr<IValue> listValue = ValueFactory::createValue(ValueType::LIST, std::vector<std::string>());
            auto listValuePtr = dynamic_cast<IListValue *>(listValue.get());
            if (!listValuePtr)
            {
                return "ERR failed to create list value\n";
            }
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                listValuePtr->rpush(tokens[i]);
            }
            db->insertRecord(key, std::move(listValue), client);
        }
        return "OK\n";
    }
};

#endif // RPUSH_COMMAND_H
