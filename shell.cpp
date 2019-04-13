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
    const int pid = fork();
    if (pid == -1) {
        print_error("Fork failed: ");
    } else if (pid == 0) {
        auto args_char = string_to_char(args);
        char **argv = args_char.data();
        std::vector<char *> env_vector = string_to_char(get_env_vect());
        if (execve(argv[0], argv, env_vector.data()) == -1) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == EXIT_SUCCESS) {
            std::cout << "Execution succeed: " << WEXITSTATUS(status) << std::endl;
        } else {
            print_error("Execution failed: ");
        }
    }
}

void add_env(const std::string& token) {
    //std::string name, value;
    auto eq_index = token.find('=');
    if (eq_index == std::string::npos) {
        print_error("Incorrect 'set' command: ");
    }
    std::string name = token.substr(0, eq_index);
    std::string value = token.substr(eq_index + 1);
    env[name] = value;
}

void erase_env(const std::string& token) {
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
    }
    execute(args);
}

int main() {
    while (true) {
        std::cout << "$";
        std::cout.flush();
        std::string command;
        getline(std::cin, command);
        if (command == "Exit program") {
            break;
        }
        run(command);
    }
    return 0;
}
