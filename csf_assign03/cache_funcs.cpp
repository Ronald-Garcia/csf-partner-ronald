#include "cache_funcs.h"

void read_file(std::istream &in, std::deque<std::string> &trace) {

    std::string buf;
    while (!in.eof()) {
        getline(in, buf);
        trace.push_back(buf);
    }
}