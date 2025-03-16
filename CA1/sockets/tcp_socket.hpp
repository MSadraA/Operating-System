#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include "define.hpp"

class Tcp_socket {
private:
    int tcp_socket;
    struct sockaddr_in address;

    // Poll
    struct pollfd poll_fd;
    void make_poll();

public:
    Tcp_socket(); 
    ~Tcp_socket();
    // Init socket
    void create_tcp_client(int server_port);
    void create_tcp_server(int port);

    // Handle request
    void send_massage_to_server(string message);
    void send_message_to_client(int client_fd, const string& message);
    int accept_client(struct sockaddr_in &client_addr);
    string receive_message_from_client(int client_fd);
    string receive_message_from_server();
    string send_and_receive_blocking(const string& message, int timeout_ms);
    void close_socket();

    // geters
    int get_fd() {return tcp_socket;};
    struct sockaddr_in get_address() {return address;};

    bool check_events();
};

#endif
