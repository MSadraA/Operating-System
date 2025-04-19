#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include "define.hpp"
#include "shared_func.hpp"

int main() {
    vector<string> results;

    while (access(FINAL_RES_PIPE_PATH.c_str(), F_OK) == -1) {
        usleep(DELAY); // 100ms
    }

    int fd = open(FINAL_RES_PIPE_PATH.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("Failed to open final result pipe");
        return 1;
    }

    char buffer[BUFF_SIZE];
    string temp;
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        temp.append(buffer, bytes_read);

        size_t pos;
        while ((pos = temp.find('\n')) != string::npos) {
            string line = temp.substr(0, pos);
            temp.erase(0, pos + 1);
            results.push_back(line);
        }
    }

    close(fd);

    for (const auto& res : results) {
        cout << res << endl;
    }

    return 0;

}
