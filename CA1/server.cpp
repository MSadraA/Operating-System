#include "server.hpp"

Server::Server(int tcp_port_) : requestHandler(teams), running(true){
    tcp_port = tcp_port_;
}

void Server::start() {
    init_tcp();
    init_udp();

    struct pollfd server_poll;
    server_poll.fd = tcp_socket;
    server_poll.events = POLLIN;
    poll_fds.push_back(server_poll);
    run();
}

void Server::init_tcp() {
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        throw runtime_error("Failed to create TCP socket");
    }

    tcp_address.sin_family = AF_INET;
    tcp_address.sin_addr.s_addr = INADDR_ANY;
    tcp_address.sin_port = htons(tcp_port);

    if (bind(tcp_socket, (struct sockaddr*)&tcp_address, sizeof(tcp_address)) == -1) {
        perror("Bind error");
        throw runtime_error("Failed to bind TCP socket");
    }

    if (listen(tcp_socket, MAX_CLIENTS) == -1) {
        throw runtime_error("Failed to listen on TCP socket");
    }
}

void Server::init_udp() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        throw runtime_error("Failed to create UDP socket");
    }

    udp_address.sin_family = AF_INET;
    udp_address.sin_addr.s_addr = INADDR_ANY;
    udp_address.sin_port = htons(UDP_PORT);

    if (bind(udp_socket, (struct sockaddr*)&udp_address, sizeof(udp_address)) < 0) {
        perror("Bind error");
        throw runtime_error("Failed to bind UDP socket");
    }
}

void Server::accept_client() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(tcp_socket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket < 0) {
        throw runtime_error("Failed to accept client");
    }

    struct pollfd client_poll;
    client_poll.fd = client_socket;
    client_poll.events = POLLIN;
    poll_fds.push_back(client_poll);

    // cout << "Client connected, waiting for info..." << endl;

    handle_client(client_socket);
}

void Server::register_client(int client_socket, const std::string& username, const std::string& role, struct sockaddr_in addr) {
    clients.push_back({client_socket, username, role, addr});
    // cout << "New client : " << username;
}

void Server::remove_client(int client_socket) {
    for (size_t i = 0; i < poll_fds.size(); i++) {
        if (poll_fds[i].fd == client_socket) {
            poll_fds.erase(poll_fds.begin() + i);
            break;
        }
    }
    close(client_socket);

    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].socket == client_socket) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}

void Server::handle_client(int client_socket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        remove_client(client_socket);
        return;
    }

    string client_info(buffer);
    string username = client_info.substr(0, client_info.find(":"));
    string role = client_info.substr(client_info.find(":") + 1);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr*)&client_addr, &client_len);

    register_client(client_socket, username, role, client_addr);
}

void Server::run() {
    while (running) {
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            throw runtime_error("Poll error");
        }

        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == tcp_socket) {
                    accept_client();
                } else {
                    handle_client(poll_fds[i].fd);
                }
            }
        }
    }
}

void Server::stop() {
    for (size_t i = 0; i < clients.size(); i++) {
        cout << clients[i].username << endl;
    }
    running = false;
}

void Server::message_broadcast(const std::string& message) {
    struct sockaddr_in broadcast_addr;
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, BROADCAST_IP, &broadcast_addr.sin_addr);

    int broadcast_enable = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        throw runtime_error("Failed to set broadcast option");
    }

    if (sendto(udp_socket, message.c_str(), message.size(), 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        throw runtime_error("Failed to send broadcast message");
    }
}