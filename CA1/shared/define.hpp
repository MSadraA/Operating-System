#ifndef DEFINE_H
#define DEFINE_H

#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include <map>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/timerfd.h>
#include <iostream>

using namespace std;

#define STDIN 0
#define STDOUT 1

#define MAX_CLIENTS 100

// Server Broadcast
#define UDP_PORT_SERVER 8081
#define BROAD_CAST_IP "255.255.255.255"


// Error Messages
#define SOCKET_ERROR_MSG "Failed to create socket"
#define BIND_ERROR_MSG "Failed to bind socket"
#define LISTEN_ERROR_MSG "Failed to listen on socket"
#define ACCEPT_ERROR_MSG "Failed to accept connection"
#define RECV_ERROR_MSG "Failed to receive message"
#define SEND_ERROR_MSG "Failed to send message"
#define CONNECT_ERROR_MSG "Failed to connect"
#define BROADCAST_ERROR_MSG "Failed to enable broadcast on UDP"

// Buffer 
#define BUFFER_SIZE 1024

// Delimiter
#define DELIM ':'

// IP
#define SERVER_IP "127.0.0.1"
extern const char* BROADCAST_IP;

// Requst Types
#define REQUEST_CONNECT "connect"
#define REQUEST_MESSAGE "message"
#define REQUEST_GET_PROBLEM "get_problem"
#define REQUEST_SEND_SOLUTION "send_solution"

// Request Errors
#define REQUEST_ERROR_INVALID "invalid_request"
#define REQUEST_ERROR_NOT_FOUND "not_found"
#define REQUEST_ERROR_INVALID_PARAMS "invalid_params"

// Player type
#define CODER "coder"
#define NAVIGATOR "navigator"

// Server request type
const string REG  = "register";
const string MSG  = "message";

// Problem signature
#define PROBLEM_1 "def add_number(a, b)"
#define PROBLEM_2 "def reverse_string(s)"
#define PROBLEM_3 "def is_palindrome(s)"

// Clients
#define CLIENT_INFO_ARGS 3
#define TIME_OUT 500 //ms


const std::vector<std::string> SIGNT = {PROBLEM_1, PROBLEM_2, PROBLEM_3};

#endif