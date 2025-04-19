#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include "define.hpp"


string game_record_to_string(const GameRecord& record);
GameRecord string_to_game_record(const string& line);
vector<string> split_line(const string& line , char delim = DELIM);
string trim(const string& str);
void print_record(const GameRecord& record);

string worker_to_string(const WorkerInfo& worker);
WorkerInfo string_to_worker(const string& line);
void print_worker(const WorkerInfo& worker);

string score_to_string(const Score& score);
Score string_to_score(const string& line);

void sort_scores(vector<Score>& scores);
void print_scores(const vector<Score>& scores);

#endif