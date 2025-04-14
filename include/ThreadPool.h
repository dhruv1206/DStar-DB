#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdexcept>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>

class ThreadPool
{
public:
    ThreadPool(size_t threads) : stop(false)
    {
        for (size_t i = 0; i < threads; i++)
        {
            workers.emplace_back(
                [this]
                {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            // acquire lock to access the queue
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            //  wait until there is a task or the queue is stopping
                            this->condition.wait(lock, [this]
                                                 { return this->stop || !this->tasks.empty(); });
                            // Is the pool is stopping and there are no tasks left, exit;
                            if (this->stop && this->tasks.empty())
                            {
                                return;
                            }
                            // Retrieve the next task.
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                        // Execute the retrieved task outside the lock.
                        task();
                    }
                });
        }
    }

    template <class F>
    void enqueue(F &&f)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
            {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace(std::forward<F>(f));
        }
        // Notify one worker that there is a new task
        condition.notify_one();
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join(); // Wait for each worker thread to finish.
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_H
