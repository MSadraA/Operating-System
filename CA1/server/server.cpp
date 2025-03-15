#include "server.hpp"

Server::Server(int tcp_port_) : tcp_port(tcp_port_), requestHandler(*this), running(false) {
    tcp_socket.create_tcp_server(tcp_port);
    udp_socket.create_udp(UDP_PORT_SERVER);
    add_poll(tcp_socket.get_fd());
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
}


void Server::remove_team(int client_socket) {
    ClientInfo& p1 = find_client_info(find_teammate_by_socket(client_socket));
    ClientInfo& p2 = find_client_info(client_socket);
    p1.has_team = false;
    p2.has_team = false;
    for (auto it = teams.begin(); it != teams.end(); ++it) {
        if (p1.socket == client_socket || p2.socket == client_socket) {
            teams.erase(it); 
            udp_socket.unicast_message("Your teammate has been disconnected!" , p1.client_udp_address);
            return; 
        }
    }
}

void Server::remove_poll(int client_socket) {
    for (auto it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == client_socket) {
            poll_fds.erase(it);
            break;
        }
    }
}

void Server::remove_client(int client_socket) {
    ClientInfo& client = find_client_info(client_socket);
    if(client.has_team)
    {
        if(teams.size() > 0)
            remove_team(client_socket);
    }

    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].socket == client_socket) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
    remove_poll(client_socket);

    // for (auto client : clients){
    //     cout << "CLIENTS : " << client.username << " " << client.has_team << endl;
    // }
}

// Handlers
void Server::handle_client(int client_socket) {
    string input = tcp_socket.receive_message_from_client(client_socket);

    if (input == "") {
        remove_client(client_socket);
        return;
    }
    string req_type;
    string data;

    split_by_delim(input , DELIM , req_type , data);
    if(data == "")
        return;
    requestHandler.handleRequest(client_socket , req_type , data);
}

void Server::check_console_poll(){
    // making console poll
    struct pollfd stdin_poll;
    stdin_poll.fd = STDIN_FILENO;
    stdin_poll.events = POLLIN;
    // checking it
    int stdin_poll_count = poll(&stdin_poll, 1, 0);
    if (stdin_poll_count > 0 && (stdin_poll.revents & POLLIN)) {
        std::string command;
        std::getline(std::cin, command); 
        if (command != "quit" && command != "start") // u cant quit or start while server is running 
        {
            processCommand(command);
        }
    }
}

void Server::run() {
    while (running) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            throw std::runtime_error("Poll error");
            break;
        }

        check_console_poll();
        // Receive connect request from clients
        if (poll_fds[0].revents & POLLIN) {
            if (tcp_socket.check_events()) {
                accept_client();
            }
        }
        // Receive other request from clients
        for (size_t i = 1; i < poll_fds.size(); i++) { 
            if (poll_fds[i].revents & POLLIN) {
                handle_client(poll_fds[i].fd);
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
                teams.push_back({make_pair(client.socket , new_client.socket) , 0}); // init team
                udp_socket.unicast_message("A teammate has been found for you !" , client.client_udp_address);
                udp_socket.unicast_message("A teammate has been found for you !" , new_client.client_udp_address);
            }
        }
    }
       
}

void Server::add_poll(int fd){
    struct pollfd server_poll;
    server_poll.fd = fd;
    server_poll.events = POLLIN;
    poll_fds.push_back(server_poll);
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

// Finders
int Server::find_teammate_by_socket(int client_socket) {
    for (const auto& team : teams) {
        int p1 = team.team_members.first;
        int p2 = team.team_members.second;
        
        if (p1 == client_socket) 
            return p2;
        if (p2 == client_socket) 
            return p1;
    }
    return -1;
}

ClientInfo& Server::find_client_info(int client_socket)
{
    for (auto& client : clients) {
        if (client.socket == client_socket) {
            return client; 
        }
    }
    throw std::runtime_error("Client not found");
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
