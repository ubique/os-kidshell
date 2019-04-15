#include <iostream>
#include <vector>
#include <cstring>
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "errno.h"

using namespace std;

pair <int, char**> split(string query) {
    if (query[0] != '/') {
        query.insert(query.begin(), '/');
    }
    string cur = "";
    vector <string> args;
    for (char c : query) {
        if (c == ' ') {
            args.push_back(cur);
            cur = "";
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        args.push_back(cur);
    }
    char** argv = new char*[args.size()];
    for (int i = 0; i < args.size(); i++) {
        argv[i] = new char[args[i].size()];
        for (int j = 0; j < args[i].size(); j++) {
            argv[i][j] = args[i][j];
        }
    }
    return {args.size(), argv};
}

void free_argv_memory(int sz, char** argv) {
    for (int i = 0; i < sz; i++) {
        delete[] argv[i];
    }
    delete[] argv;
}

void print_error(string query) {
    cout << "Call " + query + " failed: " + strerror(errno) << endl;
}

int main() {
    string query, response;
    while (true) {
        getline(cin, query);
        if (query.back() == '\n') query.pop_back();
        response = "";
        auto data = split(query);
        const pid_t pid = fork();
        if (pid == -1) {
            cout << "Call failed: " << strerror(errno) << endl;
        } else if (!pid) {
            char *env[] = {nullptr};
            if (execve(data.second[0], data.second, env) == -1) {
                print_error(query);
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        } else if (pid) {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                print_error(query);
            } else {
                if (WIFEXITED(status)) {
                    response = "Call " + query + " performed:\n" + "Exit code is " + to_string(WEXITSTATUS(status));
                } else {
                    response = "Call " + query + "exited abnormally";
                }
                cout << response << endl;
            }
            free_argv_memory(data.first, data.second);
        }
    }
    return 0;
}