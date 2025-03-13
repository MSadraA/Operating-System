#ifndef SERVER_HPP
#define SERVER_HPP

#include "define.hpp"
#include "request_handler.hpp"
#include "client.hpp"
#include <vector>
#include <poll.h>
#include <map>
#include <string>

struct ClientInfo {
    int socket;
    std::string username;
    std::string role;
    struct sockaddr_in addr;
};

class Server {
private:
    int tcp_socket;
    int udp_socket;
    struct sockaddr_in tcp_address;
    struct sockaddr_in udp_address;
    std::vector<ClientInfo> clients; // وکتور برای نگهداری اطلاعات کلاینت‌ها
    std::map<int, int> teams; // نقشه تیم‌ها
    Request_handler requestHandler;
    std::vector<struct pollfd> poll_fds;
    bool running; // فلگ برای کنترل حلقه اصلی

    void init_tcp();
    void init_udp();
    void accept_client();
    void handle_client(int client_socket);
    void remove_client(int client_socket);
    void register_client(int client_socket, const std::string& username, const std::string& role, struct sockaddr_in addr);

public:
    Server();
    void start();
    void run();
    void stop(); // تابع برای متوقف کردن سرور
    void message_broadcast(const std::string& message); // تابع جدید برای ارسال پیام broadcast
};

#endif // SERVER_HPP