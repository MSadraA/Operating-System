#include "input_layer.hpp"


InputLayer::InputLayer(ThreadPool* pool, const std::string& imagePath, const std::string& labelPath, int maxImg)
    : Layer(pool), maxImages(maxImg) {
    imageFile = openMNISTImageFile(imagePath.c_str());
    labelFile = openMNISTLabelFile(labelPath.c_str());
    sem_init(&imagePermit, 0, 1);
}

InputLayer::~InputLayer() {
    fclose(imageFile);
    fclose(labelFile);
    sem_destroy(&imagePermit);
}

void InputLayer::start() {
    pthread_create(&thread, nullptr, [](void* arg) -> void* {
        static_cast<InputLayer*>(arg)->processLoop();
        return nullptr;
    }, this);
}

void InputLayer::processLoop() {
    for (int i = 0; i < maxImages; ++i) {
        sem_wait(&imagePermit);

        MNIST_Image img = getImage(imageFile);
        MNIST_Label lbl = getLabel(labelFile);

        std::vector<double> pixels(28 * 28);
        for (int j = 0; j < 28 * 28; ++j)
            pixels[j] = static_cast<double>(img.pixel[j]) / 255.0;

        LayerData data;
        data.values = std::move(pixels);
        data.label = lbl;
        data.backSemaphore = &imagePermit;

        if (nextLayer)
            nextLayer->enqueueInput(data);
    }
}