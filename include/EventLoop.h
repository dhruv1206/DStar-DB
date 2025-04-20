#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

#include "../include/ClientManager/ClientManager.h"
#include "../include/CommandProcessor/CommandProcessor.h"
#include "GlobalThreadPool.h"
#include <atomic>
#include <iostream>
#include <vector>

class EventLoop
{
public:
    // Constructor takes the listening socket and a reference to the client manager.
    EventLoop(int listenSocket, ClientManager &clientMgr, std::shared_ptr<CommandProcessor> cmdProc)
        : listenSock(listenSocket), clientManager(clientMgr), commandProcessor(cmdProc), running(true) {}

    // Runs the main event loop.
    void run()
    {
        while (running.load())
        {
            // Prepare the read file descriptor set.
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(listenSock, &readfds);
            int maxfd = listenSock;

            // Get a snapshot of current clients.
            auto clients = clientManager.getClients();
            for (const auto &p : clients)
            {
                int sock = p.first;
                FD_SET(sock, &readfds);
                if (sock > maxfd)
                    maxfd = sock;
            }

            // Set a timeout for select() (1 second here).
            timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            // Wait for an event on any file descriptor.
            int ret = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);
            if (ret < 0)
            {
                std::cerr << "Select error: " << strerror(errno) << "\n";
            }

            // 1. Check for new connections on the listening socket.
            if (FD_ISSET(listenSock, &readfds))
            {
                int clientSock = accept(listenSock, nullptr, nullptr);
                if (clientSock >= 0)
                {
                    std::cout << "New client connected: " << clientSock << "\n";
                    clientManager.addClient(clientSock);
                    // Send welcome message to the new client.
                    std::string welcome = welcomeMessage + prompt;
                    clientManager.getClients()[clientSock]->writeData(welcome);
                }
                else
                {
                    std::cerr << "Accept failed\n";
                }
            }

            // 2. Check for data on client sockets.
            for (const auto &p : clients)
            {
                int sock = p.first;
                // If this client socket is ready for reading.
                if (FD_ISSET(sock, &readfds))
                {
                    handleClient(p.second);
                }
            }
        }
    }

    // Stop the event loop.
    void stop()
    {
        running = false;
    }

