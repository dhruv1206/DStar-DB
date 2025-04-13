#ifndef ILIST_VALUE_H
#define ILIST_VALUE_H

#include "IValue.h"
#include <vector>
#include <string>

class IListValue : public IValue
{
public:
    virtual ~IListValue() = default;
    // Push an element to the head of the list.
    virtual void lpush(const std::string &element) = 0;
    // Push an element to the tail of the list.
    virtual void rpush(const std::string &element) = 0;
    // Pop an element from the head of the list.
    virtual std::string lpop() = 0;
    // Pop an element from the tail of the list.
    virtual std::string rpop() = 0;
    // Get a range of elements.
    virtual std::vector<std::string> range(int start, int stop) const = 0;
    // Get the element at the specified index.
    virtual std::string get(const size_t index) const = 0;
    // Get the size of the list
    virtual std::size_t size() const = 0;
};

#endif // ILIST_VALUE_H
