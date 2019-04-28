#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <cerrno>

char** parse(const std::string& s) {
    std::stringstream stream(s);
    std::string tmp;
    std::vector<std::string> vector;
    while (stream >> tmp) {
        vector.push_back(tmp);
    }
    char** args = new char* [vector.size() + 1];
    args[vector.size()] = nullptr;
    for (int i = 0; i < vector.size(); ++i) {
        args[i] = new char[vector[i].size() + 1];
        strcpy(args[i], vector[i].c_str());
    }
    return args;
}

int main() {
    std::string s;
    while (std::getline(std::cin, s)) {
        char** args = parse(s);
        
        pid_t pid = fork();
        int code = -1;
        if (pid == 0) {
            if (execv(args[0], args) == -1) {
                std::cout << strerror(errno) << std::endl;
            }
        } else {
            int status = 0;
            pid_t p = waitpid(pid, &status, 0);
            if (p > 0) {
                if (WIFEXITED(status)) {
                    code = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    std::cerr << "Program was terminated by signal with code " << WTERMSIG(status) << std::endl;
                } else if (WCOREDUMP(status)) {
                    std::cerr << "Program generated a error in core of system." << std::endl;
                } else {
                    std::cerr << "Process was stopped by signal with code " << WSTOPSIG(status) << std::endl;
                }
            } else {
                std::cout << strerror(errno) << std::endl;
            }
        }
        std::cout << "Program returns with code " << code << std::endl;
    }
    return 0;
}