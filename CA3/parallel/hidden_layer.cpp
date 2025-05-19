#include "hidden_layer.hpp"


struct HiddenTaskArgs {
    HiddenLayer* layer;
    int start, end;
    std::vector<double>* input;
    std::vector<double>* output;
};


HiddenLayer::HiddenLayer(ThreadPool* pool, int numNeurons, int inputDim)
    : Layer(pool), neuronCount(numNeurons), inputSize(inputDim) {
    neurons.resize(neuronCount);
    for (auto& neuron : neurons)
        neuron.weights.resize(inputSize);
}

void HiddenLayer::loadParams(const std::string& weightsFile, const std::string& biasFile) {
    std::ifstream wfile(weightsFile), bfile(biasFile);
    for (int i = 0; i < neuronCount; ++i) {
        for (int j = 0; j < inputSize; ++j) {
            wfile >> neurons[i].weights[j];
        }
        bfile >> neurons[i].bias;
    }
}

void HiddenLayer::start() {
    pthread_create(&thread, nullptr, [](void* arg) -> void* {
        static_cast<HiddenLayer*>(arg)->processLoop();
        return nullptr;
    }, this);
}

void HiddenLayer::processLoop() {
    while (true) {
        sem_wait(&dataAvailable);

        pthread_mutex_lock(&queueMutex);
        if (inputQueue.empty()) {
            pthread_mutex_unlock(&queueMutex);
            continue;
        }
        LayerData task = inputQueue.front();
        inputQueue.pop();
        pthread_mutex_unlock(&queueMutex);

        std::vector<double> result(neuronCount);
        int workers = 8;
        int chunk = neuronCount / workers;

        for (int i = 0; i < workers; ++i) {
            int start = i * chunk;
            int end = (i == workers - 1) ? neuronCount : start + chunk;
            auto* args = new HiddenTaskArgs{this, start, end, &task.values, &result};
            threadPool->enqueueTask([](void* arg) {
                auto* args = static_cast<HiddenTaskArgs*>(arg);
                args->layer->computeRange(args->start, args->end, *args->input, *args->output);
                delete args;
            }, args);
        }
        if (task.backSemaphore)
            sem_post(task.backSemaphore);
    }
}

void HiddenLayer::computeRange(int startIdx, int endIdx, const std::vector<double>& input, std::vector<double>& result) {
    for (int i = startIdx; i < endIdx; ++i) {
        double sum = neurons[i].bias;
        for (int j = 0; j < inputSize; ++j)
            sum += neurons[i].weights[j] * input[j];
        result[i] = std::max(0.0, sum); // ReLU
    }
}