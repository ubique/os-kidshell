//
// Created by vitalya on 03.04.19.
//

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <wait.h>

static std::vector<char*> ENV = {nullptr};


std::vector<std::string> parse_command(const std::string& command) {
    std::istringstream command_stream(command);
    std::vector<std::string> result;
    std::string tmp;
    while (command_stream >> tmp) {
        result.push_back(tmp);
    }
    return result;
}

void set_env_var(std::string_view expr) {
    auto eq_index = expr.find('=');
    if (eq_index == std::string_view::npos) {
        std::cerr << "Usage: set KEY=VALUE [KEY=VALUE*]" << std::endl;
        return;
    }

    char* expr_ptr = new char[expr.size() + 1];
    memcpy(expr_ptr, expr.data(), expr.size());
    expr_ptr[expr.size()] = '\0';

    std::string_view key = expr.substr(0, eq_index);
    auto it = std::find_if(ENV.begin(), ENV.end() - 1,
            [key](const char* tmp_expr) -> bool {return memcmp(tmp_expr, key.data(), key.size()) == 0;});

    if (it == ENV.end() - 1) {
        ENV.back() = expr_ptr;
        ENV.push_back(nullptr);
    } else {
        delete[] *it;
        *it = expr_ptr;
    }

}

void unset_env_var(std::string_view var) {
    auto it = std::find_if(ENV.begin(), ENV.end() - 1,
            [var](const char* tmp_expr) -> bool {return memcmp(tmp_expr, var.data(), var.size()) == 0;});

    if (it != ENV.end() - 1) {
        delete[] *it;
        ENV.erase(it);
    }
}

void execute(const std::vector<std::string>& args) {
    std::vector<char*> args_ptrs;
    args_ptrs.reserve(args.size());
    for (const std::string& arg : args) {
        args_ptrs.push_back(const_cast<char*>(arg.data()));
    }

    int argc = args.size();
    char** argv = args_ptrs.data();

    switch (pid_t pid = fork()) {
        case -1:
            std::cerr << "Can't fork: " << strerror(errno) << std::endl;
            break;
        case 0:
            if (execve(argv[0], argv, ENV.data()) == -1) {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        default:
            int exit_code;
            if (waitpid(pid, &exit_code, 0) == EXIT_FAILURE) {
                std::cerr << "Execution failed: " << strerror(errno) << std::endl;
            } else {
                std::cout << "Executed. Return code: " << WEXITSTATUS(exit_code) << std::endl;
            }
    }
}


void run(const std::string& command) {
    auto args = parse_command(command);
    if (args.empty()) {
        return;
    }
    if (args[0] == "set") {
        for (size_t i = 1; i < args.size(); ++i) {
            set_env_var(args[i]);
        }
    } else if (args[0] == "unset") {
        for (size_t i = 1; i < args.size(); ++i) {
            unset_env_var(args[i]);
        }
    } else {
        execute(args);
    }
}

int main() {
  while (true) {
    std::cout << "$ ";
    std::cout.flush();
    std::string command;
    getline(std::cin, command);
    if (std::cin.eof() || command == "exit") {
      break;
    }
    run(command);
  }
}