private:
    int listenSock;
    ClientManager &clientManager;
    std::shared_ptr<CommandProcessor> commandProcessor;
    std::atomic<bool> running;
    const std::string welcomeMessage = "\033[32mWelcome to DStarDB!\033[0m\n\n\r";
    const std::string prompt = "\033[32mDStarDB> \033[0m";

    void handleClient(std::shared_ptr<Client> client)
    {
        const char *normalPrompt = "\033[32mDStarDB> \033[0m";
        const char *transactionPrompt = "\033[32mDStarDB(TXN)> \033[0m";

        int sock = client->getSocket();
        std::string data;
        // readData() reads data from the client.
        if (!client->readData(data, 1))
        {
            std::cout << "Client disconnected: " << sock << "\n";
            clientManager.removeClient(sock);
        }
        else
        {
            char ch = data[0];
            if (ch == 3)
            {
                // send a goodbye message before closing the connection
                std::string goodbye = "Goodbye!\r\n";
                client->writeData(goodbye);
                std::cout << "Client disconnected: " << sock << "\n";
                clientManager.removeClient(sock);
                return;
            }
            // Check for backspace (ASCII 8 or 127)
            if (ch == 8 || ch == 127)
            {
                if (!client->getBuffer().empty())
                {
                    std::string buffer = client->getBuffer();
                    buffer.pop_back();
                    client->clearBuffer(); // Clear the buffer for the next command
                    client->appendToBuffer(buffer);
                    // Send backspace, space, then backspace again to erase on client's terminal
                    const char *bs = " \b";
                    client->writeData(bs, 2);
                }
                else
                {
                    client->writeData(" ", 1);
                }
                // If buffer empty, ignore further backspaces.
                return;
            }
            // Check for escape sequence: Arrow keys send ESC (27) then '[' then final char.
            if (ch == 27)
            {
                std::string arrowInput;
                if (client->readData(arrowInput, 2))
                {
                    char seq[2];
                    seq[0] = arrowInput[0];
                    seq[1] = arrowInput[1];
                    if (seq[0] != '[')
                        return; // Ignore if not an escape sequence

                    if (seq[1] == 'A') // UP arrow
                    {
                        if (!client->getCommandHistory().empty() && client->getCommandHistoryIndex() > 0)
                        {
                            client->setCommandHistoryIndex(client->getCommandHistoryIndex() - 1);
                            std::string clearLine = "\r" + std::string(prompt.size() + client->getBuffer().size(), ' ') + "\r";
                            client->writeData(clearLine);
                            client->clearBuffer(); // Clear the buffer for the next command
                            client->appendToBuffer(client->getCommandHistory()[client->getCommandHistoryIndex()]);
                            std::string cmdLine = prompt + client->getBuffer();
                            client->writeData(cmdLine);
                        }
                    }
                    else if (seq[1] == 'B') // DOWN arrow
                    {
                        if (!client->getCommandHistory().empty() && client->getCommandHistoryIndex() < static_cast<int>(client->getCommandHistory().size()) - 1)
                        {
                            client->setCommandHistoryIndex(client->getCommandHistoryIndex() + 1);
                            std::string clearLine = "\r" + std::string(prompt.size() + client->getBuffer().size(), ' ') + "\r";
                            client->writeData(clearLine);
                            client->clearBuffer(); // Clear the buffer for the next command
                            client->appendToBuffer(client->getCommandHistory()[client->getCommandHistoryIndex()]);
                            std::string cmdLine = prompt + client->getBuffer();
                            client->writeData(cmdLine);
                        }
                        else if (!client->getCommandHistory().empty() && client->getCommandHistoryIndex() == static_cast<int>(client->getCommandHistory().size()) - 1)
                        {
                            client->setCommandHistoryIndex(client->getCommandHistoryIndex() + 1);
                            std::string clearLine = "\r" + std::string(prompt.size() + client->getBuffer().size(), ' ') + "\r";
                            client->writeData(clearLine);
                            client->clearBuffer(); // Clear the buffer for the next command
                            std::string cmdLine = prompt;
                            client->writeData(cmdLine);
                        }
                    }
                }
                return; // Ignore the rest of the escape sequence
            }

            // Ignore non-printable characters
            if (!isalnum(ch) && !isprint(ch) && ch != '\n' && ch != '\r')
                return;
            // If carriage return, ignore it (we handle newline)
            if (ch == '\r')
            {
                return;
            }
            if (ch == '\n')
            {
                std::shared_ptr<CommandProcessor> localCommandProcessor = commandProcessor;
                GlobalThreadPool::getInstance().enqueue(
                    [normalPrompt, transactionPrompt, localCommandProcessor, client]()
                    {
                        // Send a newline to properly break the line.
                        const char *newline = "\n\r";
                        client->writeData(newline, 2);

                        std::string buffer = client->getBuffer();
                        if (!buffer.empty() && buffer.back() == '\r')
                            buffer.pop_back(); // Remove trailing \r
                        if (!buffer.empty())
                        {
                            std::string response;
                            localCommandProcessor->processCommand(buffer, response, client);
                            client->writeData(response);
                            client->writeData(newline, 2);
                            client->addCommandToHistory(buffer);
                        }
                        client->clearBuffer();                                                                             // Clear the buffer for the next command
                        client->writeData((client->transactionContext->inTransaction ? transactionPrompt : normalPrompt)); // Send prompt again
                    });
            }
            else
            {
                client->appendToBuffer(std::string(1, ch)); // Append character to buffer
            }
        }
    }
};

#endif // EVENT_LOOP_H
