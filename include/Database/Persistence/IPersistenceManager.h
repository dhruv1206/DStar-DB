#ifndef IPERSISTENCEMANAGER_H
#define IPERSISTENCEMANAGER_H

#include "../IDatabase.h"

class IPersistenceManager
{
public:
    virtual ~IPersistenceManager() = default;
    // Load database state from persistent storage
    virtual void load(IDatabase *db) = 0;
    // Save database state to persistent storage
    virtual void save(const IDatabase *db) = 0;
    virtual void initialize(IDatabase *db) = 0;

protected:
    std::string filename = "dstar_persisted_storage.db"; // Default snapshot filename
};

#endif // IPERSISTENCEMANAGER_H