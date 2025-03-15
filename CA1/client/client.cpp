#include "client.hpp"


Client::Client(string uname, string r, int port , 
int server_tcp_port_): username(uname), role(r), udp_port(port) {
    server_tcp_port = server_tcp_port_;
    tcp_socket.create_tcp_client(server_tcp_port);
    udp_socket.create_udp(port);
    start();
}

void Client::start()
{
    sendClientInfo();
    running = true;
    run();
}

void Client::run() {
    struct pollfd fds[2];
    // Console
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    //tcp socket
    fds[1].fd = tcp_socket.get_fd();
    fds[1].events = POLLIN;
   
    while (running) {
        int poll_count = poll(fds, 2, TIME_OUT);  
        if (poll_count < 0) {
            std::cerr << "Error in poll: " << strerror(errno) << std::endl;
            throw std::runtime_error("poll error");
        }
        // Console commands 
        if (fds[0].revents & POLLIN) {
            std::string command;
            std::getline(std::cin, command);
            std::cout << "Received command: " << command << std::endl;  // لاگ برای اشکال‌زدایی
            if (command == "quit") {
                stop();
                break;
            } else {
                process_command(command);
            }
        }

        // TCP request
        if (fds[1].revents & POLLIN) {
            std::string message = tcp_socket.receive_message_from_server();
            if (!message.empty()) {
                std::cout << "TCP message: " << message << std::endl;
            }
        }

        //UDP request
        if ((udp_socket.check_events() == true) && POLLIN) {
                std::string message = udp_socket.receive_message();
            if (!message.empty()) {
                std::cout << "UDP message: " << message << std::endl;
            }
        }
    }
}

void Client::stop()
{
    running = false;
}

void Client::sendClientInfo() {
    string type = REG + DELIM;
    string client_info = type + username + DELIM + role + DELIM + to_string(udp_port) 
    + DELIM + get_address_as_string(udp_socket.get_bound_address());

    tcp_socket.send_massage_to_server(client_info);
}


void Client::send_message_to_team(string msg){
    string type = MSG + DELIM;
    string msg_snd = type + msg;
    
    tcp_socket.send_massage_to_server(msg_snd);
}

Client::~Client() {
    stop();
    Port_manager::getInstance()->release_port(udp_port);
}

void Client::help(){
    std::cout << "Available commands:\n";
    std::cout << "  message <text> - Send a message to the server\n";
    std::cout << "  quit - Exit the client\n";
}

void Client::process_command(const string& command) {
    std::string trimmed_command = trim(command); 

    if (trimmed_command == "help") {
        help();
    } 
    else if (trimmed_command.rfind("message ", 0) == 0) { 
        std::string message = trimmed_command.substr(8);
        send_message_to_team(message);
    } 
    else {
        std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./client <username> <role> <server_tcp_port>" << std::endl;
        return 1;
    }

    string username = argv[1];
    string role = argv[2];
    int server_tcp_port = stoi(argv[3]);

    int udp_port = Port_manager::getInstance()->generate_unique_port();

    Client client(username, role, udp_port, server_tcp_port);
    client.start();
    return 0;
}


