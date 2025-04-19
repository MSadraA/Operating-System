#include "define.hpp"
#include "shared_func.hpp"
#include "worker.hpp"


int main() {
    int worker_num = 3;

    //make named pipe
    mkfifo(FINAL_RES_PIPE_PATH.c_str(), 0666);
    int network_pipe[worker_num][2];
    vector<pid_t> children;

    for (int i = 0; i < worker_num; ++i) {
        pipe(network_pipe[i]);
        pid_t pid = fork();

        if (pid == 0) {
            // child process (Worker)
            close(network_pipe[i][0]); //write only
            Worker worker(i, REGISTER_PIPE_PATH , network_pipe[i][1]);
            worker.run();
            return 0;
        }
        children.push_back(pid);
    }

    for(int i = 0 ; i < worker_num ; i++){
        close(network_pipe[i][1]);
    }

    int final_fd = open(FINAL_RES_PIPE_PATH.c_str(), O_WRONLY);
    if (final_fd < 0) {
        perror("Failed to open final pipe");
        return 1;
    }

    for (int i = 0; i < worker_num; ++i) {
        char buffer[BUFF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(network_pipe[i][0], buffer, sizeof(buffer))) > 0) {
            write(final_fd, buffer, bytes_read);
        }
        close(network_pipe[i][0]);
    }

    close(final_fd);

    for (int i = 0; i < worker_num; ++i) {
        wait(nullptr);
    }

    return 0;
}
