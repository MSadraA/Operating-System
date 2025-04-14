#ifndef EXTRACTOR_HPP
#define EXTRACTOR_HPP

#include "define.hpp"

class Extractor{
    public :
    Extractor(const string& inputfile_, const string& pipepath_);
    void run();

    private:
    vector<GameRecord> data;
    string input_file;
    string pipe_path;
    
    void load_csv_file();

    vector<string> split_line(const string& line);
    GameRecord parse_record(const vector<string>& columns);
    float parse_price(const string& price_str);
    int convert_review_to_label(const string& review);
};

#endif



