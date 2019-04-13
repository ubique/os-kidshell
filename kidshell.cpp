#include <iostream>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include "Environment.h"

Environment environment;
const std::string MANUAL = "Welcome to kidshell by SP4RK!\n"
        "Available commands: \n"
        "   help - if you want help\n"
        "   exit - to stop working with this program, but why...?\n"
        "   export [name]=[value] - to add new environment variable (you can pass several at once)\n"
        "   unset [name] - to delete environment variable (you can pass several at once)\n"
        "   env - to show all added environment variables\n"
        "Or just type: [path] [arguments] in order to execute file from given path with passed arguments";


void getArgs(std::string& commandString, std::vector<std::string>& arguments) {
    std::istringstream ss(commandString);
    std::string arg;
    while (ss >> arg) {
        arguments.push_back(arg);
    }
}

std::vector<char*> getConvertedArgs(std::vector<std::string>& args) {
    std::vector<char*> arguments;
    for (auto& arg: args) {
        arguments.push_back(&arg[0]);
    }
    arguments.push_back(nullptr);
    return arguments;
}

void execute(std::vector<std::string>& arguments) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Couldn't fork current process: " << strerror(errno) << std::endl;
    } else if (pid == 0) {
        std::vector<char*> convertedArguments = getConvertedArgs(arguments);
        std::vector<std::string> environmentVariables = environment.envToVector();
        std::vector<char*> convertedEnvironmentVars = getConvertedArgs(environmentVariables);
        if (execve(arguments[0].c_str(), convertedArguments.data(), convertedEnvironmentVars.data()) == -1) {
            std::cerr << "Couldn't execute file: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "Error while waiting for child return";
        } else {
            std::cout << "Program finished with code: " << WEXITSTATUS(status) << std::endl;
        }

    }
}

int main() {
    std::string commandString;
    std::cout << MANUAL << std::endl;
    std::cout << "~kidshell ";
    while (getline(std::cin, commandString)) {
        std::vector<std::string> programArguments;
        getArgs(commandString, programArguments);
        if (!programArguments.empty()) {
            if (programArguments[0] == "exit") {
                exit(EXIT_SUCCESS);
            } else if (programArguments[0] == "export") {
                if (programArguments.size() == 1) {
                    std::cout << "Wrong format, try: \\n export [name]=[value] (can be several)" << std::endl;
                    continue;
                }
                for (size_t i = 1; i < programArguments.size(); ++i) {
                    environment.addEnvVar(programArguments[i]);
                }
            } else if (programArguments[0] == "unset") {
                if (programArguments.size() == 1) {
                    std::cout << "Wrong format, try: \\n unset [name] (can be several)" << std::endl;
                    continue;
                }
                for (size_t i = 1; i < programArguments.size(); ++i) {
                    environment.deleteEnvVar(programArguments[i]);
                }
            } else if (programArguments[0] == "env") {
                environment.printEnvVars();
            } else if (programArguments[0] == "help") {
               std::cout << MANUAL << std::endl;
            } else {
                execute(programArguments);
            }
        }
        std::cout << "~kidshell ";
    }
}