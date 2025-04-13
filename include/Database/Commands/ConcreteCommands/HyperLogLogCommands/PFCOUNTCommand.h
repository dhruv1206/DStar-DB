#ifndef PFCOUNT_COMMAND_H
#define PFCOUNT_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <vector>
#include "../../../ConcreteValues/HyperLogLogValue.h"

class PFCOUNTCommand : public ICommand
{
public:
    // PFCOUNT <key>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 2)
        {
            return "ERR wrong usage of PFCOUNT, expected PFCOUNT <key>\n";
        }
        std::string key = tokens[1];
        try
        {
            auto record = db->getRecord(key);
            if (!record->getValue() || record->getValue()->getType() != "hyperloglog")
            {
                return "ERR record is not a HyperLogLog\n";
            }
            auto hll = dynamic_cast<HyperLogLogValue *>(record->getValue());
            if (!hll)
                return "ERR internal type mismatch in PFCOUNT\n";
            return std::to_string(hll->estimate()) + "\n";
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // PFCOUNT_COMMAND_H
