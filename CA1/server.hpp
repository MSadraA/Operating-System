#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "request_handler.hpp"


struct ClientInfo {
    int socket;
    std::string username;
    std::string role;
    struct sockaddr_in addr;
};

class Server {
private:
    int tcp_port;
    int tcp_socket;
    int udp_socket;
    struct sockaddr_in tcp_address;
    struct sockaddr_in udp_address;
    std::vector<ClientInfo> clients; 
    std::map<int, int> teams;
    Request_handler requestHandler;
    std::vector<struct pollfd> poll_fds;
    bool running; 

    void init_tcp();
    void init_udp();
    void accept_client();
    void handle_client(int client_socket);
    void remove_client(int client_socket);
    void register_client(int client_socket, const std::string& username, const std::string& role, struct sockaddr_in addr);

public:
    Server(int tcp_port_);
    int get_tcp_port() {return tcp_port;};
    void start();
    void run();
    void stop(); 
    void message_broadcast(const std::string& message);
};

#endif // SERVER_HPP