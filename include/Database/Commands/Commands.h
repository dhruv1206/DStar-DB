#ifndef COMMANDS_H
#define COMMANDS_H

class Commands
{
public:
    // String Commands
    static constexpr const char *SET = "SET";
    static constexpr const char *GET = "GET";
    static constexpr const char *INCR = "INCR";
    static constexpr const char *INCRBY = "INCRBY";
    static constexpr const char *DECR = "DECR";
    static constexpr const char *DECRBY = "DECRBY";

    // Key Commands
    static constexpr const char *EXPIRE = "EXPIRE";
    static constexpr const char *EXISTS = "EXISTS";
    static constexpr const char *DEL = "DEL";
    static constexpr const char *PERSIST = "PERSIST";
    static constexpr const char *TYPE = "TYPE";

    // Hash Commands
    static constexpr const char *HGETALL = "HGETALL";
    static constexpr const char *HGET = "HGET";
    static constexpr const char *HMSET = "HMSET";
    static constexpr const char *HDEL = "HDEL";
    static constexpr const char *HEXISTS = "HEXISTS";

    // List Commands
    static constexpr const char *LPUSH = "LPUSH";
    static constexpr const char *RPUSH = "RPUSH";
    static constexpr const char *LRANGE = "LRANGE";
    static constexpr const char *LPOP = "LPOP";
    static constexpr const char *RPOP = "RPOP";
    static constexpr const char *LLEN = "LLEN";
    static constexpr const char *LINDEX = "LINDEX";

    // Set Commands
    static constexpr const char *SADD = "SADD";
    static constexpr const char *SREM = "SREM";
    static constexpr const char *SMEMBERS = "SMEMBERS";
    static constexpr const char *SISMEMBER = "SISMEMBER";
    static constexpr const char *SCARD = "SCARD";

    // Sorted Set Commands
    static constexpr const char *ZADD = "ZADD";
    static constexpr const char *ZREM = "ZREM";
    static constexpr const char *ZCARD = "ZCARD";
    static constexpr const char *ZISMEMBER = "ZISMEMBER";
    static constexpr const char *ZMEMBERS = "ZMEMBERS";

    // HyperLogLog Commands
    static constexpr const char *PFADD = "PFADD";
    static constexpr const char *PFCOUNT = "PFCOUNT";

    // Stream Commands
    static constexpr const char *XADD = "XADD";
    static constexpr const char *XREAD = "XREAD";
    static constexpr const char *XRANGE = "XRANGE";
    static constexpr const char *XGROUP = "XGROUP";
    static constexpr const char *XPENDING = "XPENDING";
    static constexpr const char *XACK = "XACK";
    static constexpr const char *XREADGROUP = "XREADGROUP";

    // Transaction Commands
    static constexpr const char *MULTI = "MULTI";
    static constexpr const char *EXEC = "EXEC";
    static constexpr const char *DISCARD = "DISCARD";
    static constexpr const char *WATCH = "WATCH";
    static constexpr const char *UNWATCH = "UNWATCH";

    // Misc
    static constexpr const char *PING = "PING";
    static constexpr const char *INVALID = "INVALID";
};

#endif // COMMANDS_H
