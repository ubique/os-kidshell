#include <utility>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <wait.h>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
using namespace std;

map<string, string> env;

void printExport() {
    for (auto &i : env)
        cout << i.first << "=\"" << i.second << "\"\n";
}

void addOneExport(const string& a, string b) {
    env[a] = std::move(b);
}

void parseExport(string str) {
    int ind = -1;
    for (int i = 0; i < str.length(); ++i)
        if (str[i] == '=') {
            ind = i;
            break;
        }
    if (ind == -1 || ind == 0) {
        cout << "Wrong format of export's args\n";
        return;
    }
    string first = str.substr(0, ind);
    string second = str.substr(ind + 1, str.length() - 1 - ind);
    addOneExport(first, second);
}

void unset(const string& str) {
    env.erase(str);
}


void processExecve(char **args, char **data) {
    pid_t pid;
    switch (pid = fork()) {
        case -1:
            //error
            perror("fork");
        case 0: {
            //child
            if (execve(args[0], args, data) == -1){
                perror("execve");
                exit(EXIT_FAILURE);
            }
            break;
        }
        default:
            //parent
            int result;
            if (waitpid(pid, &result, 0) == -1) {
                perror("waitpid");
            } else
                cout << "Result: " << WEXITSTATUS(result) << '\n';
            break;
    }
}

int main(int argc, char *argv[]) {
    for (char ** i = environ; *i; ++i)
        parseExport(*i);
    size_t size = 256;
    char dir[size];
    getcwd(dir, size);
    cout << dir << ">> ";
    string request;
    while (getline(cin, request)) {
        if (request == "exit" || cin.eof()) return 0;
        stringstream ss(request);
        string command;
        vector<char *> commands;
        vector <string> args;
        while (ss >> command)
            args.push_back(command);
        for (auto &i: args) {
            char *tmp = new char[128];
            strcpy(tmp, i.data());
            commands.push_back(tmp);
        }
        commands.push_back(nullptr);
        if (commands.empty()) {
            getcwd(dir, size);
            cout << dir << ">> ";
        }
        if (strcmp(commands[0], "export") == 0) {
            switch (commands.size()) {
                case 2:
                    printExport();
                    break;
                case 3:
                    parseExport(commands[1]);
                    break;
                case 4:
                    addOneExport(commands[1], commands[2]);
                    break;
                default:
                    cout << "Wrong format of export\n";
                    break;
            }
            getcwd(dir, size);
            cout << dir << ">> ";
        }
        if (strcmp(commands[0], "unset") == 0) {
            switch (commands.size()) {
                case 3:
                    unset(commands[1]);
                    break;
                default:
                    cout << "Wrong format of unset\n";
                    break;
            }
            getcwd(dir, size);
            cout << dir << ">> ";
        }
        std::vector<char *> envir;
        for (auto &i : env) {
            std::string s = i.first + "=\"" + i.second + "\"";
            char *tmp = new char[128];
            strcpy(tmp, s.data());
            envir.push_back(tmp);
        }
        envir.push_back(nullptr);
        processExecve(commands.data(), envir.data());
        envir.clear();
        commands.clear();
        getcwd(dir, size);
        cout << dir << ">> ";
    }
    return 0;
}
