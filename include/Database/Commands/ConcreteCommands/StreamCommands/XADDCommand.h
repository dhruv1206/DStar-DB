#ifndef XADD_COMMAND_H
#define XADD_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include "../../../IStreamValue.h"
#include <vector>
#include <memory>
#include "../../../ValueFactory.h"
#include "../../../ConcreteValues/StreamValue.h"

class XADDCommand : public ICommand
{
public:
    // Format: XADD <key> <field1> <value1> [<field2> <value2> ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 4 || tokens.size() % 2 != 0)
        {
            return "ERR wrong usage of XADD command, expected XADD <key> <field> <value> [<field> <value> ...]\n";
        }
        std::string key = tokens[1];
        std::unordered_map<std::string, std::string> fields;
        for (size_t i = 2; i < tokens.size(); i += 2)
        {
            fields[tokens[i]] = tokens[i + 1];
        }
        try
        {
            // Retrieve or create the stream.
            auto record = db->getRecord(key);
            auto streamVal = dynamic_cast<StreamValue *>(record->getValue());
            if (!streamVal)
            {
                return "ERR internal type mismatch in XADD\n";
            }
            auto messageId = streamVal->xadd(fields);
            return messageId.toString() + "\n";
        }
        catch (const std::exception &e)
        {
            std::unique_ptr<IValue> streamValue = ValueFactory::createValue(ValueType::STREAM, std::any());
            auto streamValuePtr = dynamic_cast<IStreamValue *>(streamValue.get());
            if (!streamValuePtr)
            {
                return "ERR failed to create stream value\n";
            }
            auto messageId = streamValuePtr->xadd(fields);
            db->insertRecord(key, std::move(streamValue));
            return messageId.toString() + "\n";
        }
    }
};

#endif // XADD_COMMAND_H
