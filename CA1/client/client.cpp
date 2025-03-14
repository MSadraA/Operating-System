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
    while(running) {
        if (udp_socket.check_events()) {
            std::string message = udp_socket.receive_message();
            if (!message.empty()) {
                std::cout << "[Client " << username << "] Received message: " << message << std::endl;
            }
        }
    }
}


void Client::stop()
{
    running = false;
}

void Client::sendClientInfo() {
    // cout << get_address_as_string(udp_socket.get_bound_address());
    string client_info = username + DELIM + role + DELIM + to_string(udp_port) + DELIM + get_address_as_string(udp_socket.get_bound_address());
    tcp_socket.send_massage(client_info);
}


Client::~Client() {
    stop();
    Port_manager::getInstance()->release_port(udp_port);
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

    Client client(username, role, udp_port , server_tcp_port);
    return 0;
}


