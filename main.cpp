#include <iostream>
#include <zconf.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <wait.h>

struct command {
    std::string path;
    std::vector<char *> argv;

    explicit command(const std::vector<std::string> &args) : path(args[0]), argv(args.size() + 1) {
        for (int i = 0; i < args.size(); i++) {
            argv[i] = new char[args[i].size()];
            args[i].copy(argv[i], args[i].size(), 0);
        }
        argv.back() = nullptr;
    }
};


command parsecmd(const std::string &s) {
    std::istringstream iss(s);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{});
    if (tokens.empty()) {
        return {};
    }
    return command(tokens);

}

int execve(command const &cmd) {
    std::vector<char *> env(1, nullptr);
    return execve(cmd.path.data(), cmd.argv.data(), env.data());
}

void run(command const &cmd) {
    int pid = fork();
    if (pid == -1) {
        perror("fork");
    } else if (pid == 0) {
        execve(cmd);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("");
        } else if (WIFEXITED(status)) {
            std::cout << "Returned code: " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cerr << "Execution is stopped with signal " << WSTOPSIG(status) << std::endl;
        }
    }
}

int main() {
    while (true) {
        std::string s;
        getline(std::cin, s);
        auto cmd = parsecmd(s);
        if (((cmd.path) == "exit") || (std::cin.eof())) {
            break;
        }
        run(cmd);
    }
    return 0;
}