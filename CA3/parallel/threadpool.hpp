#ifndef TREADPOOL_HPP
#define TREADPOOL_HPP

#include "define.hpp"


class ThreadPool {
public:
    ThreadPool(size_t min_threads, size_t max_threads, ThreadPriorityLevel thread_priority);
    ~ThreadPool();

    // افزودن یک وظیفه به صف
    void enqueueTask(void (*func)(void*), void* arg);

    // توقف کامل
    void shutdown();

    // بررسی خالی بودن صف وظایف
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
