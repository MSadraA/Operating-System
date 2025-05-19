#include "input_layer.hpp"
#include "hidden_layer.hpp"
#include "threadpool.hpp"
#include <iostream>
#include <unistd.h>

int main() {
ThreadPool pool(8, 8, ThreadPriorityLevel::MEDIUM);

// 2. ایجاد لایه پنهان
HiddenLayer hidden(&pool, 256 , 28 * 28); // مثلاً 256 نورون
hidden.loadParams(HIDDEN_WEIGHTS_FILE, HIDDEN_BIASES_FILE);

// 3. ایجاد لایه ورودی و اتصالش به لایه پنهان
InputLayer input(&pool, MNIST_TESTING_SET_IMAGE_FILE_NAME, MNIST_TESTING_SET_LABEL_FILE_NAME, 2); // فقط 2 تصویر
input.setNextLayer(&hidden);
hidden.setPrevSemaphore(input.getPermitSemaphore());

// 4. راه‌اندازی لایه‌ها
hidden.start(); // ترد داخلی خودش
input.start();  // ترد داخلی خودش

return 0;
}

