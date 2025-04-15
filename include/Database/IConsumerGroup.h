#ifndef ICONSUMER_GROUP__H
#define ICONSUMER_GROUP__H

#include <string>
#include "ConcreteValues/Stream/StreamMessage.h"
#include "ConcreteValues/Stream/StreamConsumerGroup.h"

class IConsumerGroup
{
public:
    virtual ~IConsumerGroup() = default;

    // Create a new consumer group with the given name.
    virtual void createConsumerGroup(const std::string &groupName, const StreamId &startId = {0, 0}) = 0;

    // Get Consumer Group information.
    virtual ConsumerGroup *getConsumerGroup(const std::string &groupName) = 0;

    // Read messages for a consumer in the group.
    virtual std::vector<StreamMessage> xreadGroup(const std::string &groupName, const std::string &consumerName, const StreamId &lastId, int count) = 0;
};

#endif // ICONSUMER_GROUP__H
