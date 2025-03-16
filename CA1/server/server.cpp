#include "server.hpp"

Server::Server(int tcp_port_) : tcp_port(tcp_port_), requestHandler(*this), running(false) {
    tcp_socket.create_tcp_server(tcp_port);
    // evaluation_socket.create_tcp_client(EVAL_PORT);
    udp_socket.create_udp(UDP_PORT_SERVER);
    udp_broadcast_socket.create_udp_broad_cast();
    // init problems
    problems.push_back(PROBLEM_1);
    problems.push_back(PROBLEM_2);
    problems.push_back(PROBLEM_3);
    // init scores
    scores.push_back(SCORE_1);
    scores.push_back(SCORE_2);
    scores.push_back(SCORE_3);

    is_contest_started = false;
    cur_problem = 0;
    add_poll(tcp_socket.get_fd());
}

void Server::accept_client() {
    struct sockaddr_in address;
    int client_socket = tcp_socket.accept_client(address);
    handle_client(client_socket);
}

void Server::register_client(int client_socket, const std::string& username, const std::string& role,
    const std::string& udp_port , struct sockaddr_in udp_address , bool has_team_) {
    clients.push_back({stoi(udp_port) , client_socket, username, role, udp_address , has_team_});
    
    make_teames();
}


void Server::remove_team(int client_socket) {
    ClientInfo& p1 = find_client_info(find_teammate_by_socket(client_socket));
    ClientInfo& p2 = find_client_info(client_socket);
    p1.has_team = false;
    p2.has_team = false;
    for (auto it = teams.begin(); it != teams.end(); ++it) {
        if (p1.socket == client_socket || p2.socket == client_socket) {
            teams.erase(it); 
            string type = BRDCST + DELIM;
            string msg = type + "Your teammate has been disconnected!";
            udp_socket.unicast_message(msg , p1.client_udp_address);
            return; 
        }
    }
}

void Server::remove_poll(int client_socket) {
    for (auto it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == client_socket) {
            poll_fds.erase(it);
            break;
        }
    }
}

void Server::remove_client(int client_socket) {
    ClientInfo& client = find_client_info(client_socket);
    if(client.has_team)
    {
        if(teams.size() > 0)
            remove_team(client_socket);
    }

    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].socket == client_socket) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
    remove_poll(client_socket);
}

// Handlers
void Server::handle_client(int client_socket) {
    string input = tcp_socket.receive_message_from_client(client_socket);

    if (input == "") {
        remove_client(client_socket);
        return;
    }
    string req_type;
    string data;

    
    split_by_delim(input , DELIM , req_type , data);
    if(data == "" || req_type == "")
        return;
    // cout << "DATA : " << data << endl;
    // cout << "TYPE : " << req_type << endl;
    requestHandler.handleRequest(client_socket , req_type , data);
}

void Server::check_console_poll(struct pollfd & stdin_poll){
    int stdin_poll_count = poll(&stdin_poll, 1, 0);
    if (stdin_poll_count > 0 && (stdin_poll.revents & POLLIN)) {
        std::string command;
        read_line(command); 
        if (command != "quit" && command != "start") {
            processCommand(command);
        }
    }
}

void Server::send_message_to_team(const Team& team , string message){
    string type = BRDCST + DELIM;
    udp_socket.unicast_message(type + message , find_client_info(team.team_members.first).client_udp_address);
    udp_socket.unicast_message(type + message , find_client_info(team.team_members.second).client_udp_address);
}


void Server::start_contest(){
    last_problem_time = std::chrono::steady_clock::now();
    auto now = std::chrono::system_clock::now();
    auto end_time = now + std::chrono::minutes(TIME/60 * MAX_PROBLEM);

    time_t start_timestamp = std::chrono::system_clock::to_time_t(now);
    time_t end_timestamp = std::chrono::system_clock::to_time_t(end_time);

    std::string start_time_str = format_time(start_timestamp);
    std::string end_time_str = format_time(end_timestamp);

    std::string msg = "Contest has started! Start time: " + start_time_str +
                      " End time: " + end_time_str;

    broadcast_message_to_teams(msg);
    send_problem();
    is_contest_started = true;
}

void Server::reset_submit(){
    for (auto& team : teams)
        team.has_submit = false;
}

void Server::check_elpased_time(){
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - last_problem_time);

    if(cur_problem >= MAX_PROBLEM){
        broadcast_message_to_teams("The contest is over...");
        is_contest_started = false;
        cur_problem = 0;
        return;
    }

    if (elapsed_time.count() >= TIME) { // check time left
        cur_problem += 1;
        send_problem();
        last_problem_time = now;
    }
}

void Server::run() {
    // making console poll
    struct pollfd stdin_poll;
    stdin_poll.fd = STDIN_FILENO;
    stdin_poll.events = POLLIN;
    poll_fds.push_back(stdin_poll);

    while (running) {
        auto now = std::chrono::steady_clock::now();

        int timeout = -1;
        if (is_contest_started) {
            auto next_check_time = last_problem_time + std::chrono::seconds(1);
            timeout = std::chrono::duration_cast<std::chrono::milliseconds>(next_check_time - now).count();
            if (timeout < 0) timeout = 0;
        }
        
        int poll_count = poll(poll_fds.data(), poll_fds.size(), timeout);
        if (poll_count < 0) {
            throw std::runtime_error("Poll error");
            break;
        }   

        // Sendes problem every x min
        if(is_contest_started)
            check_elpased_time();

        // Receive connect request from clients
        if (poll_fds[0].revents & POLLIN) {
            if (tcp_socket.check_events()) {
                accept_client();
                continue;
            }
        }
        
        // Receive other request from clients
        for (size_t i = 0; i < poll_fds.size(); i++) { 
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == STDIN_FILENO) {
                    check_console_poll(poll_fds[i]);
                } else {
                    handle_client(poll_fds[i].fd);
                }
            }
        }
    }
}

