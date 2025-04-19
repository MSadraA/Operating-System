#include "extractor.hpp"
#include "transformer.hpp"
#include "define.hpp"


const int PROC_NUM = 3;
const vector<string> csv_files = {
    "assets/steamdb1.csv",
    "assets/steamdb2.csv",
    "assets/steamdb3.csv"
};

int main() {
    //make named pipe
    mkfifo(EXTRACT_PIPE_PATH.c_str(), 0666);

    int extract_pipes[PROC_NUM][2];
    int transformer_pipes[PROC_NUM][2];
    vector<pid_t> children;

    for (int i = 0; i < PROC_NUM; ++i) {
        pipe(extract_pipes[i]);
        pipe(transformer_pipes[i]);

        // extractor fork
        pid_t pid1 = fork();
        if (pid1 == 0) {
            close(extract_pipes[i][0]); // write only
            close(transformer_pipes[i][0]);
            close(transformer_pipes[i][1]);
            Extractor extractor(csv_files[i], extract_pipes[i][1]);
            extractor.run();
            exit(0);
        }
        children.push_back(pid1);

        // transformer fork
        pid_t pid2 = fork();
        if (pid2 == 0) {
            close(extract_pipes[i][1]);      // read only
            close(transformer_pipes[i][0]);  // write only

            Transformer transformer(extract_pipes[i][0], transformer_pipes[i][1]);
            transformer.run();
            exit(0);
        }

        children.push_back(pid2);
    }

    for(int i = 0 ; i < PROC_NUM ; i++){
        close(extract_pipes[i][0]);
        close(extract_pipes[i][1]);
        close(transformer_pipes[i][1]);
    }

    // write all data's in named pipe
    int final_fd = open(EXTRACT_PIPE_PATH.c_str(), O_WRONLY);
    if (final_fd < 0) {
        perror("Failed to open final pipe");
        return 1;
    }

    for (int i = 0; i < PROC_NUM; ++i) {
        char buffer[BUFF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(transformer_pipes[i][0], buffer, sizeof(buffer))) > 0) {
            write(final_fd, buffer, bytes_read);
        }
        close(transformer_pipes[i][0]);
    }

    close(final_fd);

    // wait for children
    for (pid_t pid : children) {
        waitpid(pid, nullptr, 0);
    }
}