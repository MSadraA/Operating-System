#include "port_manager.hpp"

Port_manager* Port_manager::instance = nullptr;

// Constructor
Port_manager::Port_manager(int min_p, int max_p) : min_port(min_p), max_port(max_p) {
    srand(time(0));
}

// Destructor
Port_manager::~Port_manager() {
    delete instance;
    instance = nullptr;
}

// Singleton Implementation 
Port_manager* Port_manager::getInstance() {
    if (!instance) {
        instance = new Port_manager();
    }
    return instance;
}

int Port_manager::generate_unique_port() {
    int port;
    do {
        port = rand() % (max_port - min_port + 1) + min_port;
    } while (used_ports.find(port) != used_ports.end());

    used_ports.insert(port);
    return port;
}

void Port_manager::release_port(int port) {
    used_ports.erase(port);
}
