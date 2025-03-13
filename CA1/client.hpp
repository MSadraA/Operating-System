#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "define.hpp"

using namespace std;

class Client {
private:
    int tcp_socket, udp_socket;
    struct sockaddr_in server_tcp_addr, client_udp_addr;
    string username;
    string role;
    void init_tcp();
    void init_udp();

public:
    Client(string uname, string r);
    ~Client();
};

#endif // CLIENT_HPP