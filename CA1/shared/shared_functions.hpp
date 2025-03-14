#ifndef SHARED_FUNCTIONS_HPP
#define SHARED_FUNCTIONS_HPP

#include <netinet/in.h>
#include <vector>
#include <poll.h>
#include <map>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/timerfd.h>
#include <iostream>
#include <sstream>
#include <random>

using namespace std;

vector<string> splitString(const string& str, char delimiter);
string get_address_as_string(sockaddr_in address);

#endif
