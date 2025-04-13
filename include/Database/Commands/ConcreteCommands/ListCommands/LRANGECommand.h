#ifndef LRANGE_COMMAND_H
#define LRANGE_COMMAND_H

#include "../../../IDatabase.h"
#include "../../../IListValue.h"
#include "../../ICommand.h"
#include <string>
#include <sstream>
#include <vector>

class LRANGECommand : public ICommand
{
public:
    // Expects: LRANGE <key> <start> <stop>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 4)
        {
            return "ERR wrong usage of LRANGE command. Expected LRANGE <key> <start> <stop>\n";
        }
        std::string key = tokens[1];
        int start = std::stoi(tokens[2]);
        int stop = std::stoi(tokens[3]);
        try
        {
            auto record = db->getRecord(key);
            if (!record->getValue() || record->getValue()->getType() != "list")
            {
                return "ERR record is not a list\n";
            }
            auto listValue = dynamic_cast<IListValue *>(record->getValue());
            if (!listValue)
            {
                return "ERR internal type conversion error in LRANGE\n";
            }
            std::vector<std::string> range = listValue->range(start, stop);
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < range.size(); ++i)
            {
                oss << range[i];
                if (i < range.size() - 1)
                {
                    oss << ", ";
                }
            }
            oss << "]\n";
            return oss.str();
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // LRANGE_COMMAND_H
