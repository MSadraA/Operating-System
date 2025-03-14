#include "server.hpp"

Server::Server(int tcp_port_) : tcp_port(tcp_port_), requestHandler(*this), running(true) {}


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

