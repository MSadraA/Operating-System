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
        usleep(100000); // 100 ms
    }

    ifstream pipe(FINAL_RES_PIPE_PATH);
    if (!pipe.is_open()) {
        cerr << "Failed to open pipe: " << FINAL_RES_PIPE_PATH << endl;
        return 0;
    }

    string line;
    while (getline(pipe, line)) {
        results.push_back(line);
    }
    pipe.close();

    for(auto res : results){
        cout << res << endl;
    }

}
