#include "output_layer.hpp"
#include <cmath>

struct OutputTaskArgs {
    OutputLayer* layer;
    const LayerData* input;
    std::vector<double>* outputs;
    int startIndex;
    int endIndex;
    std::atomic<int>* doneCount;
    int totalThreads;
};

OutputLayer::OutputLayer(ThreadPool* pool, int outputCount, int threadCount)
    : Layer(pool, threadCount) {
    output_nodes.resize(outputCount);
    allocateOutputParameters();
}

OutputLayer::~OutputLayer() {
    sem_destroy(&dataAvailable);

    pthread_mutex_lock(&queueMutex);
    while (!inputQueue.empty()) {
        inputQueue.pop();
    }
    pthread_mutex_unlock(&queueMutex);
}

void OutputLayer::start() {
    pthread_t thread;
    pthread_create(&thread, nullptr, OutputLayer::processLoop, this);
    pthread_detach(thread);
}

void* OutputLayer::processLoop(void* arg) {
    auto* layer = static_cast<OutputLayer*>(arg);
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

        auto* outputs = new std::vector<double>(layer->output_nodes.size(), 0.0);
        auto* doneCount = new std::atomic<int>(0);

        int total = layer->output_nodes.size();
        int chunk = (total + layer->threadCount - 1) / layer->threadCount;

        for (int i = 0; i < layer->threadCount; ++i) {
            int start = i * chunk;
            int end = std::min(total, start + chunk);
            auto* args = new OutputTaskArgs{layer, task, outputs, start, end, doneCount, layer->threadCount};
            layer->threadPool->enqueueTask(OutputLayer::computeTask, args);
        }
    }
    return nullptr;
}

void OutputLayer::computeTask(void* rawArg) {
    auto* args = static_cast<OutputTaskArgs*>(rawArg);

    for (int i = args->startIndex; i < args->endIndex; ++i) {
        double sum = args->layer->output_nodes[i].bias;
        for (size_t j = 0; j < args->input->values.size(); ++j) {
            sum += args->input->values[j] * args->layer->output_nodes[i].weights[j];
        }
        double out = 1.0 / (1.0 + std::exp(-sum)); // sigmoid
        args->layer->output_nodes[i].output = out;
        (*args->outputs)[i] = out;
    }

    if (++(*args->doneCount) == args->totalThreads) {
        LayerData output;
        output.values = *args->outputs;
        output.label = args->input->label;

        if (args->layer->nextLayer) {
            args->layer->nextLayer->enqueueInput(output);
        }
        
        delete args->outputs;
        delete args->doneCount;
        delete args->input;
    }

    delete args;
}

void OutputLayer::allocateOutputParameters(){
    size_t idx = 0;
    size_t bidx = 0;

    std::ifstream weights(OUTPUT_WEIGHTS_FILE);
    for(std::string line; std::getline(weights, line); ) {
        std::stringstream in(line);
        for (int i = 0; i < 256; ++i) {
            in >> output_nodes[idx].weights[i];
        }
        idx++;
    }
    weights.close();

    std::ifstream biases(OUTPUT_BIASES_FILE);
    for(std::string line; std::getline(biases, line); ) {
        std::stringstream in(line);
        in >> output_nodes[bidx].bias;
        bidx++;
    }
    biases.close();
}
