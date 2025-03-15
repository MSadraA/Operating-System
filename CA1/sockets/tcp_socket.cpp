#include "tcp_socket.hpp"

Tcp_socket::Tcp_socket() : tcp_socket(-1) {}

void Tcp_socket::make_poll(){
    struct pollfd server_poll;
    server_poll.fd = tcp_socket;
    server_poll.events = POLLIN;
    poll_fd = server_poll;
}

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

    make_poll();
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
    return client_socket;
}

void Tcp_socket::send_massage_to_server(string message)
{
    ssize_t sent_bytes = send(tcp_socket, message.c_str(), message.size(), 0);
    if (sent_bytes == -1) {
        std::cerr << "Error sending message: " << strerror(errno) << std::endl;
    } else 
        std::cout << "Message sent: " << message << std::endl;
}

bool Tcp_socket::check_events() {
    int activity = poll(&poll_fd , 1 , 0);
    if (activity < 0) {
        throw runtime_error("Poll error");
    }
    return (activity > 0) && poll_fd.revents;
}

void Tcp_socket::close_socket() {
    if (tcp_socket != -1) {
        close(tcp_socket);
        tcp_socket = -1;
    }
}

string Tcp_socket::receive_message_from_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        return "";
    }

    return std::string(buffer, bytes_received); 
}

void Tcp_socket::send_message_to_client(int client_fd, const string& message) {
    send(client_fd, message.c_str(), message.size(), 0);
}

string Tcp_socket::receive_message_from_server() {
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(tcp_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        return string(buffer, bytes_received);
    }
    return "";
}

Tcp_socket::~Tcp_socket() {
    close_socket();
}

