#include "define.hpp"

void make_pipes(int worker_num){
    mkfifo(EXTRACT_PIPE_PATH.c_str() , 0666);
    mkfifo(REGISTER_PIPE_PATH.c_str() , 0666);
    mkfifo(FINAL_RES_PIPE_PATH.c_str() , 0666);
    for(int i = 0 ; i < worker_num ; i++){
        string path = WORKER_PIPE_PATH + to_string(i);
        mkfifo(path.c_str() , 0666);
    }
}

int main() {
    make_pipes(3);

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

    pid_t write_pid = fork();
    if (write_pid == 0) {
        execl("./bin/write_to_csv", "write_to_csv", nullptr);
        exit(1);
    }

    int status;
    waitpid(extract_pid, &status, 0);
    waitpid(loader_pid, &status, 0);
    waitpid(network_pid, &status, 0);
    waitpid(write_pid, &status, 0);

    return 0;
}
