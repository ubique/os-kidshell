#include <iostream>
#include <string>
#include <sstream>

#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

std::string USAGE_SEP = "\n\t";
std::vector<std::pair<std::string, std::string>> USAGE{
    {"PATH [ARG]*", "- run program on PATH with specified arguments"},

    {"set KEY=VALUE [KEY=VALUE]*", "- set environment variables"},
    {"unset KEY [KEY]*", "- unset environment variables"},
    {"list", "- list all environment variables"},

    {"help", "- show this help message"},
    {"exit", "- exit shell"}
};

class environment {

    std::unordered_map<std::string, std::string> _variables;

public:

    environment() = default;
    environment(environment const &other) = default;

    struct environment_exception : public std::runtime_error {

        environment_exception(std::string const &message) : runtime_error(message) {}

    };

    void set(std::string const &name, std::string const &value) {
        _variables.insert({name, value});
    }

    void unset(std::string const &name) {
        if (_variables.count(name) != 0) {
            _variables.erase(name);
        } else {
            throw environment_exception("No variable \"" + name + "\" exists");
        }
    }

    std::vector<std::string> get_all() {
        std::vector<std::string> all(_variables.size());
        std::transform(_variables.begin(), _variables.end(), all.begin(),
                       [](std::pair<std::string, std::string> const &var) {
                           return var.first + "=" + var.second;
                       });
        return all;
    }

    void list(std::ostream &out) {
        out << "environment:\n";
        int index = 1;
        for (auto const &var : _variables) {
            out << "[" << index++ << "]\t" << var.first << "=" << var.second << std::endl;
        }
    }

};

namespace parser {

    struct parsing_exception : std::runtime_error {

        parsing_exception(std::string const &message) : runtime_error(message) {}

    };

    std::vector<std::string> parse_command_line(std::string const &command) {
        std::istringstream stream(command);
        std::vector<std::string> args;

        std::string arg;
        while (stream >> arg) {
            args.push_back(arg);
        }

        return args;
    }

    std::pair<std::string, std::string> parse_set(std::string const &arg) {
        auto index = arg.find('=');
        if (index == std::string::npos) {
            throw parsing_exception("Invalid 'set' syntax, usage : " + USAGE[1].first);
        }

        std::string name = arg.substr(0, index);
        if (std::any_of(name.begin(), name.end(), [](char c) {
            return !(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z');
        })) {
            throw parsing_exception("Environment variables should consist only of latin letters");
        }
        std::string value = arg.substr(index + 1);
        return {name, value};
    }

}

namespace runtime {

    std::string _ERROR = "\033[31m";
    std::string _DEFAULT = "\033[0m";

    std::string PREFIX = "shell@";
    bool RUNNING = true;
    environment ENV;

    struct runtime_exception : public std::runtime_error {

        runtime_exception(std::string const &message) : runtime_error(message) {}

    };

    std::vector<char *> convert_to_cstring(std::vector<std::string> const &args) {
        std::vector<char *> cargs;
        std::for_each(args.begin(), args.end(), [&cargs](std::string const &arg) {
            return cargs.push_back(const_cast<char *>(arg.c_str()));
        });
        cargs.push_back(nullptr);
        return cargs;
    }

    void report(std::string const &message, int err = 0) {
        std::cerr << _ERROR << message;
        if (err != 0) {
            std::cerr << std::strerror(errno);
        }
        std::cerr << std::endl << _DEFAULT;
    }

    bool file_exists(std::string const &path) {
        return access(path.c_str(), F_OK) != -1;
    }

    void process(std::string const &command) {
        auto args = parser::parse_command_line(command);

        if (args.size() == 0) {
            return;
        }
        if (args[0] == "set") {
            if (args.size() == 1) {
                throw runtime_exception("Invalid 'set' syntax, usage : " + USAGE[1].first);
            }
            std::for_each(args.begin() + 1, args.end(), [](std::string const &arg) {
                auto nv = parser::parse_set(arg);
                ENV.set(nv.first, nv.second);
            });
        } else if (args[0] == "unset") {
            if (args.size() == 1) {
                throw runtime_exception("Invalid 'unset' syntax, usage : " + USAGE[2].first);
            }
            std::for_each(args.begin() + 1, args.end(), [](std::string const &arg) {
                ENV.unset(arg);
            });
        } else if (args[0] == "list") {
            if (args.size() != 1) {
                throw runtime_exception("Invalid 'list' syntax, expected no arguments");
            }
            ENV.list(std::cout);
        } else if (args[0] == "help") {
            if (args.size() != 1) {
                throw runtime_exception("Invalid 'help' syntax, expected no arguments");
            }
            std::cout << "Kidshell v.1.0.0. Basic shell emulator.\nAvailable commands:\n";
            std::for_each(USAGE.begin(), USAGE.end(), [](std::pair<std::string, std::string> const &use) {
                std::cout << "  " << use.first << USAGE_SEP << use.second << std::endl;
            });
        } else if (args[0] == "exit") {
            RUNNING = false;
        } else {
            if (!file_exists(args[0])) {
                throw runtime_exception("Specified path does not exist");
            }

            auto cargs = convert_to_cstring(args);
            auto cenv = convert_to_cstring(ENV.get_all());

            pid_t pid = fork();
            if (pid == -1) {
                report("Fork failed: ", errno);
            } else if (pid == 0) {
                if (execve(cargs[0], cargs.data(), cenv.data()) == -1) {
                    exit(EXIT_FAILURE);
                } else {
                    exit(EXIT_SUCCESS);
                }
            } else {
                int status;
                if (waitpid(pid, &status, 0) == -1 || !WIFEXITED(status)) {
                    report("Execution failed: ", errno);
                } else {
                    std::cout << "Return code: " << WEXITSTATUS(status) << std::endl;
                }
            }
        }
    }

}

int main(int argc, char *argv[]) {

    std::cout << runtime::_DEFAULT << "call@";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << ' ';
    }
    std::cout << std::endl;

    while (runtime::RUNNING) {
        std::cout << runtime::PREFIX << "me: ";
        std::cout.flush();

        std::string command;
        getline(std::cin, command);

        try {
            runtime::process(command);
        } catch (std::runtime_error const &e) {
            runtime::report(e.what());
        }
        if (std::cin.eof()) {
            std::cout << std::endl;
            runtime::RUNNING = false;
        }
    }

}
