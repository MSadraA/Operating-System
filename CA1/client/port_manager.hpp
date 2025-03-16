#ifndef PORT_MANAGER_HPP
#define PORT_MANAGER_HPP

#include "define.hpp"

#define MIN  10000
#define MAX  20000

class Port_manager {
private:
    static Port_manager* instance; 
    std::set<int> used_ports;
    int min_port;
    int max_port;

   
    Port_manager(int min_p = MIN, int max_p = MAX);

public:
    static Port_manager* getInstance();

    int generate_unique_port();
    void release_port(int port);

    ~Port_manager();
};

#endif // PORT_MANAGER_HPP
