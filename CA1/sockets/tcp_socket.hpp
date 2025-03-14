#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "define.hpp"
#define TMIE_OUT 500 //ms

class Tcp_socket {
private:
    int tcp_socket;
    struct sockaddr_in address;
    vector<struct pollfd> poll_fds;

public:
    Tcp_socket(); 
    ~Tcp_socket();

    void create_tcp_client(int server_port);
    void create_tcp_server(int port);
    void send_massage(string message);
    int accept_client(struct sockaddr_in &client_addr);
    string receive_message(int client_fd);
    void remove_poll(int client_fd);
    bool check_events();
    void close_socket();

    struct sockaddr_in get_address() {return address;};
};

#endif
