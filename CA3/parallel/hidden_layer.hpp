#ifndef HIDDEN_LAYER_HPP
#define HIDDEN_LAYER_HPP

#include "layer.hpp"
#include "define.hpp"
#include "shared_functions.hpp"
#include "threadpool.hpp"


struct HiddenNeuron {
    std::vector<double> weights;
    double bias;
};

class HiddenLayer : public Layer {
private:
    int neuronCount;
    int inputSize;
    std::vector<HiddenNeuron> neurons;
    pthread_t thread;

public:
    HiddenLayer(ThreadPool* pool, int numNeurons, int inputDim);
    void loadParams(const std::string& weightsFile, const std::string& biasFile);
    void start() override;

protected:
    void processLoop() override;
    void computeRange(int startIdx, int endIdx, const std::vector<double>& input, std::vector<double>& result);
};

#endif