#include "shared_functions.hpp"


std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(str.substr(start)); 
    return tokens;
}

string get_address_as_string(sockaddr_in address) {
    
    char ip[INET_ADDRSTRLEN]; 
    inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);

    return string(ip);
}

void split_by_delim(const std::string& input, char delim, std::string& type, std::string& data) {
    size_t pos = input.find(delim); 

    if (pos != std::string::npos) { 
        type = input.substr(0, pos);  
        data = input.substr(pos + 1); 
    } else { 
        type = input;
        data = ""; 
    }
}

std::string trim(const std::string& str) {
    std::string s = str;

    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

std::string format_time(time_t raw_time) {
    std::tm* time_info = std::localtime(&raw_time);
    char buffer[9]; // HH:MM:SS + null-terminator
    std::sprintf(buffer, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    return std::string(buffer);
}

void print(const std::string& text) {
    write(STDOUT_FILENO, text.c_str(), text.size());
}

void print_error(const std::string& text) {
    write(STDERR_FILENO, text.c_str(), text.size());
}

void read_line(std::string& input) {
    input.clear();
    char buffer[256];
    ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        input = buffer;

        if (!input.empty() && input.back() == '\n') {
            input.pop_back();
        }
    }
}

