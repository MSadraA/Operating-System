#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"

uint32_t flipBytes(uint32_t n);
FILE *openMNISTImageFile(const char *fileName);
void readImageFileHeader(FILE *imageFile, MNIST_ImageFileHeader *ifh);
FILE *openMNISTLabelFile(const char *fileName);
void readLabelFileHeader(FILE *imageFile, MNIST_LabelFileHeader *lfh);
MNIST_Image getImage(FILE *imageFile);
MNIST_Label getLabel(FILE *labelFile);



#endif