#include "define.hpp"
#include "shared_func.hpp"
#include "worker.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <worker_num>\n";
        return 1;
    }
    int worker_num = stoi(argv[1]);

    int network_pipe[worker_num][2];
    vector<pid_t> children;

    for (int i = 0; i < worker_num; ++i) {
        if (pipe(network_pipe[i]) < 0) {
            perror("pipe creation failed");
            return 1;
        }
    }

    for (int i = 0; i < worker_num; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            close(network_pipe[i][0]);

            Worker worker(i, REGISTER_PIPE_PATH, network_pipe[i][1]);
            worker.run();

            close(network_pipe[i][1]);  
            return 0;
        }

        close(network_pipe[i][1]);
        children.push_back(pid);
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

    for (pid_t child : children) {
        waitpid(child, nullptr, 0);
    }

    return 0;
}
