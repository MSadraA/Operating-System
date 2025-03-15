#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "define.hpp"
#include <map>
#include <functional>
#include <stdexcept>

class Server;

class Request_handler {
private:
    Server& server;
    map<string, function<void(int, string)>> handlers;

public:
    Request_handler(Server& srv);
    void handleRequest(int client_fd, string request_type, string data);

private:
    void handle_register(int client_fd, string data);
    void handle_message(int client_fd, string data);
};

#endif // REQUEST_HANDLER_HPP