#include "shared_func.hpp"

string to_string(const GameRecord& record) { 
    std::ostringstream oss; 
    oss << record.title << ","; 
    oss << record.original_price << ","; 
    oss << record.discount_percent << ","; 
    oss << record.recent_review_summary << ","; 
    oss << record.all_review_summary << ","; 
    oss << record.recent_review_number << ","; 
    oss << record.all_review_number;
    return oss.str(); 
}

GameRecord from_string(const std::string& line) { 
    std::stringstream ss(line); 
    std::string token; 
    std::vector<std::string> fields; 
    while (std::getline(ss, token, ',')) { 
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
