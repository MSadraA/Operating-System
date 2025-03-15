#include "udp_socket.hpp"

Udp_socket::Udp_socket(){
    udp_socket = -1;
}

void Udp_socket::make_poll() {
    struct pollfd broadcast_poll;
    broadcast_poll.fd = udp_socket;
    broadcast_poll.events = POLLIN;
    poll_fd = broadcast_poll;
}

void Udp_socket::create_udp(int port){
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        throw runtime_error("Failed to create UDP unicast socket");
    }
    memset(&address , 0, sizeof(udp_socket));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port); 
    int reuse = 1;
    setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (bind(udp_socket , (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind error (unicast)");
        throw runtime_error("Failed to bind UDP unicast socket");
    }
    make_poll();
}

void Udp_socket::unicast_message(const std::string& message, sockaddr_in& client_addr) {
    
    sendto(udp_socket , message.c_str(), message.size(), 0,
           (struct sockaddr*)&client_addr, sizeof(client_addr));   
}

bool Udp_socket::check_events() {
    int activity = poll(&poll_fd , 1 , 0);
    if (activity < 0) {
        throw runtime_error("Poll error");
    }
    return (activity > 0) && poll_fd.revents;
}

string Udp_socket::receive_message() {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recvfrom(udp_socket , buffer, sizeof(buffer), 0,
                                  (struct sockaddr*)&sender_addr, &addr_len);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "[DEBUG] Received Broadcast message: " << buffer << std::endl;
        return std::string(buffer);
    }
    
    return "";
}


void Udp_socket::close_socket() {
    if (udp_socket != -1) {
        close(udp_socket);
        udp_socket = -1;
    }
}

sockaddr_in Udp_socket::get_bound_address() {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    if (getsockname(udp_socket, (struct sockaddr*)&addr, &addr_len) == -1) {
        perror("getsockname failed");
    }
    return addr;
}



Udp_socket::~Udp_socket() {
    close_socket();
}