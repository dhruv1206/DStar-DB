#pragma once

#include <memory>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include "../Database/IDatabase.h"
#include "TransactionContext.h"
#include "../Database/Observer/IDatabaseObserver.h"

class Client : public IDatabaseObserver
{
public:
    Client(int socket_fd) : sockfd(socket_fd)
    {
        lastActivity = std::chrono::steady_clock::now();
        transactionContext = std::make_unique<TransactionContext>();
    }

    ~Client()
    {
#ifdef _WIN32
        closesocket(sockfd);
#else
        close(sockfd);
#endif
    }

    int getSocket() const
    {
        return sockfd;
    }

    // Read data from client. Returns false if client disconnects.
    bool readData(std::string &data, size_t buff_len = 1024)
    {
        char buffer[buff_len];
        int n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n <= 0)
            return false;
        data.append(buffer, n);
        updateActivity();
        return true;
    }

    // Write data to client.
    bool writeData(const std::string &data, size_t len = 0)
    {
        if (len == 0)
            len = data.size();
        int sent = send(sockfd, data.c_str(), len, 0);
        updateActivity();
        return sent == (int)len;
    }

    // Update the last activity time.
    void updateActivity()
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        lastActivity = std::chrono::steady_clock::now();
    }

    std::chrono::steady_clock::time_point getLastActivity() const
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        return lastActivity;
    }

    // Buffer management for command parsing.
    void appendToBuffer(const std::string &data)
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.append(data);
    }

    std::string getBuffer()
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        return buffer;
    }

    // Clear the command buffer.
    void clearBuffer()
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer.clear();
    }

    // Add command to history.
    void addCommandToHistory(const std::string &command)
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        commandHistory.push_back(command);
        commandHistoryIndex = commandHistory.size();
    }

    void setCommandHistoryIndex(int index)
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        commandHistoryIndex = index;
    }

    std::vector<std::string> getCommandHistory() const
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        return commandHistory;
    }
    int getCommandHistoryIndex() const
    {
        std::lock_guard<std::mutex> lock(activityMutex);
        return commandHistoryIndex;
    }

    void onRecordModified(const std::string &operation, const std::string &key) override
    {
        std::lock_guard<std::mutex> lock(txMutex);
        if (transactionContext->watchedRecords.find(key) != transactionContext->watchedRecords.end())
        {
            transactionContext->dirty = true;
        }
    }

    void onRecordModified(const std::string &operation, const std::string &key, IValue *) override
    {
        onRecordModified(operation, key);
    }

    void clearTransaction(IDatabase *db)
    {
        std::lock_guard<std::mutex> lock(txMutex);
        for (const std::string &watchedRecord : transactionContext->watchedRecords)
        {
            db->unregisterRecordObserver(watchedRecord, this);
        }
        transactionContext->clear();
    }

    std::unique_ptr<TransactionContext> transactionContext;

private:
    int sockfd;
    mutable std::mutex activityMutex;
    std::chrono::steady_clock::time_point lastActivity;
    std::vector<std::string> commandHistory;
    int commandHistoryIndex = -1;
    mutable std::mutex bufferMutex;
    std::string buffer;
    std::mutex txMutex;
};
