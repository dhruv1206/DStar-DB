#ifndef SADD_COMMAND_H
#define SADD_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISetValue.h"
#include "../../../ValueFactory.h"
#include <unordered_set>

class SADDCommand : public ICommand
{
public:
    // Expects: SADD <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of SADD command. Expected SADD <key> <value> [<value> ...]\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                auto setValue = dynamic_cast<ISetValue *>(record->getValue());
                if (!setValue)
                {
                    return "ERR " + key + " is not a set\n";
                }
                setValue->sadd(tokens[i]);
            }
        }
        catch (const std::exception &e)
        {
            std::unique_ptr<IValue> setValue = ValueFactory::createValue(ValueType::SET, std::unordered_set<std::string>());
            auto setValuePtr = dynamic_cast<ISetValue *>(setValue.get());
            if (!setValuePtr)
            {
                return "ERR failed to create set value\n";
            }
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                setValuePtr->sadd(tokens[i]);
            }
            db->insertRecord(key, std::move(setValue), client);
        }
        return "OK\n";
    }
};

#endif // SADD_COMMAND_H