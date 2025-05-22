#include "hidden_layer.hpp"


struct HiddenTaskArgs {
    HiddenLayer* layer;
    const LayerData* input;
    std::vector<double>* output;
    int start;
    int end;
    std::atomic<int>* doneCount;
    int totalThreads;
};


HiddenLayer::HiddenLayer(ThreadPool* pool, int neuronCount, int threadCount , string weightsFile, string biasesFile)
    : Layer(pool, threadCount) , hiddenWeightsFile(weightsFile), hiddenBiasesFile(biasesFile) {
    hidden_nodes.resize(neuronCount);
    allocateHiddenParameters(weightsFile, biasesFile);
    dataPerThread = neuronCount / threadCount;
}

HiddenLayer::~HiddenLayer() {
    sem_destroy(&dataAvailable);

    pthread_mutex_lock(&queueMutex);
    while (!inputQueue.empty()) {
        inputQueue.pop();
    }
    pthread_mutex_unlock(&queueMutex);
}

void HiddenLayer::start() {
    pthread_t thread;
    pthread_create(&thread, nullptr, HiddenLayer::processLoop, this);
}

void* HiddenLayer::processLoop(void* arg) {
    auto* layer = static_cast<HiddenLayer*>(arg);
    while (true) {
        sem_wait(&layer->dataAvailable);

        pthread_mutex_lock(&layer->queueMutex);
        if (layer->inputQueue.empty()) {
            pthread_mutex_unlock(&layer->queueMutex);
            continue;
        }
        LayerData* task = new LayerData(layer->inputQueue.front());
        layer->inputQueue.pop();

        pthread_mutex_unlock(&layer->queueMutex);

        auto output = new std::vector<double>(layer->hidden_nodes.size());
        auto* doneCount = new std::atomic<int>(0);

        int total = layer->hidden_nodes.size();
        int chunk = (total + layer->threadCount - 1) / layer->threadCount;


        for (int i = 0; i < layer->threadCount; ++i) {
            int start = i * chunk;
            int end = std::min(total, start + chunk);
            auto* args = new HiddenTaskArgs{layer, task, output, start, end, doneCount, layer->threadCount};
            layer->threadPool->enqueueTask(HiddenLayer::computeTask, args);
        }
    }
    return nullptr;
}

void HiddenLayer::computeTask(void* arg) {
    auto* args = static_cast<HiddenTaskArgs*>(arg);
    for (int i = args->start; i < args->end; ++i) {
        double sum = args->layer->hidden_nodes[i].bias;
        for (size_t j = 0; j < args->input->values.size(); ++j) {
            sum += args->input->values[j] * args->layer->hidden_nodes[i].weights[j];
        }
        args->layer->hidden_nodes[i].output = std::max(0.0, sum);
        (*args->output)[i] = args->layer->hidden_nodes[i].output;
    }

    if (++(*args->doneCount) == args->totalThreads) {
        LayerData out;
        out.values = *args->output;
        out.label = args->input->label;
        if(args->layer->nextLayer != nullptr) {
            args->layer->nextLayer->enqueueInput(out);
        }
        delete args->output;
        delete args->doneCount;
        delete args->input;
    }
    delete args;
}

void HiddenLayer::allocateHiddenParameters(string weightsFile, string biasesFile) {
    size_t idx = 0;
    size_t bidx = 0;


    std::ifstream weights(weightsFile);
    for (std::string line; getline(weights, line) && idx < hidden_nodes.size(); ) {
        std::stringstream in(line);
        for (int i = 0; i < 28 * 28; ++i) {
            in >> hidden_nodes[idx].weights[i];
        }
        idx++;
    }
    weights.close();

    std::ifstream biases(biasesFile);
    for (std::string line; getline(biases, line) && bidx < hidden_nodes.size(); ) {
        std::stringstream in(line);
        in >> hidden_nodes[bidx].bias;
        bidx++;
    }
    biases.close();
}