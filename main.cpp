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
char **envirmas;

void recount() {
    std::vector<char *> vec;
    for (auto &i : env) {
        std::string s = i.first + "=\"" + i.second + "\"";
        char *tmp = new char[128];
        strcpy(tmp, s.data());
        vec.push_back(tmp);
        delete[] tmp;
    }
    vec.push_back(nullptr);
    envirmas = vec.data();
}

void printExport() {
    for (auto &i : env)
        cout << i.first << "=\"" << i.second << "\"\n";
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
    env[first] = second;
}

void unset(string str) {
    env.erase(str);
}

void addOneExport(string a, string b) {
    env[a] = b;
}

int main(int argc, char *argv[]) {
    char **env = environ;
    while (*env) {
        parseExport(*env);
        env++;
    }
    size_t size = 256;
    char dir[size];
    getcwd(dir, size);
    cout << dir << ">> ";
    string request;
    while (getline(cin, request)) {
        if (request == "exit" || cin.eof()) return 0;
        stringstream ss(request);
        string command;
        vector<string> commands;
        while (ss >> command)
            commands.push_back(command);
        if (commands.empty()) {
            getcwd(dir, size);
            cout << dir << ">> ";
            continue;
        }
        if (commands[0] == "export") {
            switch (commands.size()) {
                case 1:
                    printExport();
                    break;
                case 2:
                    parseExport(commands[1]);
                    break;
                case 3:
                    addOneExport(commands[1], commands[2]);
                    break;
                default:
                    cout << "Wrong format of export\n";
                    break;
            }
            getcwd(dir, size);
            cout << dir << ">> ";
            continue;
        }
        if (commands[0] == "unset") {
            switch (commands.size()) {
                case 2:
                    unset(commands[1]);
                    break;
                default:
                    cout << "Wrong format of unset\n";
                    break;
            }
            getcwd(dir, size);
            cout << dir << ">> ";
            continue;
        }
        pid_t pid;
        switch (pid = fork()) {
            case -1:
                //error
                perror("fork");
                exit(1);
            case 0: {
                //child
                vector<const char *> args(commands.size() + 1);
                for (int i = 0; i < commands.size(); ++i)
                    args[i] = commands[i].data();
                args[commands.size()] = nullptr;
                recount();
                if (execve(args[0], const_cast<char *const *>(args.data()), envirmas) == -1) {
                    perror("execve");
                    exit(1);
                }
                break;
            }
            default:
                //parent
                int result;
                if (waitpid(pid, &result, 0) == -1) {
                    perror("waitpid");
                    exit(1);
                } else
                    cout << "Result: " << WEXITSTATUS(result) << '\n';
                break;
        }
        getcwd(dir, size);
        cout << dir << ">> ";
    }
    return 0;
}
