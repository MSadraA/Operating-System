#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "define.hpp"
#include "tcp_socket.hpp"
#include "udp_socket.hpp"
#include "shared_functions.hpp"
#include "port_manager.hpp"


class Client {
private:
    int server_tcp_port;
    bool running;
    string username;
    string role;
    int udp_port;
    
    Tcp_socket tcp_socket;
    Udp_socket udp_socket;

    string current_problem_id;
    string current_code;

public:
    Client(string uname, string r, int udp_port , int server_tcp_port_);

    // Client commands
    void process_command(const string& command);
    void start();
    void run();
    void stop();
    void help();

    // Receive messages
    void handle_message(string msg);


    // Request
    void sendClientInfo();
    void send_message_to_team(string msg);
    string write_code(string before = "");
    void submit_code();
    void share();
    void show_problem();

    ~Client();
};

#endif // CLIENT_HPP