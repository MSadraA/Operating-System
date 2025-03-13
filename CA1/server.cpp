#include "define.hpp"
#include "request_handler.hpp"
#include "client.hpp"

class Server {
    private:
    int tcp_socket;
    int udp_socket;
    struct sockaddr_in tcp_address;
    struct sockaddr_in udp_address;
    vector<Client> clients;
    Request_handler requestHandler;
    vector<struct pollfd> poll_fds;
    
    void init_tcp();
    void init_udp();

    public:
    Server();
    void start() {
        init_tcp();
        init_udp();
    };
    void accept_client() {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(tcp_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            throw runtime_error(ACCEPT_ERROR_MSG);
            return;
        }

        struct pollfd client_poll;
        client_poll.fd = client_socket;
        client_poll.events = POLLIN;
        poll_fds.push_back(client_poll);
    }

    void remove_client(int client_socket) {
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].fd == client_socket) {
                poll_fds.erase(poll_fds.begin() + i);
                break;
            }
        }
        close(client_socket);
    }

    void handle_client(int client_socket) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            throw runtime_error(RECV_ERROR_MSG);
            close(client_socket);
            remove_client(client_socket);
            return;
        }

        string request(buffer);
        string request_type = request.substr(0, request.find(":"));
        string data = request.substr(request.find(":") + 1);

        requestHandler.handleRequest(client_socket, request_type, data);
    }

};

    void Server::init_tcp(){
        tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket == -1){
            throw runtime_error(SOCKET_ERROR_MSG);
        }
        tcp_address.sin_family = AF_INET;
        tcp_address.sin_addr.s_addr = inet_addr(SERVER_IP);
        tcp_address.sin_port = htons(TCP_PORT);

        if(bind(tcp_socket, (struct sockaddr*)&tcp_address, sizeof(tcp_address)) == -1){
            throw runtime_error(BIND_ERROR_MSG);
        }
        if(listen(tcp_socket, MAX_CLIENTS) == -1){
            throw runtime_error(LISTEN_ERROR_MSG);
        }
    }

    void Server::init_udp(){
        udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(udp_socket == -1){
            throw runtime_error(SOCKET_ERROR_MSG);
        }
        udp_address.sin_family = AF_INET;
        udp_address.sin_addr.s_addr = inet_addr(BROADCAST_IP);
        udp_address.sin_port = htons(UDP_PORT);

        int broadcastEnable = 1;
        if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            throw runtime_error(BROADCAST_ERROR_MSG);
        }
    }

    


    