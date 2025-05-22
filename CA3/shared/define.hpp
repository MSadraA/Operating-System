#ifndef DEFINE_HPP
#define DEFINE_HPP


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

#include <semaphore.h>



using namespace std;

#define TIME_FOR_MONITORING 1000000 // 1 second

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

typedef struct Output_Node Output_Node;
typedef struct Hidden_Node Hidden_Node;
typedef struct MNIST_ImageFileHeader MNIST_ImageFileHeader;
typedef struct MNIST_LabelFileHeader MNIST_LabelFileHeader;

typedef struct MNIST_Image MNIST_Image;
typedef uint8_t MNIST_Label;


struct Hidden_Node{
    double weights[28*28];
    double bias;
    double output;
};

struct Output_Node{
    double weights[256];
    double bias;
    double output;
};

struct HiddenTask {
    std::vector<double> input;
    int label;
};


/**
 * @brief Data block defining a MNIST image
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_Image{
    uint8_t pixel[28*28];
};

/**
 * @brief Data block defining a MNIST image file header
 * @attention The fields in this structure are not used.
 * What matters is their byte size to move the file pointer
 * to the first image.
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_ImageFileHeader{
    uint32_t magicNumber;
    uint32_t maxImages;
    uint32_t imgWidth;
    uint32_t imgHeight;
};

/**
 * @brief Data block defining a MNIST label file header
 * @attention The fields in this structure are not used.
 * What matters is their byte size to move the file pointer
 * to the first label.
 * @see http://yann.lecun.com/exdb/mnist/ for details
 */

struct MNIST_LabelFileHeader{
    uint32_t magicNumber;
    uint32_t maxImages;
};



enum class ThreadPriorityLevel {
    LOW = 0,       // InputLayer
    MEDIUM = 1,    // HiddenLayer
    HIGH = 2       // OutputLayer
};
    
struct Task {
    void (*function)(void*);
    void* arg;               
};

struct LayerData {
    std::vector<double> values;
    int label;
};


#endif