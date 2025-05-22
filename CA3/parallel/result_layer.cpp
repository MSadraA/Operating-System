#include "result_layer.hpp"

#include <iomanip>


ResultLayer::ResultLayer(Controller* controller)
    : Layer(nullptr, 1), controller(controller) {}
ResultLayer::~ResultLayer() {
    sem_destroy(&dataAvailable);

    pthread_mutex_lock(&queueMutex);
    while (!inputQueue.empty()) {
        inputQueue.pop();
    }
    pthread_mutex_unlock(&queueMutex);
}


void ResultLayer::start() {
    pthread_t thread;
    pthread_create(&thread, nullptr, ResultLayer::processLoop, this);
    pthread_detach(thread);
}

void* ResultLayer::processLoop(void* arg) {
    auto* layer = static_cast<ResultLayer*>(arg);

    // Clear screen and move cursor to top-left
    std::cout << "\033[2J\033[H";  // Clear screen and home cursor

    while (true) {
        sem_wait(&layer->dataAvailable);

        pthread_mutex_lock(&layer->queueMutex);
        if (layer->inputQueue.empty()) {
            pthread_mutex_unlock(&layer->queueMutex);
            continue;
        }
        LayerData data = layer->inputQueue.front();
        layer->inputQueue.pop();
        pthread_mutex_unlock(&layer->queueMutex);

        const auto& outputs = data.values;

        double maxVal = -1.0;
        int predicted = -1;
        for (size_t i = 0; i < outputs.size(); ++i) {
            if (outputs[i] > maxVal) {
                maxVal = outputs[i];
                predicted = static_cast<int>(i);
            }
        }

        layer->totalCount++;
        if (predicted == data.label) {
            layer->correctCount++;
        }

        displayLiveProgress(1, predicted, data.label, layer->correctCount, layer->totalCount, "Parallel");

        
        // usleep(5000);
        if (layer->totalCount >= layer->controller->getImageCount()) {
            layer->controller->notifyIfFinished();
            break; 
        }
    }

    return nullptr;
}
