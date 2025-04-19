#ifndef LOADER_HPP
#define LOADER_HPP

#include "define.hpp"
#include "shared_func.hpp"

class Loader {
    public:
        Loader(const string& in_pipe_path_ , const string& register_path_ , int workers_num_);
        void show_cpu_usage();
        void run();

        private:
        string register_path;
        string in_pipe_path;
        vector<GameRecord> records;
        GameRecord max_records;
        GameRecord min_records;
        vector<WorkerInfo> workers;
        int workers_num;

        void read_from_pipe();
        void read_registered_workers();
        void fill_extreme_records();
        void fill_max_records();
        void fill_min_records();
        double get_cpu_usage(int pid);
        void sort_workers_by_cpu_usage();
        void dispatch_records_to_workers();
        void send_records_to_worker(const WorkerInfo& worker, const vector<GameRecord>& chunk);
};
#endif