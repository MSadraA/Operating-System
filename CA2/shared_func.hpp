#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"

string to_string(const GameRecord& record);
GameRecord from_string(const std::string& line);

#endif
