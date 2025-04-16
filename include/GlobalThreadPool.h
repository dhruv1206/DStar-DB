#ifndef GLOBAL_THREAD_POOL_H
#define GLOBAL_THREAD_POOL_H

#include "ThreadPool.h"
#include <thread>

// GlobalThreadPool is a singleton wrapper around ThreadPool.
class GlobalThreadPool
{
public:
    // Returns a reference to the global ThreadPool instance.
    static ThreadPool &getInstance()
    {
        // Meyers' Singleton: the instance is created on first call in a thread-safe way.
        static ThreadPool instance{std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4};
        return instance;
    }

    // Delete copy constructor and assignment operator.
    GlobalThreadPool(const GlobalThreadPool &) = delete;
    GlobalThreadPool &operator=(const GlobalThreadPool &) = delete;

private:
    // Private constructor to prevent instantiation.
    GlobalThreadPool() = default;
};

#endif // GLOBAL_THREAD_POOL_H
