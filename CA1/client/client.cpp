#include "client.hpp"


Client::Client(string uname, string r, int port , 
int server_tcp_port_): username(uname), role(r), udp_port(port) {
    server_tcp_port = server_tcp_port_;
    tcp_socket.create_tcp_client(server_tcp_port);
    udp_socket.create_udp(port);
    sendClientInfo();
}

void Client::start()
{
    running = true;
    run();
}

// Handle message
void Client::handle_message(string msg){
    string req_type;
    string data;
    split_by_delim(msg , DELIM , req_type , data);
    string tmp = "";
    if(req_type == MSG){
        tmp = "Message from your teammate : " + data + "\n";
        print(tmp);
    }
    else if(req_type == SHARE)
    {
        tmp = "Code from coder :" + data + "\n";
        print(tmp);
        current_code = data;
    }
    else if(req_type == PRBLM)
    {
        current_problem_id = data;
    }
    else if(req_type == BRDCST)
    {
        tmp = "Server : " + data + "\n";
        print(tmp);
    }
    else {
        tmp = "Server : " + data + "\n";
        print(tmp);
    }
}

void Client::run() {
    struct pollfd fds[2];
    // Console
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    //tcp socket
    fds[1].fd = tcp_socket.get_fd();
    fds[1].events = POLLIN;
    
    // Udp_socket udp_broadcast_socket;
    while (running) {
        int poll_count = poll(fds, 2, TIME_OUT);  
        if (poll_count < 0) {
            print("Error in poll: " + std::string(strerror(errno)) + "\n");
            throw std::runtime_error("poll error");
        }
        // Console commands 
        if (fds[0].revents & POLLIN) {
            std::string command;
            read_line(command);
            if (command == "quit") {
                stop();
                break;
            } else {
                process_command(command);
            }
        }
        // TCP request
        if (fds[1].revents & POLLIN) {
            std::string message = tcp_socket.receive_message_from_server();
            if (!message.empty()) {
                handle_message(message);
            }
        }
        //UDP request
        if ((udp_socket.check_events() == true) && POLLIN) {
                std::string message = udp_socket.receive_message();
            if (!message.empty()) {
                handle_message(message);
            }
        }
    }
}

void Client::stop()
{
    running = false;
}

void Client::sendClientInfo() {
    string type = REG + DELIM;
    string client_info = type + username + DELIM + role + DELIM + to_string(udp_port) 
    + DELIM + get_address_as_string(udp_socket.get_bound_address());

    tcp_socket.send_massage_to_server(client_info);
}


void Client::send_message_to_team(string msg){
    string type = MSG + DELIM;
    string msg_snd = type + msg;
    
    tcp_socket.send_massage_to_server(msg_snd);
}

void Client::share(){
    string type = SHARE + DELIM;
    string code = "";
    for (auto x : current_code)
        code += x;
    string msg_snd = type + code;
    tcp_socket.send_massage_to_server(msg_snd);
}

string Client::write_code(string before) {
    string line;
    read_line(line);
    
    if (line == "save") {
        print("Code saved successfully.\n");
        if (!before.empty() && before.back() == ';') {
            before.pop_back();
        }
        return before;
    }
    
    string n_line = before + line;
    
    if (!line.empty()) {
        if (line.back() != ':') 
            return write_code(n_line + ";");
        else 
            return write_code(n_line);
    }
    else 
        return write_code(before);
}



void Client::show_problem(){
    string tmp = "The problem is : " + current_problem_id + "\n";
    print(tmp);
}

void Client::submit_code() {
    if (current_code == "") {
        print("No code written.\n");
        return;
    }
    string type = SBMT + DELIM;
    string final_code = type + current_code;
    tcp_socket.send_massage_to_server(final_code);
}

Client::~Client() {
    stop();
    Port_manager::getInstance()->release_port(udp_port);
}

void Client::help() {
    print("Available commands:\n");
    print("  help         - Show this help message\n");
    print("  chat <msg>   - Send a message to your team\n");
    print("  problem      - Show the current problem statement\n");
    if (role == CODER) {
        print("  code         - Enter coding mode to write your solution\n");
        print("  share        - Share your code with your teammate\n");
    } 
    else if (role == NAVIGATOR) {
        print("  submit       - Submit the current code to the server\n");
    }
    print("\n");
}

void Client::process_command(const string& command) {
    std::string trimmed_command = trim(command); 
    // pablic commands
    if (trimmed_command == "help") {
        help();
        return;
    } 
    else if (trimmed_command.rfind("chat ", 0) == 0) { 
        std::string message = trimmed_command.substr(5);
        send_message_to_team(message);
        return;
    } 
    else if (trimmed_command.rfind("problem", 0) == 0) {
        show_problem();
        return;
    }
    // private commands
    else {
        if (role == CODER) {
           if(trimmed_command.rfind("code", 0) == 0){
            print("Start writing your code. Type 'save' on a new line to finish.\n");
            current_code = write_code();
            return;
           }
           else if(trimmed_command.rfind("share", 0) == 0){
            share();
            return;
           }
        }
        else if(role == NAVIGATOR) {
           if(trimmed_command.rfind("submit", 0) == 0){
            submit_code();
            return;
           }
        }
        else {
            print("Unknown command. Type 'help' for a list of commands.\n");
        }
    }
}


int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            throw std::invalid_argument("Usage: ./client <username> <role> <server_tcp_port>");
        }

        string username = argv[1];
        string role = argv[2];
        int server_tcp_port = stoi(argv[3]);

        int udp_port = Port_manager::getInstance()->generate_unique_port();

        Client client(username, role, udp_port, server_tcp_port);
        client.start();
    } 
    catch (const std::invalid_argument& e) {
        print("Invalid argument error: " + std::string(e.what()) + "\n");
        return 1;
    }
    catch (const std::exception& e) {
        print("Error: " + std::string(e.what()) + "\n");
        return 1;
    }
    catch (...) {
        print("Unknown error occurred!\n");
        return 1;
    }
    return 0;
}

