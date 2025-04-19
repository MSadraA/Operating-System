#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include "define.hpp"

class Transformer {
public:
    Transformer(int input_fd, int output_fd);
    void run();

private:
    int input_fd;
    int output_fd;

    void process();
    GameRecord parse_record(const string& row);
    float parse_price(const string& price_str);
    int convert_review_to_label(const string& review);
    int extract_review_count(const string& text);
};

#endif
