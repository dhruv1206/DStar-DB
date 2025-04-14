#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include "unordered_map"
#include "string"
#include "ICommand.h"
#include "../Commands/ConcreteCommands/StringCommands/SETCommand.h"
#include "../Commands/ConcreteCommands/StringCommands/GETCommand.h"
#include "../Commands/ConcreteCommands/PINGCommand.h"
#include "../Commands/ConcreteCommands/InvalidCommand.h"
#include "../Commands/ConcreteCommands/KeyCommands/EXPIRECommand.h"
#include "../Commands/ConcreteCommands/KeyCommands/EXISTSCommand.h"
#include "../Commands/ConcreteCommands/KeyCommands/DELCommand.h"
#include "../Commands/ConcreteCommands/KeyCommands/PERSISTCommand.h"
#include "../Commands/ConcreteCommands/StringCommands/INCRCommand.h"
#include "../Commands/ConcreteCommands/StringCommands/INCRBYCommand.h"
#include "../Commands/ConcreteCommands/StringCommands/DECRCommand.h"
#include "../Commands/ConcreteCommands/StringCommands/DECRBYCommand.h"
#include "../Commands/ConcreteCommands/KeyCommands/TYPECommand.h"
#include "../Commands/ConcreteCommands/HashCommands/HGETALLCommand.h"
#include "../Commands/ConcreteCommands/HashCommands/HGETCommand.h"
#include "../Commands/ConcreteCommands/HashCommands/HMSETCommand.h"
#include "../Commands/ConcreteCommands/HashCommands/HDELCommand.h"
#include "../Commands/ConcreteCommands/HashCommands/HEXISTSCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/LPUSHCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/RPUSHCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/LRANGECommand.h"
#include "../Commands/ConcreteCommands/ListCommands/LPOPCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/RPOPCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/LINDEXCommand.h"
#include "../Commands/ConcreteCommands/ListCommands/LLENCommand.h"
#include "../Commands/ConcreteCommands/SetCommands/SADDCommand.h"
#include "../Commands/ConcreteCommands/SetCommands/SREMCommand.h"
#include "../Commands/ConcreteCommands/SetCommands/SMEMBERSCommand.h"
#include "../Commands/ConcreteCommands/SetCommands/SISMEMBERCommand.h"
#include "../Commands/ConcreteCommands/SetCommands/SCARDCommand.h"
#include "../Commands/ConcreteCommands/SortedSetCommands/ZADDCommand.h"
#include "../Commands/ConcreteCommands/SortedSetCommands/ZREMCommand.h"
#include "../Commands/ConcreteCommands/SortedSetCommands/ZCARDCommand.h"
#include "../Commands/ConcreteCommands/SortedSetCommands/ZISMEMBERCommand.h"
#include "../Commands/ConcreteCommands/SortedSetCommands/ZMEMBERSCommand.h"
#include "../Commands/ConcreteCommands/HyperLogLogCommands/PFADDCommand.h"
#include "../Commands/ConcreteCommands/HyperLogLogCommands/PFCOUNTCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XADDCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XREADCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XRANGECommand.h"

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
        commands["PFADD"] = std::make_unique<PFADDCommand>();
        commands["PFCOUNT"] = std::make_unique<PFCOUNTCommand>();
        commands["XADD"] = std::make_unique<XADDCommand>();
        commands["XREAD"] = std::make_unique<XREADCommand>();
        commands["XRANGE"] = std::make_unique<XRANGECommand>();
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
