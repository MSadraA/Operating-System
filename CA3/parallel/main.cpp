#include "input_layer.hpp"
#include "hidden_layer.hpp"
#include "threadpool.hpp"
#include "output_layer.hpp"
#include "result_layer.hpp"
#include "controller.hpp"
#include <iostream>
#include <unistd.h>


int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <hidden_layer_count> <neurons_per_hidden_layer> <images_count>\n";
        return 1;
    }

    int hiddenLayerCount = std::stoi(argv[1]);
    int neuronsPerLayer = std::stoi(argv[2]);
    int imagesCount = std::stoi(argv[3]);

    if (hiddenLayerCount <= 0 || neuronsPerLayer <= 0 || imagesCount <= 0) {
        std::cerr << "Both arguments must be positive integers.\n";
        return 1;
    }

    Controller controller(hiddenLayerCount, neuronsPerLayer, imagesCount);
    controller.run();
}


