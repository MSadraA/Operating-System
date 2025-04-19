#include "define.hpp"
#include "shared_func.hpp"
#include "loader.hpp"


int main() {
    Loader loader(EXTRACT_PIPE_PATH , REGISTER_PIPE_PATH , 3);
    loader.run();
    return 0;
}