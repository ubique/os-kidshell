#include <iostream>
#include <vector>
#include <unistd.h>
#include "Command.h"
#include "Runner.h"
#include "ParserException.hpp"
#include "RunnerException.hpp"
int main() {
    int return_status = 0;
    std::string line;

    while(true) {
        Command c;
        std::unique_ptr<Runner> r;

        std::cout << return_status << " -> " << std::flush;

        std::getline(std::cin, line);

        try {
            c.parse(line);
        } catch(const ParserException& e) {
            std::cerr << "Could not parse: " << e.what() << std::endl;
        }

        try {
            r.reset(new Runner(c));
        } catch(const RunnerException& e) {
            std::cerr << "Could not create process: " << e.what() << std::endl;
        }

        try {
            return_status = r->wait();
        } catch(const RunnerException& e) {
            std::cerr << "waitpid() failed: " << e.what() << std::endl;
        }
    }
}
