#ifndef XPENDING_COMMAND_H
#define XPENDING_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

class XPENDINGCommand : public ICommand
{
public:
    // Syntax: XPENDING <key> <group>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 3)
        {
            return "ERR wrong usage of XPENDING command, expected XPENDING <key> <group>\n";
        }
        std::string key = tokens[1];
        std::string groupName = tokens[2];
        try
        {
            auto record = db->getRecord(key);
            if (!record->getValue() || record->getValue()->getType() != "stream")
            {
                return "ERR key " + key + " is not a stream\n\r";
            }
            auto streamVal = dynamic_cast<IStreamValue *>(record->getValue());
            if (!streamVal)
            {
                return "ERR internal type mismatch in XPENDING\n\r";
            }
            ConsumerGroup *group = streamVal->getConsumerGroup(groupName);
            auto pending = group->getPending();
            std::ostringstream oss;
            oss << "Pending count: " << pending.size() << "\n\r";
            for (const auto &entry : pending)
            {
                oss << "ID: " << entry.messageId.toString() << ", Consumer: " << entry.consumer
                    << ", DeliveryCount: " << entry.deliveryCount << "\n\r";
            }
            return oss.str();
        }
        catch (const std::exception &ex)
        {
            return std::string("ERR ") + ex.what() + "\n\r";
        }
    }
};

#endif // XPENDING_COMMAND_H
