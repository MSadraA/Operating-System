#include "define.hpp"
#include "shared_func.hpp"
#include "worker.hpp"


int main() {
    int worker_num = 3;

    for (int i = 0; i < worker_num; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            // child process (Worker)
            Worker worker(i, REGISTER_PIPE_PATH);
            worker.run();
            return 0;
        } else if (pid < 0) {
            cerr << "Failed to fork for worker " << i << endl;
        }
    }

    // optional: parent can wait for children
    for (int i = 0; i < worker_num; ++i) {
        wait(nullptr);
    }

    return 0;
}
