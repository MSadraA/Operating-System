#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"

using namespace std;

vector<string> splitString(const string& str, char delimiter);
string get_address_as_string(sockaddr_in address);
void split_by_delim(const std::string& input, char delim, std::string& type, std::string& data);
std::string trim(const std::string& str);
std::string format_time(time_t raw_time);
void print(const std::string& text);
void print_error(const std::string& text);
string get_address_as_string(sockaddr_in address);
void read_line(std::string& input);

#endif
