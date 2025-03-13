#include "request_handler.hpp"

Request_handler::Request_handler(map<int, int>& teamsRef) : teams(teamsRef) {
    handlers["connect"] = [&](int client_fd, string data) { handleConnect(client_fd, data); };
}

void Request_handler::handleRequest(int client_fd, string request_type, string data) {
    if (handlers.find(request_type) != handlers.end()) {
        handlers[request_type](client_fd, data);
    } else {
        throw runtime_error(REQUEST_ERROR_INVALID);
    }
}

void Request_handler::handleConnect(int client_fd, string data) {
}

