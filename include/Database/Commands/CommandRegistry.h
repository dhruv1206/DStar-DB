#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include "unordered_map"
#include "string"
#include "ICommand.h"
#include "ConcreteCommands/StringCommands/SETCommand.h"
#include "ConcreteCommands/StringCommands/GETCommand.h"
#include "ConcreteCommands/PINGCommand.h"
#include "ConcreteCommands/InvalidCommand.h"
#include "ConcreteCommands/KeyCommands/EXPIRECommand.h"
#include "ConcreteCommands/KeyCommands/EXISTSCommand.h"
#include "ConcreteCommands/KeyCommands/DELCommand.h"
#include "ConcreteCommands/KeyCommands/PERSISTCommand.h"

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
        commands["EXPIRE"] = std::make_unique<EXPIRECommand>();
        commands["EXISTS"] = std::make_unique<EXISTSCommand>();
        commands["DEL"] = std::make_unique<DELCommand>();
        commands["PERSIST"] = std::make_unique<PERSISTCommand>();
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
