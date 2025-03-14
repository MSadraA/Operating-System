#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include "define.hpp"
#define TMIE_OUT 500 //ms

class Udp_socket {
private:
    int udp_socket;

    struct sockaddr_in address;

    vector<struct pollfd> poll_fds;

public:
    Udp_socket(); 
    ~Udp_socket();

    void create_udp(int port);
    void unicast_message(const std::string& message, sockaddr_in& address);
    string receive_message();
    bool check_events();
    void close_socket();

    struct sockaddr_in get_bound_address();
};

#endif
