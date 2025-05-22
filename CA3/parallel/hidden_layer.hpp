#ifndef HIDDEN_LAYER_HPP
#define HIDDEN_LAYER_HPP

#include "layer.hpp"
#include "define.hpp"
#include "shared_functions.hpp"
#include "threadpool.hpp"


class HiddenLayer : public Layer {
private:
    string hiddenWeightsFile = HIDDEN_WEIGHTS_FILE;
    string hiddenBiasesFile = HIDDEN_BIASES_FILE;
    std::vector<Hidden_Node> hidden_nodes;
    int dataPerThread;

    static void* processLoop(void* arg);
    void handleInput(LayerData data);
    static void computeTask(void* arg);
    void allocateHiddenParameters(string weightsFile, string biasesFile);

public:
    HiddenLayer(ThreadPool* pool, int neuronCount, int threadCount , string weightsFile, string biasesFile);
    ~HiddenLayer();
    void start() override;
};


#endif