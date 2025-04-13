#ifndef ZADD_COMMAND_H
#define ZADD_COMMAND_H

#include "../../ICommand.h"
#include "../../../ISortedSetValue.h"
#include "../../../ValueFactory.h"
#include <set>
#include <unordered_set>

class ZADDCommand : public ICommand
{
public:
    // Expects: ZADD <key> <value> [<value> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of ZADD command. Expected ZADD <key> <value> [<value> ...]\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                auto setValue = dynamic_cast<ISortedSetValue *>(record->getValue());
                if (!setValue)
                {
                    return "ERR " + key + " is not a sorted set\n";
                }
                setValue->sadd(tokens[i]);
            }
        }
        catch (const std::exception &e)
        {
            std::unique_ptr<IValue> setValue = ValueFactory::createValue(ValueType::SORTED_SET, std::set<std::string>());
            auto setValuePtr = dynamic_cast<ISortedSetValue *>(setValue.get());
            if (!setValuePtr)
            {
                return "ERR failed to create sorted set value\n";
            }
            for (size_t i = 2; i < tokens.size(); ++i)
            {
                setValuePtr->sadd(tokens[i]);
            }
            db->insertRecord(key, std::move(setValue));
        }
        return "OK\n";
    }
};

#endif // ZADD_COMMAND_H