#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include "define.hpp"

class Extractor {
public:
    Extractor(const string& input_file_ , int output_fd_);
    void run(); 

private:
    string input_file;
    int output_fd;
    void read_csv_write(); 
};

#endif
