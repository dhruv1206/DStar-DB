#ifndef PERSISTENCEFACTORY_H
#define PERSISTENCEFACTORY_H

#include <memory>
#include "IPersistenceManager.h"
#include "Implementations/AOFPersistence.h"
#include "Implementations/SnapshotPersistence.h"
#include "../Serializer/IDBSerializer.h"
#include "../Serializer/Strategies/BinarySerializer.h"

class PersistenceFactory
{
public:
    static std::unique_ptr<IPersistenceManager> getPersistenceManager(const std::string &mode)
    {
        if (mode == "aof")
        {
            return std::make_unique<AOFPersistence>();
        }
        else if (mode == "snapshot")
        {
            std::unique_ptr<IDBSerializer> serializer = std::make_unique<BinarySerializer>();
            return std::make_unique<SnapshotPersistence>(std::move(serializer));
        }
        else
        {
            throw std::invalid_argument("Invalid persistence mode. Use 'snapshot' or 'aof'.");
        }
    }
};

#endif // PERSISTENCEFACTORY_H