#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main() {
    const string pipe_path = "pipes/extract_pipe";

    int fd = open(pipe_path.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("failed to open pipe for reading");
        return 1;
    }

    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytes_read] = '\0'; 
        cout << buffer;
    }

    close(fd);
    return 0;
}
