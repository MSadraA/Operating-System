#include "server.hpp"

Server::Server(int tcp_port_) : requestHandler(teams), running(true){
    tcp_port = tcp_port_;
}

void Server::start() {
    tcp_socket.create_tcp_server(tcp_port);
    udp_socket.create_udp(UDP_PORT_SERVER);
    run();
}

void Server::accept_client() {
    struct sockaddr_in address;
    int client_socket = tcp_socket.accept_client(address);
    handle_client(client_socket);
}

void Server::register_client(int client_socket, const std::string& username, const std::string& role,
    const std::string& udp_port , struct sockaddr_in udp_address) {
    clients.push_back({stoi(udp_port) , client_socket, username, role, udp_address});
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
    
    vector<string> args;
    args = splitString(client_info , DELIM);

    struct sockaddr_in address;
    socklen_t client_len = sizeof(address);
    getpeername(client_socket, (struct sockaddr*)&address, &client_len);
    
    // for (auto arg : args)
    // cout << arg << endl;
    
    struct sockaddr_in client_udp_address;
    inet_pton(AF_INET, args[3].c_str(), &(client_udp_address.sin_addr));
    client_udp_address.sin_family = AF_INET;
    client_udp_address.sin_port = htons(stoi(args[2]));
    register_client(client_socket, args[0], args[1] , args[2] , client_udp_address);
}

void Server::run() {
    while (running) {
        if(tcp_socket.check_events())
        {
            accept_client();
        }
    }
}

void Server::stop() {
    for (size_t i = 0; i < clients.size(); i++) {
        cout << clients[i].username << endl;
    }
    running = false;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./server <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Invalid port number." << std::endl;
        return 1;
    }

    Server server(port);
    server.start();
    return 0;
}

bool Server::is_name_unique(string uname){
    for (auto clinet : clients)
    {
        if(uname == clinet.username)
            return false;
    }
    return true;
}
