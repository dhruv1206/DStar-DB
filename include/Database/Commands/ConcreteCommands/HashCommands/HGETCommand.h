#ifndef HGETCOMMAND_H
#define HGETCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../ConcreteValues/HashValue.h"
#include <string>
#include <vector>

class HGETCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 3)
        {
            return "ERR wrong usage of HGET command, expected HGET <key> <field>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key, client);
            if (record)
            {
                const std::string field = tokens[2];
                auto hashValue = dynamic_cast<HashValue *>(record->getValue());
                if (hashValue)
                {
                    return hashValue->get(field) + "\n";
                }
                else
                {
                    return "ERR value is not a hash\n";
                }
            }
            else
            {
                return "ERR key does not exist\n";
            }
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }
        return "\n";
    }
};

#endif // HGETCOMMAND_H