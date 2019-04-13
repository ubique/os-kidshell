#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <cstring>

struct Command {
    std::string path;
    std::vector<std::string> args;
};

std::map<std::string, std::string> env;

void print_err(const std::string &message) {
    std::cerr << std::endl << "\033[31m" << message;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << "\033[0m" << std::endl;
}

void print_help() {
    std::cout << "<path> [<arg>] - run the program on the path <path> with args [<arg>]"
              << std::endl << "env - print environment variables"
              << std::endl << "export <variable>=<value>|<variable_without_value> [<variable>=<value>|<variable_without_value] - add environment variables"
              << std::endl << "unset <variable> [<variable>] - remove environment variables"
              << std::endl << "help - print help"
              << std::endl << "exit - close shell" << std::endl;
}

Command parse_command(const std::string &input) {
    std::vector<std::string> res;
    auto i = input.begin();
    while (i != input.end()) {
        auto l = std::find_if(i, input.end(), [](char c) { return !std::isspace(c); });

        auto r = input.end();

        if (*l == '\"') {
            ++l;
            r = std::find(l, input.end(), '\"');
        }

        if (r == input.end()) {
            r = std::find_if(l, input.end(), [](char c) { return std::isspace(c); });
        }

        if (l != input.end()) {
            res.emplace_back(l, r);
        }
        i = r;
    }

    Command command = Command();
    if (!res.empty()) {
        command.path = res[0];
        command.args.resize(res.size() - 1);
        std::copy(res.begin() + 1, res.end(), command.args.begin());
    }

    return command;
}

bool file_exist(const std::string &s) {
    struct stat buffer{};
    return stat(s.c_str(), &buffer) != -1;
}

std::vector<std::string> get_env() {
    std::vector<std::string> result;
    result.reserve(env.size());
    for (auto &e : env) {
        result.push_back(e.first + "=" + e.second);
    }

    return result;
}

void print_vars(std::vector<std::string> &vars) {
    for (auto &e : vars) {
        std::cout << e << std::endl;
    }
}

std::vector<char *> get_data(std::vector<std::string> &smth, const std::string &filename = "") {
    std::vector<char *> res;
    if (!filename.empty()) {
        res.push_back(const_cast<char *> (filename.c_str()));
    }
    res.reserve(smth.size());
    for (auto &s : smth) {
        res.push_back(const_cast<char *> (s.c_str()));
    }

    res.push_back(nullptr);

    return res;
}

void run(Command &command) {

    if (command.path == "export") {
        if (command.args.empty()) {
            print_err("Expects: export <variable>=<value>|<variable_without_value> [<variable>=<value>|<variable_without_value]");
            return;
        }

        for (auto &s : command.args) {
            auto m = std::find(s.begin(), s.end(), '=');
            if (m != s.end()) {
                std::string val = std::string(m + 1, s.end());
                if (!val.empty() && (val.front() == '\'' || val.front() == '\"') && val.front() == val.back()) {
                    val = val.substr(1, val.size() - 2);
                }
                env[std::string(s.begin(), m)] = val;
            } else {
                env[s] = "";
            }
        }
        return;
    }

    if (command.path == "unset") {
        if (command.args.empty()) {
            print_err("Expects: unset <variable> [<variable>]");
            return;
        }

        for (auto &s : command.args) {
            env.erase(s);
        }

        return;
    }

    auto vars = get_env();

    if (command.path == "env") {
        print_vars(vars);
        return;
    }

    if (!file_exist(command.path)) {
        errno = 0;
        print_err("Command or file are not exist, use help or check path");
        return;
    }

    auto pid = fork();
    if (pid == -1) {
        print_err("Cannot fork process");
        return;
    }

    if (pid == 0) {
        auto path = command.path;
        if (execve(path.c_str(), get_data(command.args, path).data(), get_data(vars).data()) == -1) {
            print_err("Cannot execute program");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1 || !WIFEXITED(status)) {
            print_err("Error while execution");
        } else {
            std::cout << std::endl << "Ok. Exit code: " << WEXITSTATUS(status) << std::endl;
        }
    }
}

int main() {
    while (true) {
        std::cout << "\033[32m" << "$ " << "\033[0m" << std::flush;
        std::string input;
        std::getline(std::cin, input);

        if (std::cin.eof()) {
            break;
        }

        auto command = parse_command(input);

        if (command.path.empty()) {
            continue;
        }

        if (command.path == "exit") {
            break;
        }

        if (command.path == "help") {
            print_help();
            continue;
        }

        run(command);
    }
}
