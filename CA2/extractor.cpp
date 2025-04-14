#include "extractor.hpp"
#include "shared_func.hpp"

Extractor::Extractor(const string& input_file_ , int output_fd_) {
    input_file = input_file_;
    output_fd = output_fd_;
}

void Extractor::run() {
    read_csv_write();
}


void Extractor::read_csv_write() {
    ifstream file(input_file);
    if (!file.is_open()) {
        perror("failed to open csv");
        return;
    }
    string line;
    getline(file, line); // skip header

    while (std::getline(file, line)) {
        line += "\n";
        write(output_fd, line.c_str(), line.size());
    }
    close(output_fd);
}
