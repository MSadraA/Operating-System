#ifndef WORKER_HPP
#define WORKER_HPP

#include "define.hpp"
#include "shared_func.hpp"

class Worker {
public:
    Worker(int id, const string& pipe_path);
    void show_records();
    void run();

private:
    int id;
    string register_path;
    vector<GameRecord> records;
    GameRecord max_record;
    GameRecord min_record;

    void register_self();
    void read_records();
};

#endif
