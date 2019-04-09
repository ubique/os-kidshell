//
// Created by Павел Пономарев on 2019-03-31.
//

#include "ShellHelper.h"
#include "ParseUtils.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

using command = std::vector<std::string>;

static std::map<std::string, std::string> environment;

void printErrorMessage(std::string const& message) {
    std::cerr << "Error occurred: " << message << std::endl;
}

void printEnvironmentalVars() {
    for (auto const& x : environment) {
        std::cout << x.first
        << '='
        << x.second
        << std::endl;
    }
}

void printShell() {
    std::cout << "Shell$ " << std::flush;
}

void setEnvironmentalVars(command& cmd) {
    for (size_t i = 1; i < cmd.size(); ++i) {
        auto varInfo = ParseUtils::parseEnvironmentalVar(cmd[i]);
        if (varInfo.first.empty()) {
            printErrorMessage(cmd[i] + " not a valid identifier");
            break;
        }
        environment[varInfo.first] = varInfo.second;
    }
}

void setDefaultEnvironmentalVars(char** envp) {
    std::vector<std::string> cur;
    for (int i = 0; *envp != nullptr; ++i) {
        std::string str(*(envp++));
        auto varInfo = ParseUtils::parseEnvironmentalVar(str);
        environment[varInfo.first] = varInfo.second;
    }
}

void unsetEnvironmentalVars(command& cmd) {
    for (size_t i = 1; i < cmd.size(); ++i) {
        environment.erase(cmd[i]);
    }
}

void execute(command& cmd) {
    int status;
    pid_t pid;
    pid = fork();

    if (pid == -1) {
        printErrorMessage("fork failed");
    }

    if (pid == 0) {
        // We are in a child
        std::vector<std::string> paths = ParseUtils::parsePath(environment["PATH"]);
        std::string execCommand = ShellHelper::getCommand(paths, cmd[0]);
        std::vector<char*> arguments = ShellHelper::getCharVector(cmd);
        std::vector<std::string> tempEnv = ShellHelper::getEnvironmentVector(environment);
        std::vector<char*> env = ShellHelper::getCharVector(tempEnv);
        if (execve(execCommand.c_str(), arguments.data(), env.data()) == -1) {
            printErrorMessage("execution failed");
            exit(-1);
        }
    } else {
        // We are in a parent
        if (waitpid(pid, &status, 0) == -1) {
            printErrorMessage("error while executing");
        }
    }
}


void process() {
    std::string str;
    printShell();
    while (std::getline(std::cin, str)) {
        
        command command = ParseUtils::splitString(str);
        
        if (command.empty()) {
            printShell();
            continue;
        }
        if (command[0] == "exit") {
            break;
        }
        if (command[0] == "export") {
            if (command.size() == 1 || (command.size() >= 2 && command[1] == "-p")) {
                printEnvironmentalVars();
            } else {
                setEnvironmentalVars(command);
            }
            printShell();
            continue;
        }
        if (command[0] == "unset") {
            if (command.size() < 2) {
                printErrorMessage("illegal number of arguments. Use unset <var_name> ...");
            } else {
                unsetEnvironmentalVars(command);
            }
            printShell();
            continue;
            
        }
        execute(command);
        printShell();
    }
}

int main(int argc, char* argv[], char* envp[]) {
    setDefaultEnvironmentalVars(envp);
    process();
}
