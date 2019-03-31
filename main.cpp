#include "Logger.h"
#include "Executor.h"

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include <sstream>
#include <algorithm>


os::Logger logger;

os::Executor executor;

void init();

void print_usage();

int main() {
    init();

    std::string s;
    while (getline(std::cin, s)) {

        int success = executor.execute(s);
        if (success == -1) {
            std::cout << ("Something went wrong.Try again.") << std::endl;
        }
        if (success == 0) {
            continue;
        }
        if (success ==+1) {
            return 0;
        }
    }
}

void init() {
    logger.info("init started");
//    freopen("../input.txt", "r", stdin);

    print_usage();

    logger.info("init finished");
}

const auto USAGE = "***"
                   "\n"
                   "Welcome to custom implementation of shell. Written by Pavel Yatcheniy."
                   "\n"
                   "Usage:"
                   "\n"
                   "fullpath [args...] - to execute fullpath with args as arguments"
                   "\n"
                   "exit - to exit from shell"
                   "\n"
                   "***";

void print_usage() {
    std::cout << USAGE << std::endl;
}
