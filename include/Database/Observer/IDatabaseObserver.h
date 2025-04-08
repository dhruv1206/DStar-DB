#ifndef IDATABASEOBSERVER_H
#define IDATABASEOBSERVER_H

#include <string>
#include <any>

class IDatabaseObserver
{
public:
    virtual ~IDatabaseObserver() = default;
    // Called when database is modified
    virtual void onDatabaseModified(const std::string &operation, const std::string &key) = 0;
    virtual void onDatabaseModified(const std::string &operation, const std::string &key, std::shared_ptr<IValue> value) = 0;
};

#endif // IDATABASEOBSERVER_H