#include "transformer.hpp"
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


Transformer::Transformer(int input_fd_, int output_fd_)
    : input_fd(input_fd_), output_fd(output_fd_) {}


void Transformer::run() {
    process();
}

void Transformer::process() {
    char buffer[BUFF_SIZE];
    string temp;
    ssize_t bytes_read;

    while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) {
        temp.append(buffer, bytes_read);
        size_t pos;

        while ((pos = temp.find('\n')) != string::npos) {
            string line = temp.substr(0, pos);
            temp.erase(0, pos + 1);
            GameRecord record = parse_record(line);
            string out = game_record_to_string(record) + "\n";
            write(output_fd, out.c_str(), out.size());
        }
    }

    close(input_fd);
    close(output_fd);
}

GameRecord Transformer::parse_record(const string& row) {
    vector<string> fields = split_line(row);

    GameRecord record;

    record.title = fields[TITLE];

    // Parse prices
    float orig_price = parse_price(fields[ORIGINAL_PRICE]);
    float disc_price = parse_price(fields[DISCOUNTED_PRICE]);

    record.original_price = orig_price;
   
    float discount_percent = ((orig_price - disc_price) / orig_price) * 100.0f;
    record.discount_percent = (discount_percent == 0) ? 1:discount_percent;

    // Review summaries mapped to int
    record.recent_review_summary = convert_review_to_label(fields[RECENT_REVIEW_SUMMARY]);
    record.all_review_summary = convert_review_to_label(fields[ALL_REVIEW_SUMMARY]);

    // Review numbers
    record.recent_review_number = extract_review_count(fields[RECENT_REVIEW_NUMBER]);
    record.all_review_number = extract_review_count(fields[ALL_REVIEW_NUMBER]);

    // print_record(record);

    return record;
}

float Transformer::parse_price(const string& s) {
    string cleaned = s;
    // delete $
    if (!cleaned.empty() && cleaned[0] == '$') {
        cleaned = cleaned.substr(1); 
    }
    try {
        return stof(cleaned);
    } catch (const exception& e) {
        cerr << "[parse_price] invalid input: '" << s << "'\n";
        return 0.0f;
    }
}

int Transformer::convert_review_to_label(const string& review) {
    auto it = review_score_map.find(review);
    return (it != review_score_map.end()) ? it->second : 0;
}

int Transformer::extract_review_count(const string& text) {
    regex pattern(R"((\d+)\s+user\s+reviews)");
    smatch match;

    if (regex_search(text, match, pattern)) {
        return stof(match[1]);
    }

    return 0;
}