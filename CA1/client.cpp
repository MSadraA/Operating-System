#include "client.hpp"

class Client {
private:
    int tcp_socket, udp_socket;
    struct sockaddr_in server_tcp_addr, client_udp_addr;
    string username;
    string role;
    void init_tcp();
    void init_udp();

public:
    Client(string uname, string r) : username(uname), role(r) {
        init_tcp();
        init_udp();
    }

    ~Client() {
        close(tcp_socket);
        close(udp_socket);
    }
};
    void Client::init_tcp(){
        tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (tcp_socket == -1) {
            throw runtime_error(SOCKET_ERROR_MSG);
            exit(EXIT_FAILURE);
        }

        server_tcp_addr.sin_family = AF_INET;
        server_tcp_addr.sin_port = htons(TCP_PORT);
        inet_pton(AF_INET, SERVER_IP, &server_tcp_addr.sin_addr);
    }

    void Client::init_udp(){
        udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket == -1) {
            throw runtime_error(SOCKET_ERROR_MSG);
            exit(EXIT_FAILURE);
        }

        client_udp_addr.sin_family = AF_INET;
        client_udp_addr.sin_addr.s_addr = INADDR_ANY;
        client_udp_addr.sin_port = htons(UDP_PORT);

        if (bind(udp_socket, (struct sockaddr*)&client_udp_addr, sizeof(client_udp_addr)) < 0) {
            throw runtime_error(BIND_ERROR_MSG);
            exit(EXIT_FAILURE);
        }
    }

