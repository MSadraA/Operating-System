#include "shared_func.hpp"
#include "define.hpp"


string to_string(const GameRecord& record) { 
    std::ostringstream oss; 
    oss << record.title << DELIM; 
    oss << record.original_price << DELIM; 
    oss << record.discount_percent << DELIM; 
    oss << record.recent_review_summary << DELIM; 
    oss << record.all_review_summary << DELIM; 
    oss << record.recent_review_number << DELIM; 
    oss << record.all_review_number;
    return oss.str(); 
}

GameRecord from_string(const std::string& line) { 
    std::stringstream ss(line); 
    std::string token; 
    std::vector<std::string> fields; 
    while (std::getline(ss, token, DELIM)) { 
        fields.push_back(token); 
    } 
    GameRecord record; 
    record.title = fields[0]; 
    record.original_price = stof(fields[1]); 
    record.discount_percent = stof(fields[2]); 
    record.recent_review_summary = stoi(fields[3]); 
    record.all_review_summary = stoi(fields[4]); 
    record.recent_review_number = stoi(fields[5]); 
    record.all_review_number = stoi(fields[6]); 
    return record;
}

vector<string> split_line(const string& line) {
    vector<string> columns;
    stringstream ss(line);
    string token;

    while (getline(ss, token, DELIM)) {
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