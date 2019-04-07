#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <cstring>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


static std::map<std::string, std::string> environment;

void printErr(const std::string& message) {
    fprintf(stderr, "ERROR %s: %s\n", message.c_str(), strerror(errno));
}


std::vector<std::string> getPATH() {
    std::vector<std::string> result;
    std::istringstream iss(environment["PATH"]);
    std::string path;

    while (std::getline(iss, path, ':')) {
        result.push_back(path);
    }

    return result;
}

std::string checkExecution(const std::string& command) {
    std::vector<std::string> commandsPaths = getPATH();

    for (std::string& path: commandsPaths) {
        std::string fullPath = path.append("/").append(command);

        if (!access(fullPath.c_str(), 1)) {
            return fullPath;
        }
    }

    return command;

}


std::vector<char*> getCharVector(const std::vector<std::string>& strings) {
    std::vector<char*> result;
    result.reserve(strings.size() + 1);

    for (const std::string& string : strings) {
        result.push_back(strcpy(new char[string.size() + 1], string.data()));
    }

    result.push_back(nullptr);

    return result;
}


std::vector<std::string> parseArgs(const std::string& input) {
    std::istringstream iss(input);

    return std::vector<std::string>{std::istream_iterator<std::string>(iss),
                                    std::istream_iterator<std::string>()};
}


std::pair<std::string, std::string> parseEnvpVariable(const std::string& variable) {
    std::string name;
    std::string value;

    size_t pos = variable.find('=');


    if (pos != std::string::npos) {
        name = variable.substr(0, pos);
        value = variable.substr(pos + 1);
    }

    if (name.empty()) {
        name = variable;
    }

    return std::make_pair(name, value);
}


void exportVariable(const std::string& variable) {
    std::pair<std::string, std::string> existed = parseEnvpVariable(variable);
    auto found = environment.find(existed.first);

    if (found != environment.end()) {
        found->second = existed.second;
    } else {
        environment.insert(existed);
    }
}

void unsetVariable(const std::string& variable) {
    auto found = environment.find(variable);

    if (found != environment.end()) {
        environment.erase(found);
    }
}

void setEnvp(char* envp[]) {
    while (*envp != nullptr) {
        environment.insert(parseEnvpVariable(*envp++));
    }
}

void printEnvp() {
    for (const auto& variable : environment) {
        printf("%s=%s\n", variable.first.c_str(), variable.second.c_str());
    }
}

std::vector<std::string> getEnvp() {
    std::vector<std::string> result;

    std::transform(environment.begin(), environment.end(), std::back_inserter(result),
                   [](const std::pair<std::string, std::string>& p) {
                       return p.first + "=" + p.second;
                   });
    return result;
}


void execute(char* argv[], char* envp[]) {
    pid_t pid = fork();

    if (pid < 0) {
        printErr("Unable to create child process");

    } else if (pid == 0) {
        std::string command = checkExecution(argv[0]);

        if (execve(command.c_str(), argv, envp) == -1) {
            printErr("Execution failed");
            exit(-1);
        }

        exit(0);
    } else {
        int waitingStatus;

        if (waitpid(pid, &waitingStatus, 0) == -1) {
            printErr("Error while waiting");
        } else {
            printf("Process finished with exit code %d\n", WEXITSTATUS(waitingStatus));
        }
    }

}

void run() {
    while (true) {
        printf("$ ");

        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (std::cin.eof() || input == "exit") {
            break;
        }

        std::vector<std::string> args = parseArgs(input);

        if (args[0] == "export") {

            if (args.size() == 1 || (args[1] == "-p" && args.size() == 2)) {
                printEnvp();
            } else if (args.size() >= 2) {
                for (size_t i = 1; i < args.size(); ++i) {
                    exportVariable(args[i]);
                }
            } else {
                printf("Usage: man export\n");
            }

        } else if (args[0] == "unset") {

            if (args.size() < 2) {
                printf("Usage: man unset\n");
            } else {
                for (size_t i = 1; i < args.size(); ++i) {
                    unsetVariable(args[i]);
                }
            }

        } else {
            std::vector<char*> arguments = getCharVector(args);
            std::vector<char*> envp = getCharVector(getEnvp());

            execute(arguments.data(), envp.data());
        }

    }
}

int main(int argc, char* argv[], char* envp[]) {
    setEnvp(envp);
    run();
    return 0;
}