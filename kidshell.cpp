#include <iostream>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>

struct Command {

    std::string path;
    std::vector<char *> args;
};

void print_err(const std::string &message) {
    std::cerr << message << std::endl;
}

void print_help() {
    std::cout << "help";
}

Command parse_command(std::string input) {
    std::vector<std::string> res;
    auto i = input.begin();
    while (i != input.end()) {
        auto l = std::find_if(i, input.end(), [](char c) { return !std::isspace(c); });
        auto r = std::find_if(l, input.end(), [](char c) { return std::isspace(c); });
        res.emplace_back(l, r);
        i = r;
    }

    Command command = Command();
    if (!res.empty()) {
        command.path = res[0];
        command.args.resize(res.size() - 1);
        std::transform(res.begin() + 1, res.end(), command.args.begin(),
                       [](std::string s) { return const_cast<char *>(s.c_str()); });
    }

    return command;
}

bool file_exist(const std::string &s) {
    struct stat buffer{};
    return stat(s.c_str(), &buffer) != -1;
}

void run(Command command) {
    if (command.path == "export") {
        // env
        return;
    }

    if (command.path == "unset") {
        // env
        return;
    }

    if (!file_exist(command.path)) {
        print_err("File not exist");
        return;
    }

    auto pid = fork();
    if (pid == -1) {
        print_err("Can't fork process");
        return;
    }

    if (pid == 0) {
        if (execve(command.path.c_str(), command.args.data(), nullptr) == -1) {
            print_err("Error while execve");
            exit(-1);
        }
        exit(0);
    } else {
        int exit_code;
        if (waitpid(pid, &exit_code, 0) == -1) {
            print_err("Error while execution");
        } else {
            std::cout << "Ok. Exit code: " << WEXITSTATUS(exit_code) << std::endl;
        }
    }
}

int main() {
    while (true) {
        std::cout << "$ " << std::flush;
        std::string input;
        std::getline(std::cin, input);

        if (std::cin.eof()) {
            break;
        }

        auto command = parse_command(input);

        if (command.path.empty()) {
            print_err("Specify the path to the executable file");
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
