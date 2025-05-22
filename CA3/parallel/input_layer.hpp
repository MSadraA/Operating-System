// input_layer.hpp
#ifndef INPUT_LAYER_HPP
#define INPUT_LAYER_HPP

#include "layer.hpp"
#include "shared_functions.hpp"
#include <string>
#include <fstream>

class InputLayer : public Layer {
public:
    InputLayer(ThreadPool* pool, const std::string& imagePath, const std::string& labelPath, int totalImages);
    ~InputLayer();

    void start() override;
    void readAndDispatchImage();
    bool isFinished();

private:
    int currentImage;
    int imageCount;
    bool isFinish = false;

    FILE* imageFile = nullptr;
    FILE* labelFile = nullptr;
    MNIST_ImageFileHeader imageHeader;
    MNIST_LabelFileHeader labelHeader;
};


#endif
