#ifndef WORKER_HPP
#define WORKER_HPP

#include "define.hpp"
#include "shared_func.hpp"

class Worker {
public:
    Worker(int id, const string& pipe_path , int write_pipe);
    void show_records();
    void run();

private:
    int id;
    string register_path;
    int write_pipe;

    vector<GameRecord> records;
    GameRecord max_record;
    GameRecord min_record;
    vector<Score> scores;

    void register_self();
    void read_records();
    void min_max_scale();
    void calculate_indicators();
    void write_to_pipe();
};

inline GameField& operator++(GameField& f) {
    f = static_cast<GameField>(static_cast<int>(f) + 1);
    return f;
}


#endif
