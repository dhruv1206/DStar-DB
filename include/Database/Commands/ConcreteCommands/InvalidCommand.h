#ifndef INVALID_H
#define INVALID_H

#include "../../IDatabase.h"
#include <string>
#include "../ICommand.h"

class InvalidCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &, const std::string &command, IDatabase *, std::shared_ptr<Client>) override
    {
        return "ERR unknown command\n";
    }
};

#endif // INVALID_H