#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <sstream>
#include <vector>
#include <string>
#include "../Database/IDatabase.h"
#include "../Database/Commands/CommandRegistry.h"
#include "../ClientManager/Client.h"
#include "../Database/Commands/Commands.h"
#include <memory>

class CommandProcessor
{
public:
    CommandProcessor(IDatabase *database, CommandRegistry *registry)
        : db(database), commandRegistry(registry) {}

    void processCommand(const std::string &command, std::string &response, std::shared_ptr<Client> client)
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
            if (client->transactionContext->inTransaction &&
                cmdName != Commands::EXEC &&
                cmdName != Commands::DISCARD &&
                cmdName != Commands::MULTI &&
                cmdName != Commands::WATCH)
            {
                client->transactionContext->commands.push(std::bind(&ICommand::execute, cmd, tokens, command, db, client));
                response = "QUEUED\n";
            }
            else
            {
                response = cmd->execute(tokens, command, db, client);
            }
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
