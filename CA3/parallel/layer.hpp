#ifndef LAYER_HPP
#define LAYER_HPP

#include "define.hpp"
#include "threadpool.hpp"

class Layer {
protected:
    std::queue<LayerData> inputQueue;
    pthread_mutex_t queueMutex;
    sem_t dataAvailable;
    sem_t* prevSemaphore; 

    Layer* nextLayer; // لایه بعدی (می‌تواند nullptr باشد اگر این لایه خروجی است)

    ThreadPool* threadPool; // Thread pool مشترک برای اجرای وظایف نورون‌ها

public:
    Layer(ThreadPool* pool);
    virtual ~Layer();

    void setNextLayer(Layer* next);
    void setPrevSemaphore(sem_t* sem);
    sem_t* getPermitSemaphore();

    void enqueueInput(const LayerData& data);

    virtual void start() = 0;

protected:
    virtual void processLoop() = 0;
};

#endif
