//
// Created by daniil on 13.04.19.
//

#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <wait.h>
#include <unordered_map>

std::unordered_map<std::string, std::string> env;

std::vector<std::string> get_env_vect() {
    std::vector<std::string> result;
    result.reserve(env.size());
    for (const auto &e: env) {
        result.push_back(e.first + "=" + e.second);
    }
    return result;
}

void print_error(const std::string &message) {
    std::cerr << message << strerror(errno) << std::endl;
    std::cerr.flush();
}

std::vector<std::string> parse_args(const std::string &command) {
    std::istringstream command_stream(command);
    std::vector<std::string> result;
    std::string cur;
    while (command_stream >> cur) {
        result.push_back(cur);
    }
    return result;
}

std::vector<char *> string_to_char(const std::vector<std::string> &args) {
    std::vector<char *> result;
    result.reserve(args.size());
    for (const std::string &arg: args) {
        result.push_back(const_cast<char *>(arg.data()));
    }
    return result;
}

void execute(const std::vector<std::string> &args) {
    auto args_char = string_to_char(args);
    args_char.push_back(nullptr);
    char **argv = args_char.data();
    std::vector<char *> env_vector = string_to_char(get_env_vect());
    env_vector.push_back(nullptr);
    char** envv = env_vector.data();
    pid_t pid = fork();
    if (pid == -1) {
        print_error("Fork failed: ");
    } else if (pid == 0) {
        if (execve(argv[0], argv, envv) == -1) {
            exit(-1);
        }
        exit(EXIT_SUCCESS);
    } else {
        int status;
        pid_t wait_pid = waitpid(pid, &status, 0);
        if (wait_pid == -1) {
            print_error("Execution failed: ");
        } else {
            std::cout << "Execution returns code: " << WEXITSTATUS(status) << std::endl;
        }
    }
}

void add_env(const std::string &token) {
    auto eq_index = token.find('=');
    if (eq_index == std::string::npos) {
        print_error("Incorrect 'set' command: ");
        return;
    }
    std::string name = token.substr(0, eq_index);
    std::string value = token.substr(eq_index + 1);
    env[name] = value;
}

void erase_env(const std::string &token) {
    if (env.count(token)) {
        env.erase(token);
    }
}

void run(const std::string &command) {
    auto args = parse_args(command);
    if (args.empty()) return;
    if (args[0] == "set") {
        for (size_t i = 1; i < args.size(); i++) {
            add_env(args[i]);
        }
    } else if (args[0] == "unset") {
        for (size_t i = 1; i < args.size(); i++) {
            erase_env(args[i]);
        }
    } else {
        execute(args);
    }
}

void print_greet() {
    std::cout << "$ ";
    std::cout.flush();
}

int main() {
    print_greet();
    while (true) {
        std::string command;
        getline(std::cin, command);
        if (command == "exit") {
            break;
        }
        run(command);
        print_greet();
    }
    return 0;
}
