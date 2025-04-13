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
#include "ConcreteCommands/StringCommands/INCRCommand.h"
#include "ConcreteCommands/StringCommands/INCRBYCommand.h"
#include "ConcreteCommands/StringCommands/DECRCommand.h"
#include "ConcreteCommands/StringCommands/DECRBYCommand.h"
#include "ConcreteCommands/KeyCommands/TYPECommand.h"
#include "ConcreteCommands/HashCommands/HGETALLCommand.h"
#include "ConcreteCommands/HashCommands/HGETCommand.h"
#include "ConcreteCommands/HashCommands/HMSETCommand.h"
#include "ConcreteCommands/HashCommands/HDELCommand.h"
#include "ConcreteCommands/HashCommands/HEXISTSCommand.h"
#include "ConcreteCommands/ListCommands/LPUSHCommand.h"
#include "ConcreteCommands/ListCommands/RPUSHCommand.h"
#include "ConcreteCommands/ListCommands/LRANGECommand.h"
#include "ConcreteCommands/ListCommands/LPOPCommand.h"
#include "ConcreteCommands/ListCommands/RPOPCommand.h"
#include "ConcreteCommands/ListCommands/LINDEXCommand.h"
#include "ConcreteCommands/ListCommands/LLENCommand.h"
#include "ConcreteCommands/SetCommands/SADDCommand.h"
#include "ConcreteCommands/SetCommands/SREMCommand.h"
#include "ConcreteCommands/SetCommands/SMEMBERSCommand.h"
#include "ConcreteCommands/SetCommands/SISMEMBERCommand.h"
#include "ConcreteCommands/SetCommands/SCARDCommand.h"
#include "ConcreteCommands/SortedSetCommands/ZADDCommand.h"
#include "ConcreteCommands/SortedSetCommands/ZREMCommand.h"
#include "ConcreteCommands/SortedSetCommands/ZCARDCommand.h"
#include "ConcreteCommands/SortedSetCommands/ZISMEMBERCommand.h"
#include "ConcreteCommands/SortedSetCommands/ZMEMBERSCommand.h"

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
        commands["INCR"] = std::make_unique<INCRCommand>();
        commands["INCRBY"] = std::make_unique<INCRBYCommand>();
        commands["DECR"] = std::make_unique<DECRCommand>();
        commands["DECRBY"] = std::make_unique<DECRBYCommand>();
        commands["TYPE"] = std::make_unique<TYPECommand>();
        commands["HGETALL"] = std::make_unique<HGETALLCommand>();
        commands["HGET"] = std::make_unique<HGETCommand>();
        commands["HMSET"] = std::make_unique<HMSETCommand>();
        commands["HDEL"] = std::make_unique<HDELCommand>();
        commands["HEXISTS"] = std::make_unique<HEXISTSCommand>();
        commands["LPUSH"] = std::make_unique<LPUSHCommand>();
        commands["RPUSH"] = std::make_unique<RPUSHCommand>();
        commands["LRANGE"] = std::make_unique<LRANGECommand>();
        commands["LPOP"] = std::make_unique<LPOPCommand>();
        commands["RPOP"] = std::make_unique<RPOPCommand>();
        commands["LLEN"] = std::make_unique<LLENCommand>();
        commands["LINDEX"] = std::make_unique<LINDEXCommand>();
        commands["SADD"] = std::make_unique<SADDCommand>();
        commands["SREM"] = std::make_unique<SREMCommand>();
        commands["SMEMBERS"] = std::make_unique<SMEMBERSCommand>();
        commands["SISMEMBER"] = std::make_unique<SISMEMBERCommand>();
        commands["SCARD"] = std::make_unique<SCARDCommand>();
        commands["ZADD"] = std::make_unique<ZADDCommand>();
        commands["ZREM"] = std::make_unique<ZREMCommand>();
        commands["ZCARD"] = std::make_unique<ZCARDCommand>();
        commands["ZISMEMBER"] = std::make_unique<ZISMEMBERCommand>();
        commands["ZMEMBERS"] = std::make_unique<ZMEMBERSCommand>();
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
