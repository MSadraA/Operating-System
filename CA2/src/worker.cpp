#include "worker.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>

Worker::Worker(int id, const string& pipe_path)
    : id(id), register_path(pipe_path) {}

void Worker::run() {
    
    register_self();
    read_records();
    show_records();
}

void Worker::register_self() {

    while (access(register_path.c_str(), F_OK) == -1) {
        usleep(100000); // 100 ms
    }

    int fd = open(register_path.c_str(), O_WRONLY);
    if (fd < 0) {
        perror("open worker pipe");
        return;
    }
    // Register this worker
    WorkerInfo info;
    info.id = id;
    info.pid = getpid();
    string worker_info = worker_to_string(info) + "\n";
    write(fd, worker_info.c_str(), worker_info.size());
    close(fd);
}

void Worker::read_records() {
    string worker_path = WORKER_PIPE_PATH + to_string(id);

    while (access(worker_path.c_str(), F_OK) == -1) {
        usleep(100000); // 100 ms
    }

    vector<GameRecord> records;
    int fd = open(worker_path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open worker pipe");
        return;
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
            GameRecord record = from_string(line);
            records.push_back(record);
        }
    }

    close(fd);
    if (records.size() >= 2) {
        max_record = records.back();
        records.pop_back();
        min_record = records.back();
        records.pop_back();
    } else {
        cerr << "[Worker " << id << "] Not enough records to extract min/max\n";
    }
    this->records = records;
}

void Worker::show_records() {
    cout << "Worker " << id << " records: " << endl;
    for (const auto& record : records) {
        print_record(record);
    }
    cout << "Worker " << id << " max record: " << endl;
    print_record(max_record);
    cout << "Worker " << id << " min record: " << endl;
    print_record(min_record);
    cout << endl;
}