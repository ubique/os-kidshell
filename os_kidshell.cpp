//
// Created by anastasia on 14.04.19.
//
#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <unordered_map>
#include <wait.h>

std::unordered_map<std::string, std::string> env;

std::vector<char *> getEnvVector() {
    std::vector<std::string> stringEnvVect;
    stringEnvVect.reserve(env.size());
    for (const auto &e: env) {
        stringEnvVect.push_back(e.first + "=" + e.second);
    }
    std::vector<char*> envVect;
    envVect.reserve(stringEnvVect.size());
    for (const std::string& arg : stringEnvVect) {
        envVect.push_back(const_cast<char*>(arg.data()));
    }
    envVect.emplace_back(nullptr);
    return envVect;
}

void execute(std::vector<std::string> args) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Can't fork: " << strerror(errno) << std::endl;
    } else if (pid == 0) {
        std::vector<char*> argsChar;
        argsChar.reserve(args.size());
        for (const std::string& arg : args) {
            argsChar.push_back(const_cast<char*>(arg.data()));
        }
        argsChar.emplace_back(nullptr);
        std::vector<char *> envVector = getEnvVector();

        if (execve(argsChar[0], argsChar.data(), envVector.data()) == -1) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else {
        int exit_code;
        if (waitpid(pid, &exit_code, 0) == EXIT_FAILURE) {
            std::cerr << "Execution failed: " << strerror(errno) << std::endl;
        } else {
            std::cout << "Executed. Return code: " << WEXITSTATUS(exit_code) << std::endl;
        }
    }
}

std::vector<std::string> parseArgs(const std::string &cmd) {
    std::stringstream cmdStream(cmd);
    std::vector<std::string> args;
    std::string s;
    while (cmdStream >> s) {
        args.push_back(s);
    }
    return args;
}

void setEnvVar(const std::string &exp) {
    auto id = exp.find('=');
    if (id == std::string::npos) {
        std::cerr << "Incorrect 'set' command" << strerror(errno) << std::endl;
        return;
    }
    std::string key = exp.substr(0, id);
    std::string value = exp.substr(id + 1);
    env[key] = value;
}

void unsetEnvVar(const std::string &exp) {
    auto pos = env.find(exp);
    if (pos != env.end()) {
        env.erase(pos);
    }
}

void run(std::string cmd) {
    auto args = parseArgs(cmd);
    if (args.empty())
        return;
    if (args[0] == "set") {
        for (size_t i = 1; i < args.size(); ++i) {
            setEnvVar(args[i]);
        }
    } else if (args[0] == "unset") {
        for (size_t i = 1; i < args.size(); ++i) {
            unsetEnvVar(args[i]);
        }
    } else {
        execute(args);
    }
}

int main() {
    while (true) {
        std::cout << "$ ";
        std::cout.flush();
        std::string cmd;
        getline(std::cin, cmd);
        if (std::cin.eof() || cmd == "exit") {
            break;
        }
        run(cmd);
    }
    return 0;
}