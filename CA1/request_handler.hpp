#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "define.hpp"
#include <map>
#include <functional>
#include <string>
#include <stdexcept>

using namespace std;

class Request_handler {
private:
    map<string, function<void(int, string)>> handlers;
    map<int, int>& teams; 

public:
    Request_handler(map<int, int>& teamsRef);
    void handleRequest(int client_fd, string request_type, string data);

private:
    void handleConnect(int client_fd, string data);
};

#endif // REQUEST_HANDLER_HPP