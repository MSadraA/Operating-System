#ifndef TREADPOOL_HPP
#define TREADPOOL_HPP

#include "define.hpp"


class ThreadPool {
    private:
        static void* worker_wrapper(void* arg);
        void worker_thread();

        static void* monitor_wrapper(void* arg);
        void monitor_thread();

        vector<pthread_t> workers;
        pthread_t monitor;
    
        priority_queue<PrioritizedTask> tasks;
    
        pthread_mutex_t queue_mutex;
        pthread_cond_t condition;
    
        size_t min_threads;
        size_t max_threads;
        atomic<bool> stop;
        atomic<size_t> active_threads;

    public:
        ThreadPool(size_t min_threads, size_t max_threads);
        ~ThreadPool();

        void enqueueTask(void (*func)(void*), void* arg, TaskPriority priority = TaskPriority::MEDIUM);

        void shutdown();

        bool isIdle();
};
#endif
