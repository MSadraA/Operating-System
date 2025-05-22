#include "loader.hpp"

Loader::Loader(const string& in_pipe_path_ , const string& register_path_ , int workers_num_) {
    register_path = register_path_;
    in_pipe_path = in_pipe_path_;
    workers_num = workers_num_;
    max_records = { "", 0, 0, 0, 0, 0, 0 };
    min_records = { "", 0, 0, 0, 0, 0, 0 };
}

void Loader::read_from_pipe() {
    int fd = open(in_pipe_path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror(("open failed: " + in_pipe_path).c_str());
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
    
            GameRecord record = string_to_game_record(line);
            records.push_back(record);
        }
    }
    close(fd);
}

void Loader::run() {
    read_from_pipe();
    fill_extreme_records();
    read_registered_workers();
    // show_cpu_usage();
    dispatch_records_to_workers();
}

void Loader::read_registered_workers(){
    int dummy = open(register_path.c_str(), O_RDONLY | O_NONBLOCK);
    close(dummy);

    int fd = open(register_path.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open register pipe");
        return;
    }

    while (int(workers.size()) < workers_num) {
        string line;
        char ch;
        while (read(fd, &ch, 1) == 1) {
            if (ch == '\n') break;
            line += ch;
        }
        if (!line.empty()) {
            workers.push_back(string_to_worker(line));
        } else {
            usleep(DELAY);
        }
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
    while (access(pipe_path.c_str(), F_OK) == -1) {
        usleep(DELAY);
    }

    int fd = open(pipe_path.c_str(), O_WRONLY);
    if (fd < 0) {
        perror(("open worker pipe: " + pipe_path).c_str());
        return;
    }

    for (const GameRecord& r : chunk) {
        string line = game_record_to_string(r) + "\n";
        if (write(fd, line.c_str(), line.size()) < 0) {
            perror("write record failed");
        }
    }
    string min_str = game_record_to_string(min_records) + "\n";
    string max_str = game_record_to_string(max_records) + "\n";

    if (write(fd, min_str.c_str(), min_str.size()) < 0) 
        perror("write min failed");
    
    if (write(fd, max_str.c_str(), max_str.size()) < 0) 
        perror("write max failed");
    
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
