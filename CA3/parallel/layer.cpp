#include "layer.hpp"

Layer::Layer(ThreadPool* pool , int threadCount) : nextLayer(nullptr), threadPool(pool), threadCount(threadCount) {
    pthread_mutex_init(&queueMutex, nullptr);
    sem_init(&dataAvailable, 0, 0);
}

Layer::~Layer() {
    pthread_mutex_destroy(&queueMutex);
    sem_destroy(&dataAvailable);
}

void Layer::setNextLayer(Layer* next) {
    this->nextLayer = next;
}

void Layer::enqueueInput(const LayerData& data) {
    pthread_mutex_lock(&queueMutex);
    inputQueue.push(data);
    pthread_mutex_unlock(&queueMutex);
    sem_post(&dataAvailable);
}

bool Layer::isQueueEmpty() {
    pthread_mutex_lock(&queueMutex);
    bool result = inputQueue.empty();
    pthread_mutex_unlock(&queueMutex);
    return result;
}
