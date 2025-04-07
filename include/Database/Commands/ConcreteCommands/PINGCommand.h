#ifndef PINGCOMMAND_H
#define PINGCOMMAND_H

#include "../../IDatabase.h"
#include <string>
#include "../ICommand.h"

class PINGCommand : public ICommand
{
public:
    std::string execute(std::vector<std::string> &, const std::string &command, IDatabase *) override
    {
        return "PONG\n";
    }
};

#endif // PINGCOMMAND_H