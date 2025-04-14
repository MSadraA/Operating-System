#include "extractor.hpp"
#include "shared_func.hpp"

const unordered_map<string, int> review_score_map = { 
    {"Overwhelmingly Positive", 7}, 
    {"Very Positive", 6}, 
    {"Positive", 5},
    {"Mostly Positive", 4}, 
    {"Mixed", 3}, 
    {"Mostly Negative", 2},
    {"Overwhelmingly Negative", 1}
};

Extractor::Extractor(const string& input_file_, const string& pipe_path_){
    input_file = input_file_;
    pipe_path = pipe_path_;
}

void Extractor::load_csv_file() {
    ifstream file(input_file);
    string line;

    getline(file, line); // skip header

    while (getline(file, line)) {
        vector<string> columns = split_line(line);
        GameRecord record = parse_record(columns);
        data.push_back(record);
    }
}
int Extractor::convert_review_to_label(const string& review) {
    auto it = review_score_map.find(review);
    return (it != review_score_map.end()) ? it->second : 0;
}

float Extractor::parse_price(const string& price_str) {
    string clean = price_str;
    if (!clean.empty() && clean[0] == '$') clean = clean.substr(1);
    return stof(clean);
}

GameRecord Extractor::parse_record(const vector<string>& columns) {
    GameRecord record;

    record.title = columns[TITLE];

    // Parse prices
    float orig_price = parse_price(columns[ORIGINAL_PRICE]);
    float disc_price = parse_price(columns[DISCOUNTED_PRICE]);

    record.original_price = orig_price;
    float scale = disc_price / orig_price;
    record.discount_percent = (scale == 1.0f) ? 1.0f : (1.0f - scale) * 100.0f;

    // Review summaries mapped to int
    record.recent_review_summary = convert_review_to_label(columns[RECENT_REVIEW_SUMMARY]);
    record.all_review_summary = convert_review_to_label(columns[ALL_REVIEW_SUMMARY]);

    // Review numbers
    record.recent_review_number = stoi(columns[RECENT_REVIEW_NUMBER]);
    record.all_review_number = stoi(columns[ALL_REVIEW_NUMBER]);

    return record;
}

vector<string> Extractor::split_line(const string& line) {
    vector<string> columns;
    stringstream ss(line);
    string token;

    while (getline(ss, token, DELIM)) {
        columns.push_back(token);
    }
    return columns;
}

void Extractor::run() {
    load_csv_file();

    int fd = open(pipe_path.c_str(), O_WRONLY);
    if (fd == -1) {
        perror("open pipe failed");
        return;
    }

    for (const GameRecord& record : data) {
        // serialize each record into a string
        string out = to_string(record);
        write(fd, out.c_str(), out.size());
    }
    close(fd);
}
