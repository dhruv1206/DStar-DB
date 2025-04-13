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

#include <sstream>
#include <iostream>
#include <cstring>
#include <thread>
#include <atomic>
#include <vector>
#include "../include/Database/Database.h"
#include "../include/Database/IDatabase.h"
#include "../include/Database/Commands/CommandRegistry.h"
#include "../include/Database/Commands/ICommand.h"
#include "../include/ThreadPool.h"
#include "../headers/cxxopts-master/include/cxxopts.hpp"
#include "../include/Constants.h"
#include "../include/Database/Persistence/IPersistenceManager.h"
#include "../include/Database/Persistence/PersistenceFactory.h"
#include "../include/Database/Decorators/TTLDatabaseDecorator.h"

std::atomic<bool> running(true);

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
    std::cout << R"(                                          By Dhruv Agrawal)" << std::endl;;
}

std::unique_ptr<IDatabase> db;
CommandRegistry *commandRegistry = new CommandRegistry();

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
        response = cmd->execute(tokens, command, db.get());
    }
    else
    {
        response = "ERR unknown command\n";
    }
}

void handleClient(int client_fd)
{
    std::cout << "New client connected: " << client_fd << "\n";
    const std::string prompt = "\033[32mDStarDB> \033[0m";
    send(client_fd, prompt.c_str(), prompt.size(), 0);

    std::string buffer = "";
    char ch;
    int recvResult;

    std::vector<std::string> commandHistory;
    int commandHistoryIndex = -1;

    while (true)
    {
        recvResult = recv(client_fd, &ch, 1, 0);
        if (recvResult <= 0)
        {
            std::cerr << "Client disconnected or error occurred\n";
            break; // Client disconnected or error occurred
        }
        if (ch == 3)
        { // Ctrl+C (ETX)
            // send a goodbye message before closing.const
            std::string goodbye = "Goodbye!\r\n";
            send(client_fd, goodbye.c_str(), goodbye.size(), 0);
            break; // Exit the loop, closing the connection.
        }
        // Check for backspace (ASCII 8 or 127)
        if (ch == 8 || ch == 127)
        {
            if (!buffer.empty())
            {
                buffer.pop_back();
                // Send backspace, space, then backspace again to erase on client's terminal
                const char *bs = " \b";
                send(client_fd, bs, 2, 0);
            }
            else
            {
                send(client_fd, " ", 1, 0);
            }
            // If buffer empty, ignore further backspaces.
            continue;
        }

        // Check for escape sequence: Arrow keys send ESC (27) then '[' then final char.
        if (ch == 27)
        {
            char seq[2];
            int res = recv(client_fd, seq, 2, 0);
            if (res == 2 && seq[0] == '[')
            {
                if (seq[1] == 'A') // UP arrow
                {
                    if (!commandHistory.empty() && commandHistoryIndex > 0)
                    {
                        commandHistoryIndex--;
                        std::string clearLine = "\r" + std::string(prompt.size() + buffer.size(), ' ') + "\r";
                        send(client_fd, clearLine.c_str(), clearLine.size(), 0);
                        buffer = commandHistory[commandHistoryIndex];
                        std::string cmdLine = prompt + buffer;
                        send(client_fd, cmdLine.c_str(), cmdLine.size(), 0);
                    }
                }
                else if (seq[1] == 'B') // DOWN arrow
                {
                    if (!commandHistory.empty() && commandHistoryIndex < static_cast<int>(commandHistory.size()) - 1)
                    {
                        commandHistoryIndex++;
                        std::string clearLine = "\r" + std::string(prompt.size() + buffer.size(), ' ') + "\r";
                        send(client_fd, clearLine.c_str(), clearLine.size(), 0);
                        buffer = commandHistory[commandHistoryIndex];
                        std::string cmdLine = prompt + buffer;
                        send(client_fd, cmdLine.c_str(), cmdLine.size(), 0);
                    }
                    else if (!commandHistory.empty() && commandHistoryIndex == static_cast<int>(commandHistory.size()) - 1)
                    {
                        commandHistoryIndex++;
                        std::string clearLine = "\r" + std::string(prompt.size() + buffer.size(), ' ') + "\r";
                        send(client_fd, clearLine.c_str(), clearLine.size(), 0);
                        buffer.clear(); // Clear the buffer for the next command
                        std::string cmdLine = prompt;
                        send(client_fd, cmdLine.c_str(), cmdLine.size(), 0);
                    }
                }
            }
            continue; // Ignore the rest of the escape sequence
        }

        // Ignore non-printable characters
        if (!isalnum(ch) && !isprint(ch) && ch != '\n' && ch != '\r')
            continue;
        // If carriage return, ignore it (we handle newline)
        if (ch == '\r')
        {
            continue;
        }
        if (ch == '\n')
        {
            // Send a newline to properly break the line.
            const char *newline = "\n\r";
            send(client_fd, newline, 2, 0);
            // Process the accumulated command if any (trim trailing \r)
            if (!buffer.empty() && buffer.back() == '\r')
                buffer.pop_back();

            if (!buffer.empty())
            {
                std::string response;
                processCommand(buffer, response);
                send(client_fd, response.c_str(), static_cast<int>(response.length()), 0);
                send(client_fd, newline, 2, 0);
                commandHistory.push_back(buffer);
                commandHistoryIndex = commandHistory.size();
            }
            buffer.clear();                                    // Clear the buffer for the next command
            send(client_fd, prompt.c_str(), prompt.size(), 0); // Send prompt again
        }
        else
        {
            buffer.push_back(ch);
        }
    }
    std::cout << "Client disconnected: " << client_fd << "\n";
#ifdef _WIN32
    closesocket(client_fd);
#else
    close(client_fd);
#endif
}

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

        // Create a thread pool with a fixed number of threads.
        const size_t numThreads = std::thread::hardware_concurrency();
        ThreadPool pool(numThreads == 0 ? 4 : numThreads); // default to 4 if hardware_concurrency returns 0

        // Main server loop using select() for a timeout.
        while (running)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(server_fd, &readfds);

            timeval tv;
            tv.tv_sec = 1; // Timeout of 1 second.
            tv.tv_usec = 0;

            int ret = select(server_fd + 1, &readfds, nullptr, nullptr, &tv);
            if (ret > 0 && FD_ISSET(server_fd, &readfds))
            {
                int client_fd = accept(server_fd, nullptr, nullptr);
                if (client_fd >= 0)
                {
                    pool.enqueue([client_fd]
                                 { handleClient(client_fd); });
                }
            }
            // If ret == 0, it means timeout expired, and we loop again to check 'running'
        }

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
