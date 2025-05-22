#ifndef TREADPOOL_HPP
#define TREADPOOL_HPP

#include "define.hpp"


class ThreadPool {
public:
    ThreadPool(size_t min_threads, size_t max_threads, ThreadPriorityLevel thread_priority);
    ~ThreadPool();

    void enqueueTask(void (*func)(void*), void* arg);

    void shutdown();

    bool isIdle();

private:
    static void* workerWrapper(void* arg);
    void workerThread();

    static void* monitorWrapper(void* arg);
    void monitorThread();

    void setThreadPriority(pthread_t thread);

    std::vector<pthread_t> workers;
    pthread_t monitor;

    std::queue<Task> tasks;

    pthread_mutex_t queue_mutex;
    pthread_cond_t condition;

    size_t min_threads;
    size_t max_threads;
    std::atomic<bool> stop;
    std::atomic<size_t> active_threads;

    ThreadPriorityLevel thread_priority_level;
};

#endif
