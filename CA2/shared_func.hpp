#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"


string to_string(const GameRecord& record);
GameRecord from_string(const std::string& line);
vector<string> split_line(const string& line);
string trim(const string& str);
void print_record(const GameRecord& record);

#endif