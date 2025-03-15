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
    if(req_type == MSG){
        cout << "Message from your teammate : " << data << endl;
    }
    else if(req_type == SHARE)
    {
        cout << "Code from coder :" << data << endl;
        current_code = data;
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
            std::cerr << "Error in poll: " << strerror(errno) << std::endl;
            throw std::runtime_error("poll error");
        }
        // Console commands 
        if (fds[0].revents & POLLIN) {
            std::string command;
            std::getline(std::cin, command);
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
                std::cout << "UDP message: " << message << std::endl;
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

void Client::write_code() {    
    cout << "Start writing your code. Type 'save' on a new line to finish.\n";
    current_code = "";
    
    string line;
    while (true) {
        getline(cin, line);
        if (line == "save") {
            cout << "Code saved successfully.\n";
            break;
        }
        current_code += line;
    }
}


void Client::show_problem(){
    cout << "The problem is : " << current_problem_id << endl;
}

void Client::submit_code() {
    if (current_code == "") {
        cout << "No code written. Use 'write_code' first.\n";
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
    std::cout << "Available commands:\n";
    std::cout << "  help         - Show this help message\n";
    std::cout << "  chat <msg>   - Send a message to your team\n";
    std::cout << "  problem      - Show the current problem statement\n";

    if (role == CODER) {
        std::cout << "  code         - Enter coding mode to write your solution\n";
        std::cout << "  share        - Share your code with your teammate\n";
    } 
    else if (role == NAVIGATOR) {
        std::cout << "  submit       - Submit the current code to the server\n";
    }

    std::cout << std::endl;
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
            write_code();
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
            std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
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
        std::cerr << "Invalid argument error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 1;
    }

    return 0;
}

