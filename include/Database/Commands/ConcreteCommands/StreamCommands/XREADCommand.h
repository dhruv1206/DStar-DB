#ifndef XREAD_COMMAND_H
#define XREAD_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

// XREADCommand implements the XREAD command.
// Syntax: XREAD [COUNT count] STREAMS key [key ...] id [id ...]
class XREADCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db, std::shared_ptr<Client> client) override
    {
        // Minimum tokens: XREAD STREAMS key id  -> 4 tokens.
        if (tokens.size() < 4)
        {
            return "ERR wrong usage of XREAD command, expected XREAD [COUNT count] STREAMS key [key ...] id [id ...]\n";
        }

        int count = -1;   // -1 means no COUNT specified.
        size_t index = 1; // tokens[0] is "XREAD"

        // Parse optional COUNT clause.
        if (tokens[index] == "COUNT")
        {
            if (index + 1 >= tokens.size())
            {
                return "ERR missing count after COUNT in XREAD command\n";
            }
            try
            {
                count = std::stoi(tokens[index + 1]);
            }
            catch (const std::exception &)
            {
                return "ERR invalid count value in XREAD command\n";
            }
            index += 2;
        }

        // Next token must be "STREAMS"
        if (tokens[index] != "STREAMS")
        {
            return "ERR expected STREAMS keyword in XREAD command\n";
        }
        index++;

        // Now, the remaining tokens are split into two parts:
        // first, a list of keys; then, a list of ids.
        // The two lists must be of the same length.
        int remaining = tokens.size() - index;
        if (remaining % 2 != 0 || remaining < 2)
        {
            return "ERR wrong number of arguments for STREAMS in XREAD command\n";
        }
        int numStreams = remaining / 2;

        // Extract keys and ids.
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

        // Build response string.
        // Format similar to Redis: For each stream, return stream key and array of messages.
        std::ostringstream oss;
        for (int i = 0; i < numStreams; i++)
        {
            try
            {
                std::string key = keys[i];
                StreamId lastId = StreamId::parse(ids[i]);
                // Attempt to get the record for the stream.
                auto record = db->getRecord(key, client);
                if (!record->getValue() || record->getValue()->getType() != "stream")
                {
                    oss << "ERR key " << key << " is not a stream\n\r";
                    continue;
                }
                // Cast to StreamValue.
                auto streamVal = dynamic_cast<IStreamValue *>(record->getValue());
                if (!streamVal)
                {
                    oss << "ERR internal type mismatch for stream " << key << "\n\r";
                    continue;
                }
                // Use xread to get at most 'count' messages with IDs greater than lastId.
                // If count is -1, we assume no limit.
                int fetchCount = (count == -1) ? 1000 : count; // You can set a default max if count not specified.
                std::vector<StreamMessage> messages = streamVal->xread(lastId, fetchCount);

                // Format the stream's response.
                oss << "Stream: " << key << "\n\r";
                for (const auto &msg : messages)
                {
                    oss << msg.id.toString() << " ";
                    // Each message's fields.
                    for (const auto &field : msg.fields)
                    {
                        oss << field.first << " " << field.second << " ";
                    }
                    oss << "\n\r";
                }
                oss << "\n\r"; // Separate streams with a newline.
            }
            catch (const std::invalid_argument &ex)
            {
                oss << "ERR " << ex.what() << "\n\r";
            }
            catch (const std::exception &ex)
            {
                oss << "ERR " << ex.what() << "\n\r";
            }
        }
        return oss.str();
    }
};

#endif // XREAD_COMMAND_H
