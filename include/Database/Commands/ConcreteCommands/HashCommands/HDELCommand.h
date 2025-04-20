#ifndef HDELCOMMAND_H
#define HDELCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../ConcreteValues/HashValue.h"
#include <memory>
#include <string>
#include <vector>
#include <any>

class HDELCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of HDEL command, expected HDEL <key> <field1> <field2> ...\n";
        }
        try
        {
            std::string key = tokens[1];
            std::shared_ptr<Record> record;
            try
            {
                record = db->getRecord(key, client);
            }
            catch (const std::exception &e)
            {
                record = nullptr;
            }

            if (!record)
            {
                return "ERR key does not exist\n";
            }
            else
            {
                for (size_t i = 2; i < tokens.size(); i++)
                {
                    auto hashValue = dynamic_cast<HashValue *>(record->getValue());
                    if (hashValue)
                    {
                        hashValue->deleteField(tokens[i]);
                    }
                    else
                    {
                        return "ERR wrong type of value\n";
                    }
                }
                return "OK\n";
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

#endif // HDELCOMMAND_H