//
// Created by vikac on 11.04.19.
//

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <unistd.h>
#include <wait.h>
#include <sstream>
#include <cstring>
#include <string>
#include <cerrno>

char ** split(std::string const& s) {
    std::vector<std::string> vargs;
    auto in = std::istringstream(s);
    std::string arg;
    while(in >> arg) {
        vargs.push_back(arg);
    }

    auto sz = vargs.size();
    auto args = new char* [sz + 1];
    for (int i = 0; i < sz; ++i) {
        args[i] = const_cast<char*>(vargs[i].data());
    }
    args[sz] = nullptr;
    return args;
}

void print_error(std::string const &message) {
    std::cerr << message << strerror(errno) << std::endl;
}

void execute(char * args[], char * envp[]) {
    pid_t pid = fork();

    if(pid == 0) {
        if(execve(args[0], args, envp) == -1) {
            print_error("Execution is failed : ");
            exit(-1);
        }
    } else if(pid < 0) {
        print_error("Creation of a child process was unsuccessful.");
    } else {
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            print_error("Wait for any child process.");
        }
    }
}

void dollar() {
    std::cout << "$ ";
    std::cout.flush();
}

int main(int argc, char * argv[], char * envp[]) {

    dollar();
    std::string line;
    while(std::getline(std::cin, line)) {
        if(line == "exit") break;
        auto args = split(line);
        execute(std::unique_ptr<char*[]>(args).get(), envp);
        dollar();
    }

    return 0;
}