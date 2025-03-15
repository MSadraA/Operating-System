#include "shared_functions.hpp"


vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

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

