#ifndef INPUT_LAYER_HPP
#define INPUT_LAYER_HPP

#include "define.hpp"
#include "layer.hpp"
#include "shared_functions.hpp"

class InputLayer : public Layer {
private:
    FILE* imageFile;
    FILE* labelFile;
    int maxImages;
    pthread_t thread;
    sem_t imagePermit;

public:
    InputLayer(ThreadPool* pool, const std::string& imagePath, const std::string& labelPath, int maxImg);
    ~InputLayer();

    void start() override;

protected:
    void processLoop() override;
};

#endif
