#ifndef IDATABASEOBSERVER_H
#define IDATABASEOBSERVER_H

#include <string>
#include "../IValue.h"

class IDatabaseObserver
{
public:
    virtual ~IDatabaseObserver() = default;
    // Called when database is modified
    virtual void onDatabaseModified(const std::string &operation, const std::string &key) {}
    virtual void onDatabaseModified(const std::string &operation, const std::string &key, IValue* value) {}
    // Called when a specific record is modified
    virtual void onRecordModified(const std::string &operation, const std::string &key) {}
    virtual void onRecordModified(const std::string &operation, const std::string &key, IValue* value) {}
};

#endif // IDATABASEOBSERVER_H