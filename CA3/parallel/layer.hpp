// layer.hpp
#ifndef LAYER_HPP
#define LAYER_HPP


#include "define.hpp"
#include "threadpool.hpp"



class Layer {
protected:
    queue<LayerData> inputQueue;
    pthread_mutex_t queueMutex;
    sem_t dataAvailable;


    Layer* nextLayer;
    ThreadPool* threadPool;

    int threadCount;

public:
    Layer(ThreadPool* pool, int threadCount);
    virtual ~Layer();

    bool isQueueEmpty();

    void setNextLayer(Layer* next);

    virtual void start() = 0;
    virtual void enqueueInput(const LayerData& data);
};

#endif
