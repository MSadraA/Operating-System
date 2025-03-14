#include "server.hpp"

Server::Server(int tcp_port_) : tcp_port(tcp_port_), requestHandler(*this), running(false) {
    tcp_socket.create_tcp_server(tcp_port);
    udp_socket.create_udp(UDP_PORT_SERVER);
}


void Server::accept_client() {
    struct sockaddr_in address;
    int client_socket = tcp_socket.accept_client(address);
    handle_client(client_socket);
}

void Server::register_client(int client_socket, const std::string& username, const std::string& role,
    const std::string& udp_port , struct sockaddr_in udp_address , bool has_team_) {
    clients.push_back({stoi(udp_port) , client_socket, username, role, udp_address , has_team_});
    make_teames();
    // udp_socket.unicast_message("hello!" , udp_address);
    // cout << "New client : " << username << " Port : " << udp_port << " int port : " << stoi(udp_port) << endl;
}

void Server::remove_client(int client_socket) {
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].socket == client_socket) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

void Server::handle_client(int client_socket) {
    string client_info = tcp_socket.receive_message(client_socket);
    if (client_info == "") {
        remove_client(client_socket);
        return;
    }
    
    requestHandler.handleRequest(client_socket , REG , client_info);
}

void Server::run() {
    struct pollfd fds[2];

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN; 

    fds[1].fd = tcp_socket.get_fd();
    fds[1].events = POLLIN;

    while (running) {
        int poll_count = poll(fds, 2, -1);
        if (poll_count < 0) {
            throw ("poll error");
            break;
        }
        
        if (fds[0].revents & POLLIN) {
            std::string command;
            std::getline(std::cin, command); 
            if (command != "quit" && command != "start") {
                processCommand(command);
            }
        }

        if (fds[1].revents & POLLIN) {
            if (tcp_socket.check_events()) {
                accept_client();
            }
        }
    }
}

bool Server::is_name_unique(string uname){
    for (auto clinet : clients)
    {
        if(uname == clinet.username)
            return false;
    }
    return true;
}

void Server::make_teames(){
    ClientInfo& new_client = clients.back();
    string role = new_client.role;
    for(auto& client : clients){
        if(!client.has_team){
            if((role == CODER && client.role == NAVIGATOR) ||(role == NAVIGATOR && client.role == CODER))
            {
                client.has_team = true;
                new_client.has_team = true;
                teams.push_back(make_pair(client , new_client));
                udp_socket.unicast_message("A teammate has been found for you !" , client.client_udp_address);
                udp_socket.unicast_message("A teammate has been found for you !" , new_client.client_udp_address);
            }
        }
    }
       
}

// Server commands
void Server::help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help    - Show this help message" << std::endl;
    std::cout << "  status  - Show server status" << std::endl;
    std::cout << "  start   - Start the server" << std::endl;
    std::cout << "  quit    - Quit the server" << std::endl;
    std::cout << "  stop    - Stop the server" << std::endl;
}

void Server::status() {
    if (running) {
        std::cout << "Server is running on port " << "." << std::endl;
    } else {
        std::cout << "Server is not running." << std::endl;
    }
}

void Server::start() {
    running = true;
    run();
}

void Server::stop() {
    running = false;
}

void Server::quit() {
    if (!running) {
        return;
    }
    running = false;  
    tcp_socket.close_socket();
    udp_socket.close_socket();

    for (const auto& client : clients) {
        close(client.socket); 
    }
    clients.clear();
    teams.clear();
}
void Server::processCommand(const std::string& command) {
    if (command == "help") {
        help();
    } else if (command == "status") {
        status();
    } else if (command == "start") {
        start();
    } else if (command == "quit") {
        quit();
    } else if (command == "stop") {
        stop();
    }
        else {
        std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error("Usage: ./server <port>");
        }

        int port = std::atoi(argv[1]);
        if (port <= 0) {
            throw std::invalid_argument("Invalid port number.");
        }

        Server server(port);
        std::string command;

        while (true) {
            std::cout << "> ";
            std::getline(std::cin, command);

            if (command == "quit") {
                server.processCommand(command);
                break;
            }
            server.processCommand(command);
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } 
}
