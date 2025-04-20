#ifndef PFADD_COMMAND_H
#define PFADD_COMMAND_H

#include "../../../IDatabase.h"
#include "../../../ValueFactory.h"
#include "../../ICommand.h"
#include <vector>
#include <memory>
#include "../../../ConcreteValues/HyperLogLogValue.h"

class PFADDCommand : public ICommand
{
public:
    // PFADD <key> <element> [element ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 3)
        {
            return "ERR wrong usage of PFADD, expected PFADD <key> <element> [element ...]\n";
        }
        std::string key = tokens[1];
        // Retrieve or create HyperLogLog.
        try
        {
            auto record = db->getRecord(key, client);
            auto hll = dynamic_cast<HyperLogLogValue *>(record->getValue());
            if (!hll)
                return "ERR internal type mismatch in PFADD\n";
            for (size_t i = 2; i < tokens.size(); i++)
            {
                hll->add(tokens[i]);
            }
        }
        catch (const std::exception &e)
        {
            std::unique_ptr<IValue> hllValue = ValueFactory::createValue(ValueType::HYPER_LOG_LOG, std::any());
            auto hll = dynamic_cast<HyperLogLogValue *>(hllValue.get());
            if (!hll)
                return "ERR internal type mismatch in PFADD\n";
            for (size_t i = 2; i < tokens.size(); i++)
            {
                hll->add(tokens[i]);
            }
            db->insertRecord(key, std::move(hllValue), client);
        }
        return "OK\n";
    }
};

#endif // PFADD_COMMAND_H
