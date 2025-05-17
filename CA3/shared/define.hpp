#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <queue>
#include <pthread.h>
#include <atomic>
#include <unistd.h>


using namespace std;

#define MNIST_TESTING_SET_IMAGE_FILE_NAME "data/t10k-images-idx3-ubyte"
#define MNIST_TESTING_SET_LABEL_FILE_NAME "data/t10k-labels-idx1-ubyte"

#define HIDDEN_WEIGHTS_FILE "net_params/hidden_weights.txt"
#define HIDDEN_BIASES_FILE "net_params/hidden_biases.txt"
#define OUTPUT_WEIGHTS_FILE "net_params/out_weights.txt"
#define OUTPUT_BIASES_FILE "net_params/out_biases.txt"

#define NUMBER_OF_INPUT_CELLS  784  // use 28*28 input cells (= number of pixels per MNIST image)
#define NUMBER_OF_HIDDEN_CELLS 256  // use 256 hidden cells in one hidden layer
#define NUMBER_OF_OUTPUT_CELLS 10   // use 10 output cells to model 10 digits (0-9)

#define MNIST_MAX_TESTING_IMAGES 10000 // number of images+labels in the TEST file/s
#define MNIST_IMG_WIDTH          28    // image width in pixel
#define MNIST_IMG_HEIGHT         28    // image height in pixel

#define SLEEP_TIME 10000 // sleep time in microseconds for the worker threads

enum class TaskPriority {
        LOW,
        MEDIUM,
        HIGH
    };
    
struct PrioritizedTask {
    void (*function)(void*);
    void* arg;               
    TaskPriority priority;

    bool operator<(const PrioritizedTask& other) const {
        return static_cast<int>(priority) < static_cast<int>(other.priority);
    }
};
