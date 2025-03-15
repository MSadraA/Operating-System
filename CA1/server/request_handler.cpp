#include "request_handler.hpp"
#include "server.hpp"

Request_handler::Request_handler(Server& srv) : server(srv) {
    handlers[REG] = [&](int client_fd, string data) { handle_register(client_fd, data); };
    handlers[MSG] = [&](int client_fd, string data) { handle_message(client_fd, data); };
    //
}


void Request_handler::handleRequest(int client_fd, string request_type, string data) {
    if (handlers.find(request_type) != handlers.end()) {
        handlers[request_type](client_fd, data);
    } else {
        throw runtime_error(REQUEST_ERROR_INVALID);
    }
}

void Request_handler::handle_register(int client_fd, string data) {
    vector<string> args;
    args = splitString(data , DELIM);

    struct sockaddr_in address;
    socklen_t client_len = sizeof(address);
    getpeername(client_fd, (struct sockaddr*)&address, &client_len);
    
    struct sockaddr_in client_udp_address;
    inet_pton(AF_INET, args[3].c_str(), &(client_udp_address.sin_addr));
    client_udp_address.sin_family = AF_INET;
    client_udp_address.sin_port = htons(stoi(args[2]));  
     
    if(!server.is_name_unique(args[0])){
        server.udp_socket.unicast_message("Your username is duplicate." , client_udp_address);
        return;
    }        
    else
    {
        server.register_client(client_fd, args[0], args[1] , args[2] , client_udp_address , false);
        server.add_poll(client_fd);
    }
}

void Request_handler::handle_message(int client_fd, string data){
    int team_socket = server.find_teammate_by_socket(client_fd);
    if(team_socket == -1)
        return;
    server.tcp_socket.send_message_to_client(team_socket , data);
}



