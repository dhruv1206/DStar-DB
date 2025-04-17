#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include "Client.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class ClientManager
{
public:
    // Add a new client.
    void addClient(int sockfd)
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients[sockfd] = std::make_unique<Client>(sockfd);
    }

    // Remove a client.
    void removeClient(int sockfd)
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(sockfd);
    }

    // Get a snapshot copy of active clients.
    std::unordered_map<int, Client *> getClients()
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        std::unordered_map<int, Client *> snapshot;
        for (auto &p : clients)
        {
            snapshot[p.first] = p.second.get();
        }
        return snapshot;
    }

private:
    std::unordered_map<int, std::unique_ptr<Client>> clients;
    std::mutex clientsMutex;
};

#endif // CLIENT_MANAGER_H
