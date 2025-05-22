#ifndef OUTPUT_LAYER_HPP
#define OUTPUT_LAYER_HPP

#include "layer.hpp"
#include "define.hpp"
#include "shared_functions.hpp"
#include "threadpool.hpp"

class OutputLayer : public Layer {
private:
    std::vector<Output_Node> output_nodes;
    int correctCount;
    int totalCount;

    static void* processLoop(void* arg);
    static void computeTask(void* arg);
    void allocateOutputParameters();

public:
    OutputLayer(ThreadPool* pool, int outputCount ,int threadCount);
    ~OutputLayer();

    void start() override;
};

#endif