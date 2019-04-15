#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

std::string USAGE_SEP = " \n\t - ";
std::vector<std::pair<std::string, std::string>> USAGE{
    {"PATH [ARG]*", "run program on PATH with specified arguments"},

    {"set KEY=VALUE [KEY=VALUE]*", "set environment variables"},
    {"unset KEY [KEY]*", "unset environment variables"},
    {"list", "list all environment variables"},

    {"help", "show this help message"},
    {"exit", "exit shell"}
};

class environment {

    std::unordered_map<std::string, std::string> _variables;

public:

    environment() = default;
    environment(environment const &other) = default;

    struct environment_exception : public std::runtime_error {

        environment_exception(std::string const &message) : runtime_error(message) {};

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

    void list(std::ostream &out) {
        out << "=== ENVIRONMENT ===\n";
        for (auto const &var : _variables) {
            out << "\t" << var.first << " = " << var.second << '\n';
        }
    }

};

namespace parser {

    struct parsing_exception : std::runtime_error {

        parsing_exception(std::string const &message) : runtime_error(message) {};

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
        std::string value = arg.substr(index + 1);
        return {name, value};
    }

}

namespace runtime {

    std::string PREFIX = "my@shell:";
    bool RUNNING = true;
    environment ENV;

    struct runtime_exception : public std::runtime_error {

        runtime_exception(std::string const &message) : runtime_error(message) {};

    };

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
            std::for_each(USAGE.begin(), USAGE.end(), [](std::pair<std::string, std::string> const &use) {
                std::cout << use.first << USAGE_SEP << use.second << '\n';
            });
        } else if (args[0] == "exit") {
            RUNNING = false;
        } else {

            // run program

        }
    }

}

int main() {

    while (runtime::RUNNING) {
        std::cout << runtime::PREFIX;
        std::cout.flush();

        std::string command;
        getline(std::cin, command);

        try {
            runtime::process(command);
        } catch (std::runtime_error const &e) {
            std::cerr << e.what() << '\n';
        }
        if (std::cin.eof()) {
            runtime::RUNNING = false;
        }
    }

}
