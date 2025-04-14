#ifndef XRANGE_COMMAND_H
#define XRANGE_COMMAND_H

#include "../../../IDatabase.h"
#include "../../ICommand.h"
#include <sstream>
#include <vector>
#include "../../../IStreamValue.h"
#include "../../../ConcreteValues/Stream/StreamMessage.h"

class XRANGECommand : public ICommand
{
public:
    // Format: XRANGE <key> <start> <end>
    std::string execute(std::vector<std::string> &tokens, const std::string &command, IDatabase *db) override
    {
        if (tokens.size() != 4)
        {
            return "ERR wrong usage of XRANGE command, expected XRANGE <key> <start> <end>\n";
        }
        std::string key = tokens[1];
        std::string start = tokens[2];
        std::string end = tokens[3];
        try
        {
            auto record = db->getRecord(key);
            if (!record->getValue() || record->getValue()->getType() != "stream")
            {
                return "ERR record is not a stream\n";
            }
            auto streamVal = dynamic_cast<IStreamValue *>(record->getValue());
            if (!streamVal)
            {
                return "ERR internal type mismatch in XRANGE\n";
            }
            StreamId startId = StreamId::parse(start);
            StreamId endId = StreamId::parse(end);

            std::vector<StreamMessage> msgs = streamVal->xrange(startId, endId);
            std::ostringstream oss;
            for (const auto &msg : msgs)
            {
                oss << msg.id.toString() << " ";
                for (const auto &p : msg.fields)
                {
                    oss << p.first << " " << p.second << " ";
                }
                oss << "\n\r";
            }
            return oss.str();
        }
        catch (const std::exception &e)
        {
            return std::string("ERR ") + e.what() + "\n";
        }
    }
};

#endif // XRANGE_COMMAND_H
