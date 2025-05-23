#ifndef INCRCOMMAND_H
#define INCRCOMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../ValueFactory.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <any>

class INCRCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of INCR command, expected INCR <key>\n";
        }
        std::string key = tokens[1];
        auto value = db->getRecord(key, client);
        if (value == nullptr)
        {
            return "ERR key does not exist\n";
        }
        if (value->getValue()->getType() != ValueType::STRING)
        {
            return "ERR value is not an integer\n";
        }
        try
        {
            int currentValue = std::stoi(std::any_cast<std::string>(value->getValue()->get()));
            currentValue++;
            auto newValue = ValueFactory::createValue(ValueType::STRING, std::to_string(currentValue));
            db->updateRecord(key, std::move(newValue), client);
        }
        catch (const std::invalid_argument &e)
        {
            return "ERR value is not an integer\n";
        }
        catch (const std::out_of_range &e)
        {
            return "ERR value is out of range\n";
        }
        catch (const std::exception &e)
        {
            return "ERR " + std::string(e.what()) + "\n";
        }

        return "OK\n";
    }
};

#endif // INCRCOMMAND_H
