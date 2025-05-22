#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "define.hpp"
#include "layer.hpp"
#include "threadpool.hpp"
#include "input_layer.hpp"
#include "hidden_layer.hpp"
#include "output_layer.hpp"
#include "result_layer.hpp"

class ResultLayer;

class Controller{
private:
    int inputThreadsMin;
    int inputThreadsMax;
    int outputThreadsMin;
    int outputThreadsMax;
    int hiddenThreadsMin;
    int hiddenThreadsMax;

    int imagesCount;

    vector<ThreadPool*> threadPools;
    ThreadPool* input_pool;
    ThreadPool* output_pool;
    vector<ThreadPool*> hiddenPools;

    vector<Layer*> layers;
    InputLayer* input;
    OutputLayer* output;
    vector<HiddenLayer*> hiddenLayers;
    ResultLayer* result;

    pthread_mutex_t finishMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t finishCond = PTHREAD_COND_INITIALIZER;
    bool finished = false;

    int hiddenLayerCount;
    int neuronsPerLayer;

    void createThreadPools();
    void createLayers();
    void connectLayers();
    void startLayers();
    bool allFinish();
    
    public:
    Controller(int hiddenLayerCount, int neuronsPerLayer , int imagesCount);
    ~Controller();
    void run();
    
    int getImageCount();
    void waitUntilFinished();
    void notifyIfFinished();
};

#endif