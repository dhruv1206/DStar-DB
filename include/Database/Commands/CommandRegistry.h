#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include "unordered_map"
#include "string"
#include "ICommand.h"
#include "ConcreteCommands/SETCommand.h"
#include "ConcreteCommands/GETCommand.h"
#include "ConcreteCommands/PINGCommand.h"
#include "ConcreteCommands/InvalidCommand.h"

class CommandRegistry
{
public:
    CommandRegistry()
    {
        // Register commands
        commands["SET"] = std::make_unique<SETCommand>();
        commands["GET"] = std::make_unique<GETCommand>();
        commands["PING"] = std::make_unique<PINGCommand>();
        commands["INVALID"] = std::make_unique<InvalidCommand>();
    }

    ICommand *getCommand(const std::string &commandName)
    {
        auto it = commands.find(commandName);
        if (it != commands.end())
        {
            return it->second.get();
        }
        return commands["INVALID"].get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commands;
};

#endif // COMMANDREGISTRY_H
