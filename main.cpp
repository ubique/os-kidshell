#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

void show_error() {
    std::cout << "The following error occurred: " << strerror(errno) << std::endl;
}

std::vector<std::string> split(std::string const& line) {
    std::istringstream iss{line};
    return std::vector<std::string>(std::istream_iterator<std::string> {iss}, std::istream_iterator<std::string> {});
}

std::vector<char *> convert_to_pointers(std::vector<std::string> const& args) {
    std::vector<char *> pointers{args.size() + 1};
    std::transform(args.begin(), args.end(), pointers.begin(), [](std::string const& str) {
        return const_cast<char*>(str.c_str());
    });
    pointers.back() = nullptr;
    return pointers;
}

void process(std::string const& line, char** envp) {
    if (line.empty()) {
        return;
    }
    auto args = split(line);
    if (args.empty()) {
        return;
    }
    auto pointers = convert_to_pointers(args);
    switch (auto pid = fork()) {
    case -1:
        std::cout << "Cannot fork: " << strerror(errno) << std::endl;
        return;
    case 0:
        if (execve(pointers[0], pointers.data(), envp) == -1) {
            show_error();
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    default:
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            show_error();
        } else if (WIFEXITED(status)) {
            std::cout << "Process exit code: " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "Unhandled signal: " << strsignal(WTERMSIG(status)) << std::endl;
        } else if (WIFSTOPPED(status)) {
            std::cout << "Process stopped because of signal: " << strsignal(WSTOPSIG(status)) << std::endl;
        }
    }
}

int main(int argc, char **argv, char **envp) {
    std::cout << "Welcome to kidshell\n$ ";
    std::cout.flush();
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (std::cin.eof() || line == "exit") {
            break;
        }
        process(line, envp);
        std::cout << "$ ";
        std::cout.flush();
    }
}
