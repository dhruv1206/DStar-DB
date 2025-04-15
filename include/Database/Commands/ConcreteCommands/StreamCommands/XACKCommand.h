#ifndef XACK_COMMAND_H
#define XACK_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

class XACKCommand : public ICommand
{
public:
    // Syntax: XACK <key> <group> <id> [id ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() < 4)
        {
            return "ERR wrong usage of XACK command, expected XACK <key> <group> <id> [id ...]\n";
        }
        std::string key = tokens[1];
        std::string groupName = tokens[2];
        std::vector<std::string> ids;
        for (size_t i = 3; i < tokens.size(); i++)
        {
            ids.push_back(tokens[i]);
        }
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
                return "ERR internal type mismatch in XACK\n\r";
            }
            ConsumerGroup *group = streamVal->getConsumerGroup(groupName);
            if (!group)
                return "ERR consumer group not found\n\r";
            int ackCount = 0;
            for (const auto &id : ids)
            {
                if (group->ack(StreamId::parse(id)))
                    ackCount++;
            }
            return std::to_string(ackCount) + "\n\r";
        }
        catch (const std::exception &ex)
        {
            return std::string("ERR ") + ex.what() + "\n\r";
        }
    }
};

#endif // XACK_COMMAND_H
