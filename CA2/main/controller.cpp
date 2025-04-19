#include "define.hpp"

int main() { 
    pid_t extract_pid = fork(); 
    if (extract_pid == 0) { 
        execl("./bin/extract_and_transform", "extract_and_transform", nullptr); 
        exit(1); 
    }
    
    pid_t loader_pid = fork();
    if (loader_pid == 0) {
        execl("./bin/read_and_dispatch", "read_and_dispatch", nullptr);
        exit(1);
    }

    pid_t network_pid = fork();
    if (network_pid == 0) {
        execl("./bin/network_computing", "network_computing", nullptr);
        exit(1);
    }

    int status;
    waitpid(extract_pid, &status, 0);
    waitpid(loader_pid, &status, 0);
    waitpid(network_pid, &status, 0);

    return 0;
}
