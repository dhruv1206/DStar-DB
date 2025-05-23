#ifndef SETCOMMAND_H
#define SETCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../ValueFactory.h"
#include <string>
#include <vector>
#include <any>

class SETCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of SET command, expected SET <key> <value>\n";
        }
        std::string key = tokens[1];
        std::string valueStr = command.substr(command.find(key) + key.length() + 1);
        auto value = ValueFactory::createValue(ValueType::STRING, valueStr);
        db->updateRecord(key, std::move(value), client);
        return "OK\n";
    }
};

#endif // SETCOMMAND_H
