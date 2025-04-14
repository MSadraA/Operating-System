#include "extractor.hpp" 
#include "define.hpp"

int main() { 
    std::vector<std::string>  csv_files = { "assets/steamdb1.csv", "assets/steamdb2.csv", "assets/steamdb3.csv" };

    std::vector<std::string> pipe_paths = {
        "/pipes/pipe1",
        "/pipes/pipe2",
        "/pipes/pipe3"
    };

    for (const std::string& path : pipe_paths) {
        mkfifo(path.c_str(), 0666);
    }

    for (int i = 0; i < 3; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            Extractor extractor(csv_files[i], pipe_paths[i]);
            extractor.run();
            exit(0);
        }
    }

    for (int i = 0; i < 3; ++i) {
        wait(nullptr);
    }

    for (int i = 0; i < 3; ++i) {
        int fd = open(pipe_paths[i].c_str(), O_RDONLY);
        if (fd == -1) {
            perror("open pipe for reading failed");
            continue;
        }
    
        char buffer[1024];
        ssize_t bytesRead;
        std::cout << "Output from pipe " << pipe_paths[i] << ":\n";
    
        while ((bytesRead = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        }
    
        close(fd);
        std::cout << "-------------------------\n";
    }
    
}

