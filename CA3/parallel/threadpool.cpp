#include "threadpool.hpp"

void* ThreadPool::worker_wrapper(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    pool->worker_thread();
    return nullptr;
}

ThreadPool::ThreadPool(size_t min_t, size_t max_t)
    : min_threads(min_t), max_threads(max_t), stop(false), active_threads(0) {

    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    for (size_t i = 0; i < min_threads; ++i) {
        pthread_t thread;
        if (pthread_create(&thread, nullptr, worker_wrapper, this) == 0) {
            workers.push_back(thread);
            active_threads++;
        }
    }

    pthread_create(&monitor, nullptr, monitor_wrapper, this);
}

ThreadPool::~ThreadPool() {
    shutdown();
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&condition);
}

void ThreadPool::enqueueTask(void (*func)(void*), void* arg, TaskPriority priority) {
    pthread_mutex_lock(&queue_mutex);
    tasks.push(PrioritizedTask{func, arg, priority});
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&condition);
}

void ThreadPool::shutdown() {
    pthread_mutex_lock(&queue_mutex);
    stop = true;
    pthread_mutex_unlock(&queue_mutex);

    pthread_cond_broadcast(&condition);

    for (pthread_t& t : workers) {
        pthread_join(t, nullptr);
    }
    pthread_join(monitor, nullptr);

    workers.clear();
}

bool ThreadPool::isIdle() {
    pthread_mutex_lock(&queue_mutex);
    bool empty = tasks.empty();
    pthread_mutex_unlock(&queue_mutex);
    return empty;
}

void ThreadPool::worker_thread() {
    while (true) {
        pthread_mutex_lock(&queue_mutex);

        while (!stop && tasks.empty()) {
            pthread_cond_wait(&condition, &queue_mutex);
        }

        if (stop && tasks.empty()) {
            pthread_mutex_unlock(&queue_mutex);
            return;
        }

        PrioritizedTask task = tasks.top();
        tasks.pop();

        pthread_mutex_unlock(&queue_mutex);
        task.function(task.arg);
    }
}

void* ThreadPool::monitor_wrapper(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    pool->monitor_thread();
    return nullptr;
}

void ThreadPool::monitor_thread() {
    while (!stop) {
        usleep(SLEEP_TIME);

        pthread_mutex_lock(&queue_mutex);
        size_t queue_size = tasks.size();
        size_t current_threads = active_threads.load();
        pthread_mutex_unlock(&queue_mutex);

        if (queue_size > current_threads && current_threads < max_threads) {
            pthread_t thread;
            if (pthread_create(&thread, nullptr, worker_wrapper, this) == 0) {
                pthread_mutex_lock(&queue_mutex);
                workers.push_back(thread);
                active_threads++;
                pthread_mutex_unlock(&queue_mutex);
            }
        }
    }
}
