#include "shared_func.hpp"
#include "define.hpp"


string game_record_to_string(const GameRecord& record) { 
    ostringstream oss; 
    oss << record.title << DELIM; 
    oss << record.original_price << DELIM; 
    oss << record.discount_percent << DELIM; 
    oss << record.recent_review_summary << DELIM; 
    oss << record.all_review_summary << DELIM; 
    oss << record.recent_review_number << DELIM; 
    oss << record.all_review_number;
    return oss.str(); 
}

GameRecord string_to_game_record(const string& line) { 
    stringstream ss(line); 
    string token; 
    vector<string> fields; 
    while (getline(ss, token, DELIM)) { 
        fields.push_back(token); 
    } 
    GameRecord record; 
    record.title = fields[0]; 
    record.original_price = stof(fields[1]); 
    record.discount_percent = stof(fields[2]); 
    record.recent_review_summary = stof(fields[3]); 
    record.all_review_summary = stof(fields[4]); 
    record.recent_review_number = stof(fields[5]); 
    record.all_review_number = stof(fields[6]); 
    return record;
}

vector<string> split_line(const string& line , char delim) {
    vector<string> columns;
    stringstream ss(line);
    string token;

    while (getline(ss, token, delim)) {
        columns.push_back(trim(token));
    }
    return columns;
}


string trim(const string& str) {
    size_t start = 0;
    size_t end = str.length();

    while (start < end && (isspace(static_cast<unsigned char>(str[start])) || str[start] == '"')) 
        ++start;
    while (end > start && (isspace(static_cast<unsigned char>(str[end - 1])) || str[end - 1] == '"')) 
        --end;
        
    return str.substr(start, end - start);
}

void print_record(const GameRecord& record) {
    cout << record.title << " | "
         << record.original_price << " | "
         << record.discount_percent << " | "
         << record.recent_review_summary << " | "
         << record.all_review_summary << " | "
         << record.recent_review_number << " | "
         << record.all_review_number << endl;
}

string worker_to_string(const WorkerInfo& worker) {
    ostringstream oss;
    oss << worker.id << DELIM;
    oss << worker.pid;
    return oss.str();
}

WorkerInfo string_to_worker(const string& line) {
    stringstream ss(line);
    string token;
    vector<string> fields;
    while (getline(ss, token, DELIM)) {
        fields.push_back(token);
    }
    WorkerInfo worker;
    worker.id = stoi(fields[0]);
    worker.pid = stoi(fields[1]);
    return worker;
}

void print_worker(const WorkerInfo& worker){
    cout << worker.id << " | "
         << worker.pid << endl;
}

string score_to_string(const Score& score) {
    ostringstream oss;
    oss << score.title << DELIM;
    oss << score.score;
    return oss.str();
}
Score string_to_score(const string& line) {
    stringstream ss(line);
    string token;
    vector<string> fields;
    while (getline(ss, token, DELIM)) {
        fields.push_back(token);
    }
    Score score;
    score.title = fields[0];
    score.score = stof(fields[1]);
    return score;
}

void sort_scores(vector<Score>& scores) {
    sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
        return a.score > b.score;
    });
}
void print_scores(const vector<Score>& scores) {
    for (const auto& score : scores) {
        cout << score.title << " , " << score.score << endl;
    }
    cout << endl;
}