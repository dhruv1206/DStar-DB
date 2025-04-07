#ifndef ICOMMAND_H
#define ICOMMAND_H

#include "../IDatabase.h"
#include <string>
#include <vector>
#include <sstream>

class ICommand
{
public:
    virtual ~ICommand() = default;
    virtual std::string execute(std::vector<std::string> &tokens, const std::string &fullCommand, IDatabase *db) = 0;
    virtual void parseCommand(const std::string &command, std::vector<std::string> &tokens) const
    {
        std::istringstream iss(command);
        std::string word;
        while (iss >> word)
        {
            tokens.push_back(word);
        }
    }
};

#endif // ICOMMAND_H