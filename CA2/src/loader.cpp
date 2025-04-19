#include "loader.hpp"

Loader::Loader(const string& in_pipe_path_ , const string& register_path_ , int workers_num_) {
    register_path = register_path_;
    in_pipe_path = in_pipe_path_;
    workers_num = workers_num_;
    max_records = { "", 0, 0, 0, 0, 0, 0 };
    min_records = { "", 0, 0, 0, 0, 0, 0 };
}

void Loader::read_from_pipe() {
    while (access(in_pipe_path.c_str(), F_OK) == -1) {
        usleep(100000); // 100 ms
    }

    ifstream pipe(in_pipe_path);
    if (!pipe.is_open()) {
        cerr << "Failed to open pipe: " << in_pipe_path << endl;
        return;
    }

    string line;
    while (getline(pipe, line)) {
        records.push_back(from_string(line));
    }

    pipe.close();
}

void Loader::run() {
    read_from_pipe();
    fill_extreme_records();
    read_registered_workers();
    dispatch_records_to_workers();
    // for (const auto& record : records) {
    //     print_record(record);
    // }
    // for (const auto& worker : workers) {
    //     print_worker(worker);
    // }
    // print_record(max_records);
    // print_record(min_records);
}

void Loader::read_registered_workers(){
    // make register pipe
    mkfifo(register_path.c_str() , 0666);
    // just open from the reader side
    int dummy = open(register_path.c_str(), O_RDONLY | O_NONBLOCK);
    close(dummy);

    int fd = open(register_path.c_str(), O_RDONLY);
    for (int i = 0; i < workers_num; ++i) {
        string line;
        char ch;
        while (read(fd, &ch, 1) == 1) {
            if (ch == '\n') break;
            line += ch;
        }
        workers.push_back(string_to_worker(line));
    }
    close(fd);
}

void Loader::fill_max_records(){
    for(auto record : records){
        if(record.original_price > max_records.original_price){
            max_records.original_price = record.original_price;
        }
        if(record.discount_percent > max_records.discount_percent){
            max_records.discount_percent = record.discount_percent;
        }
        if(record.recent_review_summary > max_records.recent_review_summary){
            max_records.recent_review_summary = record.recent_review_summary;
        }
        if(record.all_review_summary > max_records.all_review_summary){
            max_records.all_review_summary = record.all_review_summary;
        }
        if(record.recent_review_number > max_records.recent_review_number){
            max_records.recent_review_number = record.recent_review_number;
        }
        if(record.all_review_number > max_records.all_review_number){
            max_records.all_review_number = record.all_review_number;
        }  
    }
}
void Loader::fill_min_records(){
    for(auto record : records){
        if(record.original_price < min_records.original_price){
            min_records.original_price = record.original_price;
        }
        if(record.discount_percent < min_records.discount_percent){
            min_records.discount_percent = record.discount_percent;
        }
        if(record.recent_review_summary < min_records.recent_review_summary){
            min_records.recent_review_summary = record.recent_review_summary;
        }
        if(record.all_review_summary < min_records.all_review_summary){
            min_records.all_review_summary = record.all_review_summary;
        }
        if(record.recent_review_number < min_records.recent_review_number){
            min_records.recent_review_number = record.recent_review_number;
        }
        if(record.all_review_number < min_records.all_review_number){
            min_records.all_review_number = record.all_review_number;
        }  
    }
}
void Loader::fill_extreme_records(){
    fill_max_records();
    min_records = max_records;
    fill_min_records();
}

double Loader::get_cpu_usage(int pid){
    ifstream proc_stat("/proc/" + to_string(pid) + "/stat");
    if (!proc_stat.is_open()) return 0.0;

    string line;
    getline(proc_stat, line);
    istringstream iss(line);
    string token;
    long utime = 0, stime = 0;

    for (int i = 1; i <= 15 && iss >> token; ++i) {
        if (i == 14) utime = stol(token);
        else if (i == 15) stime = stol(token);
    }

    return static_cast<double>(utime + stime);
}

void Loader::sort_workers_by_cpu_usage(){
    sort(workers.begin(), workers.end(), [this](const WorkerInfo& a, const WorkerInfo& b) {
        return get_cpu_usage(a.pid) < get_cpu_usage(b.pid);
    });
}

void Loader::send_records_to_worker(const WorkerInfo& worker, const vector<GameRecord>& chunk) {
    string pipe_path = WORKER_PIPE_PATH + to_string(worker.id);
    mkfifo(pipe_path.c_str(), 0666);

    int fd = open(pipe_path.c_str(), O_WRONLY);
    if (fd < 0) {
        perror("open worker pipe");
        return;
    }

    for (const GameRecord& r : chunk) {
        string s = to_string(r) + "\n";
        write(fd, s.c_str(), s.size());
    }
    string min = to_string(min_records) + "\n";
    string max = to_string(max_records) + "\n";
    write(fd, min.c_str(), min.size());
    write(fd, max.c_str(), max.size());

    close(fd);
}


void Loader::dispatch_records_to_workers(){
    
        sort_workers_by_cpu_usage();

        int num_workers = workers.size();
        int num_records = records.size();
        int records_per_worker = num_records / num_workers;
        int remainder = num_records % num_workers;
    
        int index = 0;
        for (int i = 0; i < num_workers; ++i) {
            int count = records_per_worker + (i < remainder ? 1 : 0);
            vector<GameRecord> chunk(records.begin() + index, records.begin() + index + count);
            index += count;
    
            send_records_to_worker(workers[i], chunk);
        }
}

void Loader::show_cpu_usage(){
    // Print CPU usage for each worker
    for(auto worker : workers){
        cout << "Worker " << worker.id << " CPU usage: " << get_cpu_usage(worker.pid) << endl;
    }
}