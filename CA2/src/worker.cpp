#include "worker.hpp"

Worker::Worker(int id, const string& pipe_path , int write_pipe)
    : id(id),register_path(pipe_path),write_pipe(write_pipe){}

void Worker::run() {
    register_self();
    read_records();
    show_records();
    min_max_scale();
    cout << "after scale: "<< endl;
    show_records();
    calculate_indicators();
    write_to_pipe();
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
    ssize_t w = write(fd, worker_info.c_str(), worker_info.size());
    if (w == -1) perror("write failed");

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

void Worker::min_max_scale(){
    for(auto& record : records){
        record.original_price = (record.original_price - min_record.original_price) / (max_record.original_price - min_record.original_price) + 1;
        record.discount_percent = (record.discount_percent - min_record.discount_percent) / (max_record.discount_percent - min_record.discount_percent) + 1;
        record.recent_review_summary = (record.recent_review_summary - min_record.recent_review_summary) / (max_record.recent_review_summary - min_record.recent_review_summary) + 1;
        record.all_review_summary = (record.all_review_summary - min_record.all_review_summary) / (max_record.all_review_summary - min_record.all_review_summary) + 1;
        record.recent_review_number = (record.recent_review_number - min_record.recent_review_number) / (max_record.recent_review_number - min_record.recent_review_number) + 1;
        record.all_review_number = (record.all_review_number - min_record.all_review_number) / (max_record.all_review_number - min_record.all_review_number) + 1;
    }
}

void Worker::calculate_indicators(){
    for(auto& record : records){
        float indicator = 10 * ((record.discount_percent*record.all_review_number*record.all_review_summary*record.recent_review_number*record.recent_review_summary)/record.original_price);
        scores.push_back({record.title , indicator});
    }
}

void Worker::write_to_pipe() {

    for(int i = 0 ; i < int(scores.size()) ; i++){
        string line = score_to_string(scores[i]) + "\n";
        ssize_t written = write(write_pipe, line.c_str(), line.size());
        if (written < 0) {
            perror("write failed");
            break;
        }
    }

    fsync(write_pipe);
    close(write_pipe);
}
