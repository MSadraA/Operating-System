#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "shared_functions.hpp"
#include "tcp_socket.hpp"
#include "udp_socket.hpp"

#include "request_handler.hpp"


struct ClientInfo {
    int udp_port;
    int socket;
    string username;
    string role;
    struct sockaddr_in client_udp_address;
    bool has_team;
};

struct Team{
    pair<int , int> team_members;
    int score;
};

class Server {
private:
    friend class Request_handler;
    int tcp_port;
    vector<ClientInfo> clients; 
    vector<Team> teams;
    Request_handler requestHandler;
    bool running; 
    Tcp_socket tcp_socket;
    Udp_socket udp_socket;
    vector<struct pollfd> poll_fds;

    // Handeling
    void accept_client();
    void handle_client(int client_socket);
    void register_client(int client_socket, const std::string& username, const std::string& role,
        const std::string& udp_port , struct sockaddr_in udp_address , bool has_team_);

    // Poll
    void add_poll(int fd);
    void check_console_poll(struct pollfd & stdin_poll);
        
    public:
    Server(int tcp_port_);
    
    void broadcast_message(const std::string& message);
    void unicast_message(const std::string& message, const ClientInfo& client_info);
    bool is_name_unique(string uname);
    void make_teames();

    // Finders
    int find_teammate_by_socket(int socket);
    ClientInfo& find_teammate_client_info(int socket);
    ClientInfo& find_client_info(int socket);

    // Removers
    void remove_client(int client_socket);
    void remove_team(int client_socket);
    void remove_poll(int client_socket);

    // Geters
    int get_tcp_port() {return tcp_port;};
    vector<ClientInfo> get_clients() {return clients;};
        
    // Server commands
    void processCommand(const std::string& command);
    void help();
    void status();
    void start();
    void quit();
    void run();
    void stop(); 
};



#endif // SERVER_HPP