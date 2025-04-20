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

void remove_pipes(int worker_num){
    unlink(EXTRACT_PIPE_PATH.c_str());
    unlink(REGISTER_PIPE_PATH.c_str());
    unlink(FINAL_RES_PIPE_PATH.c_str());
    for(int i = 0 ; i < worker_num ; i++){
        string path = WORKER_PIPE_PATH + to_string(i);
        unlink(path.c_str());
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <clalculator_nodes_count> <files_count>\n";
        return 1;
    }

    int worker_num = stoi(argv[1]);
    int file_num = stoi(argv[2]);

    make_pipes(worker_num);

    string worker_str = to_string(worker_num);
    string file_str = to_string(file_num);

    pid_t extract_pid = fork(); 
    if (extract_pid == 0) { 
        execl("./bin/extract_and_transform", "extract_and_transform", file_str.c_str() ,nullptr); 
        exit(1); 
    }

    pid_t loader_pid = fork();
    if (loader_pid == 0) {
        execl("./bin/read_and_dispatch", "read_and_dispatch", worker_str.c_str(), nullptr);
        exit(1);
    }
    
    pid_t network_pid = fork();
    if (network_pid == 0) {
        execl("./bin/network_computing", "network_computing", worker_str.c_str(), nullptr);
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
    remove_pipes(worker_num);
    return 0;
}
