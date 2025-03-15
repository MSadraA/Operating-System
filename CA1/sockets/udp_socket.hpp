#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include "define.hpp"

class Udp_socket {
private:
    int udp_socket;
    struct sockaddr_in address;

   struct pollfd poll_fd;

public:
    Udp_socket(); 
    ~Udp_socket();
    void close_socket();
    void create_udp(int port);

    // Request handler
    void unicast_message(const std::string& message, sockaddr_in& address);
    string receive_message();

    // Poll
    void make_poll();
    bool check_events();

    // getters
    int get_fd() {return udp_socket;};
    struct sockaddr_in get_bound_address();
};

#endif
