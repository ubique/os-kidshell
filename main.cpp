#include <iostream>
#include <zconf.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sys/wait.h>
#include <map>


const static char *BLUE = "\033[1;34m";
const static char *GREEN = "\033[0;32m";
const static char *RED = "\033[1;31m";
const static char *END = "\033[0m";

extern char **environ;
static std::map<std::string, std::string> environment;

void printColorText(const char *color, const char *text) {
    printf("%s%s%s", color, text, END);
}

char *stringToChar(std::string str) {
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

std::vector<char *> vectorStringToChar(std::vector<std::string> &vec) {
    std::vector<char *> res;
    for (const std::string &s : vec) {
        res.push_back(stringToChar(s));
    }
    return res;
}

std::vector<std::string> getEnvironment() {
    std::vector<std::string> ans;
    for (const auto &pair : environment) {
        ans.push_back("declare " + pair.first + "=\"" + pair.second + '"');
    }
    return ans;
}

std::vector<char *> getCharEnvironment() {
    std::vector<std::string> env = getEnvironment();
    return vectorStringToChar(env);
}

void removeEnvironment(std::string k) {
    auto i = environment.find(k);
    if (i != environment.end()) {
        environment.erase(i);
    }
}

void showEnvironment() {
    std::vector<std::string> env = getEnvironment();
    for (const std::string &s : env) {
        std::cout << s << "\n";
    }
}

void addEnvironment(char *env) {
    std::string en(env);
    size_t ind = en.find('=');
    if (ind != std::string::npos) {
        if (ind == en.length() - 1) {
            environment[en.substr(0, ind)] = "";
        } else {
            environment[en.substr(0, ind)] = en.substr(ind + 1, en.length());
        }
    } else {
        printColorText(RED, "Error: couldn't add new environment: not found '=' in");
        printf("%s\n", env);
    }
}

void changeDir(char *dir) {
    int res = chdir(dir);
    if (res) {
        perror("Error: couldn't change directory");
    }
}

void printUserAndTempPath() {
    char *cwd = getcwd(nullptr, 0);
    char *name = std::getenv("USER");
    if (name != nullptr) {
        printColorText(GREEN, name);
    }
    printf(":");
    if (cwd != nullptr) {
        printColorText(BLUE, cwd);
    } else {
        perror("Error: couldn't get current directory");
    }
    printf("$ ");
}

std::vector<char *> parseCommand(std::string &command) {
    std::vector<std::string> vector;
    boost::split(vector, command, [](char ch) { return ch == ' '; });
    return vectorStringToChar(vector);
}

void executeCommand(char *argv[], char *envp[]) {
    switch (pid_t pid = fork()) {
        case -1:
            perror("Error: couldn't create child process");
            break;
        case 0:
            if (execve(argv[0], argv, envp) == -1) {
                perror("Execution error");
                exit(-1);
            }
            exit(0);
        default:
            int res;
            if (waitpid(pid, &res, 0) != -1) {
                printf("\nReturn code: %d\n", res);
            } else {
                perror("Wait error");
            }

    }
}

bool compareStringAndChar(const std::vector<char *> &v, const std::string &s) {
    return s == v[0];
}

void help() {
    printf("  Commands:\n");
    printf("-    cd [dir] : change current directory on dir\n");
    printf("-    export : shows all environment variables and their values\n");
    printf("-    export [a=b]: set environment variables 'a' with value 'b'\n");
    printf("-    exit: exit application\n");
}

void callCommand(std::string &command) {
    std::vector<char *> args = parseCommand(command);
    if (args.empty()) {
        return;
    }
    if (compareStringAndChar(args, "help")) {
        help();
    } else if (compareStringAndChar(args, "cd")) {
        if (args.size() == 2) {
            changeDir(args[1]);
        } else {
            printColorText(RED, "Error: command cd need 1 argument:");
            printf(" 'cd [directory]'\n");
        }
    } else if (compareStringAndChar(args, "export")) {
        if (args.size() == 1) {
            showEnvironment();
        } else if (args.size() == 2) {
            addEnvironment(args[1]);
        } else {
            printColorText(RED, "Error: command export need 0 or 1 argument:");
            printf(" 'export' or 'export [var]=[val]'\n");
        }
    } else if (compareStringAndChar(args, "unset")) {
        if (args.size() != 2) {
            printColorText(RED, "Error: command unset need 1 argument:");
            printf(" 'unset [var]'\n");
            return;
        }
        removeEnvironment(args[1]);
    } else {
        auto env = getCharEnvironment();

        args.push_back(nullptr);
        executeCommand(args.data(), env.data());
        args.pop_back();

        for (auto e : env) {
            delete[] e;
        }
    }
    for (char *arg : args) {
        delete[] arg;
    }

}

int main(int argc, char const *argv[]) {
    for (auto c = environ; *c; ++c) {
        addEnvironment(*c);
    }
    while (true) {
        printUserAndTempPath();
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit" || std::cin.eof()) {
            break;
        } else {
            callCommand(input);
        }
    }
}
