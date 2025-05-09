#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#endif

#include <iostream>
#include <cstring>
#include <thread>
#include <atomic>
#include <memory>
#include "../include/Database/Database.h"
#include "../include/Database/IDatabase.h"
#include "../include/Database/Commands/CommandRegistry.h"
#include "../headers/cxxopts-master/include/cxxopts.hpp"
#include "../include/Constants.h"
#include "../include/Database/Persistence/IPersistenceManager.h"
#include "../include/Database/Persistence/PersistenceFactory.h"
#include "../include/Database/Decorators/TTLDatabaseDecorator.h"
#include "../include/ClientManager/ClientManager.h"
#include "../include/CommandProcessor/CommandProcessor.h"
#include "../include/EventLoop.h"

std::atomic<bool> running(true);

void *operator new(size_t bytes)
{
    MemoryManager::currentUsageBytes += bytes;
    return malloc(bytes);
}

void operator delete(void *memory, size_t bytes) noexcept
{
    MemoryManager::currentUsageBytes -= bytes;
    free(memory);
}

#ifdef _WIN32
BOOL WINAPI handleSignal(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        running = false;
        return TRUE;
    }
    return FALSE;
}
#else
void handleSignal(int signal)
{
    running = false;
}
#endif

void printServerStartedBanner()
{
    std::cout << R"(

  ____    ____     _____      _       ____     ____    ____
 |  _"\  / __"| u |_ " _| U  /"\  uU |  _"\ u |  _"\U | __")u
/| | | |<\___ \/    | |    \/ _ \/  \| |_) |//| | | |\|  _ \/
U| |_| |\u___) |   /| |\   / ___ \   |  _ <  U| |_| |\| |_) |
 |____/ u|____/>> u |_|U  /_/   \_\  |_| \_\  |____/ u|____/
  |||_    )(  (__)_// \\_  \\    >>  //   \\_  |||_  _|| \\_
 (__)_)  (__)    (__) (__)(__)  (__)(__)  (__)(__)_)(__) (__)

                DStarDB Server Running @ 1206
    )" << std::endl;
    std::cout << R"(                                          By Dhruv Agrawal)" << std::endl;
}

std::unique_ptr<IDatabase> db;
std::unique_ptr<CommandRegistry> commandRegistry = std::make_unique<CommandRegistry>();

int main(int argc, char *argv[])
{
    try
    {
        printServerStartedBanner();
        size_t cacheSizeMB = Constants::DEFAULT_CACHE_SIZE_MB;
        bool enablePersistence = false;
        std::string persistMode = "snapshot";

        cxxopts::Options options("DStarDB", "DStarDB server command line options");
        options.add_options()("p,persist", "Enable persistence", cxxopts::value<bool>()->default_value("false"))("m,mode", "Persistence mode (snapshot / aof)", cxxopts::value<std::string>()->default_value("snapshot"))("h,help", "Print help")("c,cache", "Cache size in MB", cxxopts::value<int>()->default_value(std::to_string(Constants::DEFAULT_CACHE_SIZE_MB)));

        auto result = options.parse(argc, argv);
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }
        enablePersistence = result["persist"].as<bool>();
        persistMode = result["mode"].as<std::string>();
        cacheSizeMB = result["cache"].as<int>();

        db = std::make_unique<Database>(cacheSizeMB);
        db = std::make_unique<TTLDatabaseDecorator>(std::move(db));

        std::cout << "Persistence enabled: " << (enablePersistence ? "true" : "false") << std::endl;
        std::cout << "Persistence mode: " << persistMode << std::endl;
        std::cout << "Cache size: " << cacheSizeMB << " MB" << std::endl;

        std::unique_ptr<IPersistenceManager> persistenceManager;
        if (enablePersistence)
        {
            persistenceManager = PersistenceFactory::getPersistenceManager(persistMode);
            persistenceManager->initialize(db.get()); // Initialize the persistence manager with the database
            // Load persisted state.
            persistenceManager->load(db.get());
        }

#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed\n";
            return 1;
        }

        SetConsoleCtrlHandler(handleSignal, TRUE);
#else
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
#endif

        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0)
        {
            std::cerr << "Socket creation failed\n";
            return 1;
        }

        sockaddr_in address;
        std::memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(1206); // DStarDB port

        int opt = 1;
#ifdef _WIN32
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        if (bind(server_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        {
            std::cerr << "Bind failed\n";
#ifdef _WIN32
            closesocket(server_fd);
            WSACleanup();
#else
            close(server_fd);
#endif
            return 1;
        }

        if (listen(server_fd, 10) < 0)
        {
            std::cerr << "Listen failed\n";
#ifdef _WIN32
            closesocket(server_fd);
            WSACleanup();
#else
            close(server_fd);
#endif
            return 1;
        }

        ClientManager clientManager;
        std::shared_ptr<CommandProcessor> commandProcessor = std::make_shared<CommandProcessor>(db.get(), commandRegistry.get());
        // Create and start the event loop (reactor).
        auto eventLoop = std::make_shared<EventLoop>(server_fd, clientManager, commandProcessor);
        std::thread eventLoopThread([eventLoop]()
                                    { eventLoop->run(); });

        // Main thread waits until shutdown is signaled.
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        eventLoop->stop();
        if (eventLoopThread.joinable())
            eventLoopThread.join();

        std::cout << "\nShutting down DStarDB->..\n";
#ifdef _WIN32
        closesocket(server_fd);
        WSACleanup();
#else
        close(server_fd);
#endif
        delete db.get();
        std::cout << "DStarDB server shut down successfully.\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred\n";
        return 1;
    }
}
