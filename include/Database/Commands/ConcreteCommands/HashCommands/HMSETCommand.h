#ifndef HMSETCOMMAND_H
#define HMSETCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IValue.h"
#include "../../../ConcreteValues/HashValue.h"
#include "../../../ValueFactory.h"
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <any>

class HMSETCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 4 || tokens.size() % 2 != 0)
        {
            return "ERR wrong usage of HMSET command, expected HMSET <key> <field1> <value1> <field2> <value2> ...\n";
        }
        try
        {
            std::string key = tokens[1];
            std::shared_ptr<Record> record;
            try
            {
                record = db->getRecord(key);
            }
            catch (const std::exception &e)
            {
                record = nullptr;
            }

            if (!record)
            {
                std::unique_ptr<IValue> hashValue = ValueFactory::createValue(ValueType::HASH);
                for (size_t i = 2; i < tokens.size(); i += 2)
                {
                    std::string field = tokens[i];
                    std::string valueStr = tokens[i + 1];
                    auto hashValuePtr = dynamic_cast<HashValue *>(hashValue.get());
                    if (!hashValuePtr)
                    {
                        return "ERR wrong type of value\n";
                    }
                    hashValuePtr->set(field, valueStr);
                }
                db->insertRecord(key, std::move(hashValue));
            }
            else
            {
                for (size_t i = 2; i < tokens.size(); i += 2)
                {
                    std::string field = tokens[i];
                    std::string valueStr = tokens[i + 1];
                    dynamic_cast<HashValue *>(record->getValue())->set(field, valueStr);
                }
            }
        }
        catch (const std::bad_any_cast &e)
        {
            return "ERR wrong type of value\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        catch (...)
        {
            return "ERR unknown error\n";
        }
        return "OK\n";
    }
};

#endif // HMSETCOMMAND_H