bool Server::is_name_unique(string uname){
    for (auto clinet : clients)
    {
        if(uname == clinet.username)
            return false;
    }
    return true;
}

void Server::make_teames(){
    ClientInfo& new_client = clients.back();
    string role = new_client.role;
    for(auto& client : clients){
        if(!client.has_team){
            if((role == CODER && client.role == NAVIGATOR) ||(role == NAVIGATOR && client.role == CODER))
            {
                client.has_team = true;
                new_client.has_team = true;
                teams.push_back({make_pair(client.socket , new_client.socket) , 0 , false}); // init team
                string type = BRDCST + DELIM;
                string msg = type + "A teammate has been found for you !";
                udp_socket.unicast_message(msg , client.client_udp_address);
                udp_socket.unicast_message(msg , new_client.client_udp_address);
            }
        }
    }
}

void Server::add_poll(int fd){
    struct pollfd server_poll;
    server_poll.fd = fd;
    server_poll.events = POLLIN;
    poll_fds.push_back(server_poll);
}

// Server commands
void Server::help() {
    print("Available commands:\n");
    print("  help    - Show this help message\n");
    print("  status  - Show server status\n");
    print("  start   - Start the server\n");
    print("  quit    - Quit the server\n");
    print("  stop    - Stop the server\n");
    print("  contest    - Start the contest\n");
    print("\n");
}

void Server::status() {
    string tmp1 = "Number of users : " + std::to_string(clients.size()) + "\n";
    print(tmp1);

    string tmp2 = "Current Problem : " + problems[cur_problem] + "\n";
    print(tmp2);

    string tmp3 = "Teams :\n";
    print(tmp3);

    for (auto& x : teams) {
        string tmp4 = find_client_info(x.team_members.first).username + 
                      " and " + 
                      find_client_info(x.team_members.second).username + 
                      "\n";
        print(tmp4);
    }
}


void Server::start() {
    running = true;
    run();
}

void Server::stop() {
    running = false;
}


void Server::quit() {
    if (!running) {
        return;
    }
    running = false;  
    tcp_socket.close_socket();
    udp_socket.close_socket();

    for (const auto& client : clients) {
        close(client.socket); 
    }
    clients.clear();
    teams.clear();
}
void Server::processCommand(const std::string& command) {
    if (command == "help") {
        help();
    } else if (command == "status") {
        status();
    } else if (command == "start") { // start a server
        start(); 
    } else if (command == "quit") { // delete the server
        quit();
    } else if (command == "stop") { // stop the server 
        stop();
    }
    else if (command == "contest") { // start contest
        start_contest();
    }
        else {
        print("Unknown command. Type 'help' for a list of commands.\n");
    }
}

void Server::send_problem(){
    for(auto team : teams){
        string msg = "Problem " + to_string(cur_problem + 1) + " is : " + '"' + problems[cur_problem] + '"' 
        + " you have " + to_string(TIME) +
        " seconds to solve it";
        if(cur_problem < MAX_PROBLEM)
        {
            send_message_to_team(team , msg);
            broadcast_problem_to_teams();
        }
    }
    reset_submit();
}

void Server::broadcast_message_to_teams(const std::string& message){
    for(auto team : teams){
        send_message_to_team(team , message);
    }
}

void Server::broadcast_problem_to_teams(){
    string type = PRBLM + DELIM;
    string msg = type + problems[cur_problem];
    for (auto team : teams){
        udp_socket.unicast_message(msg , find_client_info(team.team_members.first).client_udp_address);
        udp_socket.unicast_message(msg , find_client_info(team.team_members.second).client_udp_address);
    }
};

// Finders
int Server::find_teammate_by_socket(int client_socket) {
    for (const auto& team : teams) {
        int p1 = team.team_members.first;
        int p2 = team.team_members.second;
        
        if (p1 == client_socket) 
            return p2;
        if (p2 == client_socket) 
            return p1;
    }
    return -1;
}

Team& Server::find_team(int socket){
    for (auto& team : teams){
        if(team.team_members.first == socket || team.team_members.second == socket)
            return team;
    }
}

ClientInfo& Server::find_client_info(int client_socket)
{
    for (auto& client : clients) {
        if (client.socket == client_socket) {
            return client; 
        }
    }
    throw std::runtime_error("Client not found");
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            throw std::runtime_error("Usage: ./server <port>");
        }

        int port = std::atoi(argv[1]);
        if (port <= 0) {
            throw std::invalid_argument("Invalid port number.");
        }

        Server server(port);
        std::string command;

        while (true) {
            print("> ");
            read_line(command);
            if (command == "quit") {
                server.processCommand(command);
                break;
            }
            server.processCommand(command);
        }

        return 0;
    } catch (const std::exception& e) {
        print("Error: " + std::string(e.what()) + '\n');
        return 1;
    } 
}
