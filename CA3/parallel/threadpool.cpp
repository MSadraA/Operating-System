#include "threadpool.hpp"

void* ThreadPool::workerWrapper(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    pool->setThreadPriority(pthread_self());
    pool->workerThread();
    return nullptr;
}

ThreadPool::ThreadPool(size_t min_t, size_t max_t, ThreadPriorityLevel priority_level)
    : min_threads(min_t), max_threads(max_t), stop(false), active_threads(0), thread_priority_level(priority_level) {

    pthread_mutex_init(&queue_mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    for (size_t i = 0; i < min_threads; ++i) {
        pthread_t thread;
        if (pthread_create(&thread, nullptr, workerWrapper, this) == 0) {
            workers.push_back(thread);
            active_threads++;
        }
    }

    pthread_create(&monitor, nullptr, monitorWrapper, this);
}

ThreadPool::~ThreadPool() {
    shutdown();
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&condition);
}

void ThreadPool::enqueueTask(void (*func)(void*), void* arg) {
    pthread_mutex_lock(&queue_mutex);
    tasks.push(Task{func, arg});
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

void ThreadPool::workerThread() {
    while (true) {
        pthread_mutex_lock(&queue_mutex);

        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 2;

        int result = 0;
        while (!stop && tasks.empty() && result != ETIMEDOUT) {
            result = pthread_cond_timedwait(&condition, &queue_mutex, &timeout);
        }

        if (stop || (result == ETIMEDOUT && active_threads > min_threads)) {
            active_threads--;
            for (auto it = workers.begin(); it != workers.end(); ++it) {
                if (pthread_equal(*it, pthread_self())) {
                    workers.erase(it);
                    break;
                }
            }
            pthread_mutex_unlock(&queue_mutex);
            return;
        }

        if (!tasks.empty()) {
            Task task = tasks.front();
            tasks.pop();
            pthread_mutex_unlock(&queue_mutex);
            task.function(task.arg);
        } else {
            pthread_mutex_unlock(&queue_mutex);
        }
    }
}

void* ThreadPool::monitorWrapper(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    pool->monitorThread();
    return nullptr;
}

void ThreadPool::monitorThread() {
    while (!stop) {
        usleep(TIME_FOR_MONITORING);

        pthread_mutex_lock(&queue_mutex);
        size_t queue_size = tasks.size();
        size_t current_threads = active_threads.load();
        pthread_mutex_unlock(&queue_mutex);

        if (queue_size > current_threads && current_threads < max_threads) {
            pthread_t thread;
            if (pthread_create(&thread, nullptr, workerWrapper, this) == 0) {
                pthread_mutex_lock(&queue_mutex);
                workers.push_back(thread);
                active_threads++;
                pthread_mutex_unlock(&queue_mutex);
            }
        }
    }
}

void ThreadPool::setThreadPriority(pthread_t thread) {
    sched_param sch;
    int policy = SCHED_RR;
    int priority;

    switch (thread_priority_level) {
        case ThreadPriorityLevel::LOW: priority = 10; break;
        case ThreadPriorityLevel::MEDIUM: priority = 20; break;
        case ThreadPriorityLevel::HIGH: priority = 30; break;
    }

    sch.sched_priority = priority;
    if (pthread_setschedparam(thread, policy, &sch) != 0) {
        std::cerr << "[WARN] Failed to set thread priority. (Try running as root)\n";
    }
}
