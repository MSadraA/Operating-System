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

