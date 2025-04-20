#include "define.hpp"
#include "shared_func.hpp"
#include "loader.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <worker_num>\n";
        return 1;
    }
    int worker_num = stoi(argv[1]);
    Loader loader(EXTRACT_PIPE_PATH , REGISTER_PIPE_PATH , worker_num);
    loader.run();
    return 0;
}