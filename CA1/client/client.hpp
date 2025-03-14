#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "define.hpp"
#include "tcp_socket.hpp"
#include "udp_socket.hpp"
#include "shared_functions.hpp"
#include "port_manager.hpp"


class Client {
private:
    int server_tcp_port;

    bool running;
    
    string username;
    string role;
    int udp_port;

    Tcp_socket tcp_socket;
    Udp_socket udp_socket;

    void init_udp();

public:
    Client(string uname, string r, int udp_port , int server_tcp_port_);
    void start();
    void run();
    void stop();
    void sendClientInfo();
    ~Client();
};

#endif // CLIENT_HPP