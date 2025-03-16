#include "request_handler.hpp"
#include "server.hpp"

Request_handler::Request_handler(Server& srv) : server(srv) {
    handlers[REG] = [&](int client_fd, string data) { handle_register(client_fd, data); };
    handlers[MSG] = [&](int client_fd, string data) { handle_message(client_fd, data); };
    handlers[SBMT] = [&](int client_fd, string data) { handle_submit(client_fd, data); };
    handlers[SHARE] = [&](int client_fd, string data) { handle_share(client_fd, data); };
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
    {
        server.udp_socket.unicast_message( TEAMMATE_ERR , server.find_client_info(client_fd).client_udp_address);
        return;
    }
    string type = MSG + DELIM;
    string msg = type + data;
    server.tcp_socket.send_message_to_client(team_socket , msg);
}

void Request_handler::handle_share(int client_fd, string data){
    int team_socket = server.find_teammate_by_socket(client_fd);
    if(team_socket == -1)
    {
        server.udp_socket.unicast_message( TEAMMATE_ERR , server.find_client_info(client_fd).client_udp_address);
        return;
    }
    string type = SHARE + DELIM;
    string msg = type + data;
    server.tcp_socket.send_message_to_client(team_socket , msg);
}

void Request_handler::handle_submit(int client_fd, string data){
    int team_socket = server.find_teammate_by_socket(client_fd);
    Team& team = server.find_team(client_fd);
    float score = server.scores[server.cur_problem];
    if(team_socket == -1)
    {
        server.udp_socket.unicast_message( TEAMMATE_ERR , server.find_client_info(client_fd).client_udp_address);
        return;
    }
    if(team.has_submit)
    {
        string type = BRDCST + DELIM;
        server.udp_socket.unicast_message( type + "You have already submitted your answer." 
            , server.find_client_info(client_fd).client_udp_address);
        return;
    }
    if(!server.is_contest_started){
        string type = BRDCST + DELIM;
        server.udp_socket.unicast_message( type + "Contest is not started" , server.find_client_info(client_fd).client_udp_address);
        return;
    }
    // make client tcp socket
    Tcp_socket socket;
    socket.create_tcp_client(EVAL_PORT);

    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono:: seconds>(now - server.last_problem_time).count();
    float extra_point = 0.0;
    // Send code to evaluation server
    string code = server.problems[server.cur_problem] + '\n' + data;
    string answer;
    answer = socket.send_and_receive_blocking(code , WAIT);
    string msg = "\n";
    msg += "ELPASED TIME : " + to_string(elapsed_time) + " seconds" + ' ';
    if(elapsed_time <= (TIME)/2 && answer == "PASS")
    {
        extra_point = score *(0.5);
        msg += " You get " + to_string(extra_point) + " extra points.";
    }
    else if(elapsed_time <= (TIME*3/4) && answer == "PASS")
    {
        extra_point = score *(0.2);
        msg += " You get " + to_string(extra_point) + " extra points.";
    }
    msg += '\n';
    msg += "RESULT : " + answer + '\n';
    float final_score = score + extra_point;
    if(answer == "PASS")
        team.score += final_score;
    else 
        final_score = 0.0;
    msg += "The score of this problem : " + to_string(final_score) + '\n';
    msg += "Total points : " + to_string(team.score) + '\n';
    server.send_message_to_team(team , msg);
    team.has_submit = true;
    socket.close_socket();
}

