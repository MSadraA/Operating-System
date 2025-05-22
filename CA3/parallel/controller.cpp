#include "controller.hpp"

Controller::Controller(int hiddenLayerCount_, int neuronsPerLayer_ , int imagesCount_){
    this->hiddenLayerCount = hiddenLayerCount_;
    this->neuronsPerLayer = neuronsPerLayer_;
    this->imagesCount = imagesCount_;

    this->inputThreadsMin = 1;
    this->inputThreadsMax = 1;
    this->outputThreadsMin = 10;
    this->outputThreadsMax = 20;
    this->hiddenThreadsMin = 8;
    this->hiddenThreadsMax = 18;
}

Controller::~Controller() {
    for (auto layer : layers) {
        delete layer;
    }

    for (auto pool : threadPools) {
        delete pool;
    }

    hiddenLayers.clear();
    hiddenPools.clear();
    layers.clear();
    threadPools.clear();
}


void Controller::createThreadPools(){
    input_pool = new ThreadPool(inputThreadsMin, inputThreadsMax, ThreadPriorityLevel::LOW);
    output_pool = new ThreadPool(outputThreadsMin, outputThreadsMax, ThreadPriorityLevel::HIGH);
    threadPools.push_back(input_pool);
    threadPools.push_back(output_pool);
    for (int i = 0; i < hiddenLayerCount; ++i) {
        hiddenPools.push_back(new ThreadPool(hiddenThreadsMin, hiddenThreadsMax, ThreadPriorityLevel::MEDIUM));
        threadPools.push_back(hiddenPools[i]);
    }
}

void Controller::createLayers(){
    input = new InputLayer(input_pool, MNIST_TESTING_SET_IMAGE_FILE_NAME, MNIST_TESTING_SET_LABEL_FILE_NAME, imagesCount);
    output = new OutputLayer(output_pool , outputThreadsMin , outputThreadsMin);
    layers.push_back(input);
    layers.push_back(output);

    for (int i = 0; i < hiddenLayerCount; i++) {
        hiddenLayers.push_back(new HiddenLayer(hiddenPools[i] , neuronsPerLayer , hiddenThreadsMin , HIDDEN_WEIGHTS_FILE, HIDDEN_BIASES_FILE));
        layers.push_back(hiddenLayers[i]);
    }
    result = new ResultLayer(this);
    layers.push_back(result);
}

void Controller::connectLayers(){
    input->setNextLayer(hiddenLayers[0]);
    for (int i = 0; i < hiddenLayerCount - 1; ++i) {
        hiddenLayers[i]->setNextLayer(hiddenLayers[i+1]);
    }
    hiddenLayers[hiddenLayerCount - 1]->setNextLayer(output);
    output->setNextLayer(result);
}

void Controller::startLayers(){
    for (int i = layers.size() - 1; i >= 0; --i) {
        layers[i]->start();
    }
}

bool Controller::allFinish(){
    for(auto layer : layers){
        if(!layer->isQueueEmpty())
            return false;
    }
    return true;
}

void Controller::waitUntilFinished() {
    pthread_mutex_lock(&finishMutex);
    while (!finished) {
        pthread_cond_wait(&finishCond, &finishMutex);
    }
    pthread_mutex_unlock(&finishMutex);
}

void Controller::notifyIfFinished() {
    pthread_mutex_lock(&finishMutex);
    finished = true;
    pthread_cond_signal(&finishCond);
    pthread_mutex_unlock(&finishMutex);
}

int Controller::getImageCount() {
    return imagesCount;
}

void Controller::run() {
    createThreadPools();
    createLayers();
    connectLayers();
    startLayers();
    waitUntilFinished();
}

