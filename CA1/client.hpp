#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "define.hpp"

class Client {
private:
    int server_tcp_port;
    int tcp_socket;
    int udp_socket;
    struct sockaddr_in client_addr;
    string username;
    string role;
    int udp_port;
    void init_tcp();
    void init_udp();

public:
    Client(string uname, string r, int udp_port , int server_tcp_port_);
    void sendClientInfo();
    string receiveUDPMessage();
    int getSocket() const { return tcp_socket; }
    string getUsername() const { return username; }
    ~Client();
};

#endif // CLIENT_HPP