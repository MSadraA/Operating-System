#include "tcp_socket.hpp"

Tcp_socket::Tcp_socket() : tcp_socket(-1) {}

void Tcp_socket::create_tcp_server(int port) {
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        throw std::runtime_error("Failed to create TCP socket");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int reuse = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt failed");
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    if (bind(tcp_socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Bind error");
        throw std::runtime_error("Failed to bind TCP socket");
    }

    if (listen(tcp_socket, MAX_CLIENTS) == -1) {
        throw std::runtime_error("Failed to listen on TCP socket");
    }

    struct pollfd server_poll;
    server_poll.fd = tcp_socket;
    server_poll.events = POLLIN;
    poll_fds.push_back(server_poll);
}

void Tcp_socket::create_tcp_client(int server_port){
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        throw runtime_error("Failed to create TCP socket");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0;

    int reuse = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt failed");
        throw std::runtime_error("Failed to set SO_REUSEADDR");
    }

    if (bind(tcp_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("TCP Bind error");
        throw runtime_error("Failed to bind TCP socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(tcp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        throw runtime_error("Failed to connect to server");
    }
}

int Tcp_socket::accept_client(struct sockaddr_in &client_addr) {
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(tcp_socket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket < 0) {
        throw std::runtime_error("Failed to accept client");
    }

    struct pollfd client_poll;
    client_poll.fd = client_socket;
    client_poll.events = POLLIN;
    poll_fds.push_back(client_poll);

    return client_socket;
}

void Tcp_socket::send_massage(string message)
{
    if (send(tcp_socket, message.c_str(), message.size(), 0) < 0) {
        throw runtime_error("Failed to send client info");
    }
}

bool Tcp_socket::check_events() {
    int activity = poll(poll_fds.data(), poll_fds.size(), TMIE_OUT); // 500ms تایم اوت
    if (activity < 0) {
        throw runtime_error("Poll error");
    }
    return (activity > 0);
}

void Tcp_socket::close_socket() {
    if (tcp_socket != -1) {
        close(tcp_socket);
        tcp_socket = -1;
    }
}

string Tcp_socket::receive_message(int client_fd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        remove_poll(client_fd); 
        return "";
    }

    return std::string(buffer, bytes_received); 
}

void Tcp_socket::remove_poll(int client_fd) {
    close(client_fd);  

    for (auto it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == client_fd) {
            poll_fds.erase(it);
            break;
        }
    }

    std::cout << "Client " << client_fd << " disconnected and removed.\n";
}

Tcp_socket::~Tcp_socket() {
    close_socket();
}

