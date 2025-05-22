#ifndef RESULT_LAYER_HPP
#define RESULT_LAYER_HPP

#include "controller.hpp"
#include "layer.hpp"
#include "define.hpp"
#include "shared_functions.hpp"

class Controller;

class ResultLayer : public Layer {
    private:
    int correctCount = 0;
    int totalCount = 0;
    Controller *controller;

    
    public:
    ResultLayer(Controller *controller);
    ~ResultLayer();
    void start() override;
    static void* processLoop(void* arg);
};

#endif