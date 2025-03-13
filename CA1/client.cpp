#include "client.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

Client::Client(string uname, string r, int port) : username(uname), role(r), udp_port(port) {
    init_tcp();
    init_udp();
    cout << "Client " << username << " initialized with UDP port " << udp_port << endl;
}

void Client::init_tcp() {
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        throw runtime_error("Failed to create TCP socket");
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &client_addr.sin_addr);

    if (connect(tcp_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        throw runtime_error("Failed to connect to server");
    }
}

void Client::init_udp() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        throw runtime_error("Failed to create UDP socket");
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(udp_port);

    if (bind(udp_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        throw runtime_error("Failed to bind UDP socket");
    }
}

void Client::sendClientInfo() {
    string client_info = username + ":" + role;
    if (send(tcp_socket, client_info.c_str(), client_info.size(), 0) < 0) {
        throw runtime_error("Failed to send client info");
    }
    cout << "Client info sent: " << client_info << endl;
}

string Client::receiveUDPMessage() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    int bytes_received = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
    if (bytes_received <= 0) {
        return "could not receive message";
    }

    cout << "Client " << username << " received message: " << string(buffer) << endl;
    return string(buffer);
}

Client::~Client() {
    close(tcp_socket);
    close(udp_socket);
}