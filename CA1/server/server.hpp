#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "shared_functions.hpp"
#include "tcp_socket.hpp"
#include "udp_socket.hpp"

#include "request_handler.hpp"

#define CLIENT_INFO_ARGS 3


struct ClientInfo {
    int udp_port;
    int socket;
    string username;
    string role;
    struct sockaddr_in client_udp_address;
    bool has_team ;
};


class Server {
private:
    friend class Request_handler;
    int tcp_port;
    vector<ClientInfo> clients; 
    vector<pair<ClientInfo , ClientInfo>> teams;
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
        const std::string& udp_port , struct sockaddr_in udp_address , bool has_team_);

public:
    Server(int tcp_port_);
    void start();
    void run();
    void stop(); 
    void broadcast_message(const std::string& message);
    void unicast_message(const std::string& message, const ClientInfo& client_info);
    bool is_name_unique(string uname);
    
    void make_teames();
    
    int get_tcp_port() {return tcp_port;};
    vector<ClientInfo> get_clients() {return clients;};
};

#endif // SERVER_HPP