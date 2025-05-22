#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

void clearTerminalWithHeaders() {
    std::cout << "\033[2J\033[H"; 
    std::cout << "\033[1;1H" << "[Parallel Progress]";
    std::cout << "\033[3;1H" << "[Serial Progress]";
    std::cout.flush();
}

double runAndMeasure(const std::string& command) {
    auto start = std::chrono::high_resolution_clock::now();
    int status = system(command.c_str());
    auto end = std::chrono::high_resolution_clock::now();
    if (status != 0) {
        std::cerr << "Command failed: " << command << std::endl;
    }
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <hidden_layer_count> <neurons_per_layer> <image_count>" << std::endl;
        return 1;
    }

    std::string hiddenLayers = argv[1];
    std::string neurons = argv[2];
    std::string images = argv[3];

    clearTerminalWithHeaders();

    std::string parallelCmd = "./bin/parallel_exec " + hiddenLayers + " " + neurons + " " + images;
    std::string serialCmd = "./bin/serial_exec " + images;

    double parallelTime = 0, serialTime = 0;

    std::thread parallelThread([&](){
        parallelTime = runAndMeasure(parallelCmd);
    });

    std::thread serialThread([&](){
        serialTime = runAndMeasure(serialCmd);
    });

    parallelThread.join();
    serialThread.join();

    std::cout << "\n\n\033[5;1H";
    std::cout << "⏱️  Parallel Time: " << std::fixed << std::setprecision(3) << parallelTime << " seconds" << std::endl;
    std::cout << "⏱️  Serial Time:   " << std::fixed << std::setprecision(3) << serialTime   << " seconds" << std::endl;

    if (serialTime > 0) {
        double speedup = serialTime / parallelTime;
        std::cout << "🚀 Speedup:        " << std::fixed << std::setprecision(2) << speedup << "x\n";
    }

    return 0;
}
