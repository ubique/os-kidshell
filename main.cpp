#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>
#include <functional>
#include <cctype>
#include <unordered_map>

using std::string;


void show_help() {
    static const string HELP = "use \"setvar [VAR_NAME]=[VALUE]\" to set environment variable\n"
                               "use \"unsetvar [VAR_NAME]\" to unset environment variable\n"
                               "use \"listvars\" to list all environment variables\n"
                               "use \"[path to program] [arguments]...\" to execute the program with given arguments and environment variables\n";
    std::cout << HELP;
}

std::vector<std::string> split_whitespaces(const std::string &str) {
    std::vector<std::string> res;
    size_t i = 0;
    std::string buffer;
    while (i < str.size()) {
        if (std::isspace(str[i])) {
            if (!buffer.empty()) {
                res.push_back(buffer);
                buffer.clear();
            }
        } else {
            buffer.push_back(str[i]);
        }
        i++;
    }
    if (!buffer.empty()) {
        res.push_back(buffer);
        buffer.clear();
    }
    return res;
}

void set_var(std::unordered_map<string, string> &map, string &var_value) {
    size_t i = 0;
    for (; i < var_value.size() && var_value[i] != '='; ++i);
    if (i == var_value.size() || i ==  0) {
        show_help();
    }
    map.insert({var_value.substr(0, i), var_value.substr(i + 1, var_value.size())});
}

void unset_var(std::unordered_map<string, string> &map, string &var) {
    map.erase(var);
}

void list_vars(std::unordered_map<string, string> &map) {
    for (const auto & p : map) {
        std::cout << p.first << ":" << p.second << std::endl;
    }
}

std::vector<char *> to_c_strings(std::vector<string> & vector) {
    std::vector<char *> res;
    for (auto & str : vector) {
        if (str.back() != '\0') str.push_back('\0');
        res.push_back(&str[0]);
    }
    res.push_back(nullptr);
    return res;
}


int main() {
    show_help();
    std::string input;
    std::unordered_map<string, string> env_vars;
    while (true) {
        std::cout << "kidshell> ";
        std::getline(std::cin, input);
        std::vector<std::string> args = split_whitespaces(input);
        if (args.empty()) continue;
        if (args[0] == "exit") {
            break;
        }
        if (args[0] == "--help") {
            show_help();
            continue;
        }
        if (args.size() == 2) {
            if (args[0] == "setvar") {
                set_var(env_vars, args[1]);
                continue;
            }
            if (args[0] == "unsetvar") {
                unset_var(env_vars, args[1]);
                continue;
            }
        }
        if (args[0] == "listvars") {
            list_vars(env_vars);
            continue;
        }
        //so execute
        pid_t id = fork();
        if (id == -1) {
            std::cout << "Error occurred:" << strerror(errno) << std::endl;
        } else {
            if (id == 0) {
                auto c_args = to_c_strings(args);

                std::vector<string> env;
                env.reserve(env_vars.size());
                for (auto & p : env_vars) {
                    env.push_back(p.first + "=" + p.second);
                }

                auto c_env = to_c_strings(env);
                if (execve(c_args[0], c_args.data(), c_env.data()) == -1) {
                    std::cout << "Error occurred:" << strerror(errno) << std::endl;
                    return -1;
                }
                return 0;
            } else {
                int status;
                if (waitpid(id, &status, 0) == -1) {
                    std::cout << strerror(errno) << std::endl;
                } else {
                    std::cout << "Exit status: " << status << std::endl;
                }
            }
        }
    }
    return 0;
}
