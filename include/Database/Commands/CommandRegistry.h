#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include "unordered_map"
#include "string"
#include "ICommand.h"
#include "Commands.h"
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
#include "../Commands/ConcreteCommands/SETCommands/SADDCommand.h"
#include "../Commands/ConcreteCommands/SETCommands/SREMCommand.h"
#include "../Commands/ConcreteCommands/SETCommands/SMEMBERSCommand.h"
#include "../Commands/ConcreteCommands/SETCommands/SISMEMBERCommand.h"
#include "../Commands/ConcreteCommands/SETCommands/SCARDCommand.h"
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
#include "../Commands/ConcreteCommands/StreamCommands/XGROUPCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XPENDINGCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XACKCommand.h"
#include "../Commands/ConcreteCommands/StreamCommands/XREADGROUPCommand.h"
#include "../Commands/ConcreteCommands/TransactionCommands/WATCHCommand.h"
#include "../Commands/ConcreteCommands/TransactionCommands/MULTICommand.h"
#include "../Commands/ConcreteCommands/TransactionCommands/EXECCommand.h"
#include "../Commands/ConcreteCommands/TransactionCommands/DISCARDCommand.h"

class CommandRegistry
{
public:
    CommandRegistry()
    {
        // Register commands
        commands[Commands::SET] = std::make_unique<SETCommand>();
        commands[Commands::GET] = std::make_unique<GETCommand>();
        commands[Commands::PING] = std::make_unique<PINGCommand>();
        commands[Commands::INVALID] = std::make_unique<InvalidCommand>();
        commands[Commands::EXPIRE] = std::make_unique<EXPIRECommand>();
        commands[Commands::EXISTS] = std::make_unique<EXISTSCommand>();
        commands[Commands::DEL] = std::make_unique<DELCommand>();
        commands[Commands::PERSIST] = std::make_unique<PERSISTCommand>();
        commands[Commands::INCR] = std::make_unique<INCRCommand>();
        commands[Commands::INCRBY] = std::make_unique<INCRBYCommand>();
        commands[Commands::DECR] = std::make_unique<DECRCommand>();
        commands[Commands::DECRBY] = std::make_unique<DECRBYCommand>();
        commands[Commands::TYPE] = std::make_unique<TYPECommand>();
        commands[Commands::HGETALL] = std::make_unique<HGETALLCommand>();
        commands[Commands::HGET] = std::make_unique<HGETCommand>();
        commands[Commands::HMSET] = std::make_unique<HMSETCommand>();
        commands[Commands::HDEL] = std::make_unique<HDELCommand>();
        commands[Commands::HEXISTS] = std::make_unique<HEXISTSCommand>();
        commands[Commands::LPUSH] = std::make_unique<LPUSHCommand>();
        commands[Commands::RPUSH] = std::make_unique<RPUSHCommand>();
        commands[Commands::LRANGE] = std::make_unique<LRANGECommand>();
        commands[Commands::LPOP] = std::make_unique<LPOPCommand>();
        commands[Commands::RPOP] = std::make_unique<RPOPCommand>();
        commands[Commands::LLEN] = std::make_unique<LLENCommand>();
        commands[Commands::LINDEX] = std::make_unique<LINDEXCommand>();
        commands[Commands::SADD] = std::make_unique<SADDCommand>();
        commands[Commands::SREM] = std::make_unique<SREMCommand>();
        commands[Commands::SMEMBERS] = std::make_unique<SMEMBERSCommand>();
        commands[Commands::SISMEMBER] = std::make_unique<SISMEMBERCommand>();
        commands[Commands::SCARD] = std::make_unique<SCARDCommand>();
        commands[Commands::ZADD] = std::make_unique<ZADDCommand>();
        commands[Commands::ZREM] = std::make_unique<ZREMCommand>();
        commands[Commands::ZCARD] = std::make_unique<ZCARDCommand>();
        commands[Commands::ZISMEMBER] = std::make_unique<ZISMEMBERCommand>();
        commands[Commands::ZMEMBERS] = std::make_unique<ZMEMBERSCommand>();
        commands[Commands::PFADD] = std::make_unique<PFADDCommand>();
        commands[Commands::PFCOUNT] = std::make_unique<PFCOUNTCommand>();
        commands[Commands::XADD] = std::make_unique<XADDCommand>();
        commands[Commands::XREAD] = std::make_unique<XREADCommand>();
        commands[Commands::XRANGE] = std::make_unique<XRANGECommand>();
        commands[Commands::XGROUP] = std::make_unique<XGROUPCommand>();
        commands[Commands::XPENDING] = std::make_unique<XPENDINGCommand>();
        commands[Commands::XACK] = std::make_unique<XACKCommand>();
        commands[Commands::XREADGROUP] = std::make_unique<XREADGROUPCommand>();
        commands[Commands::WATCH] = std::make_unique<WATCHCommand>();
        commands[Commands::MULTI] = std::make_unique<MULTICommand>();
        commands[Commands::EXEC] = std::make_unique<EXECCommand>();
        commands[Commands::DISCARD] = std::make_unique<DISCARDCommand>();
    }

    ICommand *getCommand(const std::string &commandName)
    {
        auto it = commands.find(commandName);
        if (it != commands.end())
        {
            return it->second.get();
        }
        return commands[Commands::INVALID].get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commands;
};

#endif // COMMANDREGISTRY_H
