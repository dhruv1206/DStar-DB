#ifndef XREADGROUP_COMMAND_H
#define XREADGROUP_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

class XREADGROUPCommand : public ICommand
{
public:
    // Syntax: XREADGROUP GROUP <group> <consumer> [COUNT count] STREAMS key [key ...] id [id ...]
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        if (tokens.size() < 6)
        {
            return "ERR wrong usage of XREADGROUP command, expected XREADGROUP GROUP <group> <consumer> [COUNT count] STREAMS key [key ...] id [id ...]\n";
        }
        if (tokens[1] != "GROUP")
        {
            return "ERR XREADGROUP must include GROUP\n";
        }
        std::string groupName = tokens[2];
        std::string consumerName = tokens[3];
        int count = -1;
        size_t index = 4;
        if (tokens[index] == "COUNT")
        {
            if (index + 1 >= tokens.size())
                return "ERR missing count value for COUNT\n";
            try
            {
                count = std::stoi(tokens[index + 1]);
            }
            catch (...)
            {
                return "ERR invalid count value\n";
            }
            index += 2;
        }
        if (tokens[index] != "STREAMS")
        {
            return "ERR expected STREAMS keyword in XREADGROUP command\n";
        }
        index++;
        int remaining = tokens.size() - index;
        if (remaining % 2 != 0 || remaining < 2)
        {
            return "ERR wrong number of arguments for STREAMS in XREADGROUP command\n";
        }
        int numStreams = remaining / 2;
        std::vector<std::string> keys;
        std::vector<std::string> ids;
        for (int i = 0; i < numStreams; i++)
        {
            keys.push_back(tokens[index + i]);
        }
        for (int i = 0; i < numStreams; i++)
        {
            ids.push_back(tokens[index + numStreams + i]);
        }
        std::ostringstream oss;
        for (int i = 0; i < numStreams; i++)
        {
            try
            {
                std::string key = keys[i];
                StreamId lastId = StreamId::parse(ids[i]);
                auto record = db->getRecord(key, client);
                if (!record->getValue() || record->getValue()->getType() != "stream")
                {
                    oss << "ERR key " << key << " is not a stream\n\r";
                    continue;
                }
                auto streamVal = dynamic_cast<IStreamValue *>(record->getValue());
                if (!streamVal)
                {
                    oss << "ERR internal type mismatch for stream " << key << "\n\r";
                    continue;
                }
                std::vector<StreamMessage> msgs = streamVal->xreadGroup(groupName, consumerName, lastId, (count == -1 ? 1000 : count));
                oss << "Stream: " << key << "\n\r";
                for (const auto &msg : msgs)
                {
                    oss << msg.id.toString() << " ";
                    for (const auto &field : msg.fields)
                    {
                        oss << field.first << " " << field.second << " ";
                    }
                    oss << "\n\r";
                }
            }
            catch (const std::exception &ex)
            {
                oss << "ERR " << ex.what() << "\n\r";
            }
        }
        return oss.str();
    }
};

#endif // XREADGROUP_COMMAND_H
