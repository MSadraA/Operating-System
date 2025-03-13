#include "server.hpp"
#include "client.hpp"
#include <thread>
#include <chrono>
#include <iostream>

const char* BROADCAST_IP = "127.255.255.255";

Server* globalServer = nullptr;

void startServer() {
    try {
        Server server(8080);
        globalServer = &server;
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}

void startClient(const string& username, const string& role, int udp_port) {
    try {
        Client client(username, role, udp_port , globalServer->get_tcp_port());
        client.sendClientInfo(); 
        this_thread::sleep_for(chrono::seconds(1)); 
        string message = client.receiveUDPMessage();
        cout << "Received message: " << message << endl;
    } catch (const std::exception& e) {
        std::cerr << "Client error (" << username << "): " << e.what() << std::endl;
    }
}

int main() {
    try {
        // Start server in a separate thread
        thread serverThread(startServer);

        // Give the server some time to start
        this_thread::sleep_for(chrono::seconds(2));

        // Start clients with different UDP ports
        thread clientThread1(startClient, "user1", "role1", 9091);
        thread clientThread2(startClient, "user2", "role2", 9092);
        this_thread::sleep_for(chrono::seconds(1));

        cout << "Clients started" << endl;

        // Detach client threads
        clientThread1.detach();
        clientThread2.detach();
        cout << "Clients detached" << endl;

        // Broadcast a message to all clients
        if (globalServer) {
            globalServer->message_broadcast("📢 This is a broadcast message!");
            globalServer->stop();
        }

        // Detach the server thread
        serverThread.detach(); // Detach the server thread to allow the program to exit
        cout << "Server detached" << endl;
    } catch (const std::exception& e) {
        std::cerr << "Main error: " << e.what() << std::endl;
    }

    return 0;
}