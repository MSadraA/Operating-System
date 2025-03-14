#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "shared_functions.hpp"
#include "tcp_socket.hpp"
#include "request_handler.hpp"
#include "udp_socket.hpp"

#define CLIENT_INFO_ARGS 3


struct ClientInfo {
    int udp_port;
    int socket;
    std::string username;
    std::string role;
    struct sockaddr_in client_udp_address;
};

class Server {
private:
    int tcp_port;
    std::vector<ClientInfo> clients; 
    std::map<int, int> teams;
    Request_handler requestHandler;
    
    bool running; 

    Tcp_socket tcp_socket;
    Udp_socket udp_socket;

    void init_tcp();
    void init_udp();
    void accept_client();
    void handle_client(int client_socket);
    void remove_client(int client_socket);
    void register_client(int client_socket, const std::string& username, const std::string& role,
        const std::string& udp_port , struct sockaddr_in udp_address);

public:
    Server(int tcp_port_);
    void start();
    void run();
    void stop(); 
    void broadcast_message(const std::string& message);
    void unicast_message(const std::string& message, const ClientInfo& client_info);
    bool is_name_unique(string uname);

    int get_tcp_port() {return tcp_port;};
    vector<ClientInfo> get_clients() {return clients;};
};

#endif // SERVER_HPP