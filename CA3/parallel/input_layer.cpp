#include "input_layer.hpp"


InputLayer::InputLayer(ThreadPool* pool, const std::string& imagePath, const std::string& labelPath, int totalImages)
    : Layer(pool, 1), currentImage(0), imageCount(totalImages) {
    imageFile = fopen(imagePath.c_str(), "rb");
    if (!imageFile) {
        std::cerr << "Failed to open image file\n";
        exit(1);
    }
    readImageFileHeader(imageFile, &imageHeader);

    labelFile = fopen(labelPath.c_str(), "rb");
    if (!labelFile) {
        std::cerr << "Failed to open label file\n";
        exit(1);
    }
    readLabelFileHeader(labelFile, &labelHeader);
}

InputLayer::~InputLayer() {
    if (imageFile) fclose(imageFile);
    if (labelFile) fclose(labelFile);
}

void InputLayer::start() {
    threadPool->enqueueTask([](void* arg) {
        static_cast<InputLayer*>(arg)->readAndDispatchImage();
    }, this);
}

bool InputLayer::isFinished() {
    return isFinish;
}

void InputLayer::readAndDispatchImage() {
    while (currentImage < imageCount) {
        MNIST_Image img;
        size_t result = fread(&img, sizeof(MNIST_Image), 1, imageFile);
        if (result != 1) {
            std::cerr << "Failed to read image at index " << currentImage << std::endl;
            break;
        }

        MNIST_Label lbl;
        result = fread(&lbl, sizeof(MNIST_Label), 1, labelFile);
        if (result != 1) {
            std::cerr << "Failed to read label at index " << currentImage << std::endl;
            break;
        }

        std::vector<double> raw_input;
        raw_input.reserve(28 * 28);
        for (int i = 0; i < 28 * 28; ++i) {
            raw_input.push_back(static_cast<double>(img.pixel[i]));
        }

        LayerData data;
        data.values = raw_input;
        data.label = lbl;

        nextLayer->enqueueInput(data);

        ++currentImage;
    }

    isFinish = true;
}

