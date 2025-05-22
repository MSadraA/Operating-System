#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"
#include <iomanip>

uint32_t flipBytes(uint32_t n);
FILE *openMNISTImageFile(const char *fileName);
void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh);
FILE *openMNISTLabelFile(const char *fileName);
void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh);
MNIST_Image getImage(FILE *imageFile);
MNIST_Label getLabel(FILE *labelFile);
void displayLiveAccuracy(int predicted, int actual, int correctCount, int totalCount);
inline void displayLiveProgress(int row, int predicted, int actual, int correct, int total, const std::string& tag) {
    double accuracy = (total > 0) ? 100.0 * correct / total : 0.0;
    std::cout << "\033[" << row << ";1H";
    std::cout << "\033[K";                
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << tag << "] "
              << "Predicted: " << predicted
              << " | Actual: " << actual
              << " | Correct: " << correct << " / " << total
              << " | Accuracy: " << accuracy << "%" << std::flush;
}

#endif