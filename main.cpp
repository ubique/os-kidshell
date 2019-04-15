#include <iostream>
#include <zconf.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <wait.h>
#include <map>

// Created by Artem Ustinov at 15.04.2019

std::map<std::string, std::string> envs;

struct command {
    std::string path;
    std::vector<char *> argv;

    explicit command(const std::vector<std::string> &args) : path(args[0]), argv(args.size() + 1) {
        for (int i = 0; i < args.size(); i++) {
            argv[i] = new char[args[i].size()];
            args[i].copy(argv[i], args[i].size());
        }
        argv.back() = nullptr;
    }

    command() : path(), argv{nullptr} {}

    ~command() {
        for (auto &i : argv) {
            delete[]i;
        }
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

std::vector<char *> env_array() {
    std::vector<char *> res;
    for (auto &env:envs) {
        auto tmp = env.first + "=" + env.second;
        res.push_back(new char[tmp.size()]);
        tmp.copy(res.back(), tmp.size());
    }
    res.push_back(nullptr);
    return res;
}

int execve(command const &cmd) {
    auto envsv = env_array();
    return execve(cmd.path.data(), cmd.argv.data(), envsv.data());
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
            std::cerr << "Execution is stopped with signal " << WTERMSIG(status) << std::endl;
        }
    }
}

void add_envs(char *envp[]) {
    int i = 0;
    while (envp[i] != nullptr) {
        std::string s = envp[i];
        unsigned long pos = s.find('=');
        envs[s.substr(0, pos)] = s.substr(pos + 1, s.size());
        i++;
    }
}

void add_env(const std::string &env) {
    unsigned long pos = env.find('=');
    if (pos == std::string::npos) {
        std::cerr << "Invalid format of exporting variable " << env << std::endl;
        return;
    }
    envs[env.substr(0, pos)] = env.substr(pos + 1, env.size());
}


void erase_env(const std::string &env) {
    envs.erase(env);
}

int main(int argc, char **argv, char **envp) {
    add_envs(envp);
    while (true) {
        std::string s;
        getline(std::cin, s);
        auto cmd = parsecmd(s);
        if (((cmd.path) == "exit") || (std::cin.eof())) {
            break;
        }
        if (cmd.path == "export" && cmd.argv.size() > 2) {
            for (int i = 1; i < cmd.argv.size() - 1; i++) {
                add_env(cmd.argv[i]);
            }
            continue;
        }
        if (cmd.path == "unset" && cmd.argv.size() > 2) {
            for (int i = 1; i < cmd.argv.size() - 1; i++) {
                erase_env(cmd.argv[i]);
            }
            continue;
        }
        run(cmd);
    }
    return 0;
}