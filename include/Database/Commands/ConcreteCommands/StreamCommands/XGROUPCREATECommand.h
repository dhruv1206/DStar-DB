#ifndef XGROUPCREATE_COMMAND_H
#define XGROUPCREATE_COMMAND_H

#include "../../../IDatabase.h"
#include "../../../ValueFactory.h"
#include "../../ICommand.h"
#include <vector>
#include <memory>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

class XGROUPCREATECommand : public ICommand
{
public:
    // Syntax: XGROUP CREATE <key> <groupname> <id|$> [MKSTREAM]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 5)
        {
            return "ERR wrong usage of XGROUP CREATE command, expected XGROUP CREATE <key> <groupname> <id|$> [MKSTREAM]\n";
        }
        std::string key = tokens[2];
        std::string groupName = tokens[3];
        std::string id = tokens[4];
        bool mkstream = false;
        if (tokens.size() > 5 && tokens[5] == "MKSTREAM")
        {
            mkstream = true;
        }

        try
        {
            std::shared_ptr<Record> record;
            try
            {
                record = db->getRecord(key, client);
            }
            catch (const std::exception &e)
            {
                record = nullptr;
            }
            if (!record || !record->getValue() || record->getValue()->getType() != "stream")
            {
                if (mkstream)
                {
                    std::unique_ptr<IValue> streamValue = ValueFactory::createValue(ValueType::STREAM, std::any());
                    db->insertRecord(key, std::move(streamValue), client);
                    record = db->getRecord(key, client);
                }
                else
                {
                    return "ERR key " + key + " does not exist\n\r";
                }
            }
            auto streamVal = dynamic_cast<IStreamValue *>(record->getValue());
            if (!streamVal)
            {
                return "ERR internal type mismatch in XGROUP CREATE\n\r";
            }
            streamVal->createConsumerGroup(groupName, StreamId::parse(id));
            return "OK\n";
        }
        catch (const std::exception &ex)
        {
            return std::string("ERR ") + ex.what() + "\n\r";
        }
    }
};

#endif // XGROUPCREATE_COMMAND_H
