#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "../Database/IDatabase.h"
#include "../Database/Commands/CommandRegistry.h"

class CommandProcessor
{
public:
    CommandProcessor(IDatabase *database, CommandRegistry *registry)
        : db(database), commandRegistry(registry) {}

    void processCommand(const std::string &command, std::string &response)
    {
        std::istringstream iss(command);
        std::vector<std::string> tokens;
        std::string word;
        while (iss >> word)
        {
            tokens.push_back(word);
        }
        if (tokens.empty())
        {
            return;
        }
        std::string cmdName = tokens[0];
        ICommand *cmd = commandRegistry->getCommand(cmdName);
        if (cmd != nullptr)
        {
            response = cmd->execute(tokens, command, db);
        }
        else
        {
            response = "ERR unknown command\n";
        }
    }

private:
    IDatabase *db;
    CommandRegistry *commandRegistry;
};

#endif // COMMAND_PROCESSOR_H
