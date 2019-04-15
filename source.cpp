#include <iostream>
#include <vector>
#include <cstring>
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "errno.h"

using namespace std;

pair <char*, char**> split(string query) {
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
    char* filename = argv[0];
    return {filename, argv};
}

int main() {
    string query, response;
    freopen("input.txt", "r", stdin);
    for (int i = 0; i < 5; i++) {
        getline(cin, query);
        if (query.back() == '\n') query.pop_back();
        response = "";
        const pid_t pid = fork();
        if (pid == -1) {
            cout << "Call failed: " << strerror(errno) << endl;
        } else if (!pid) {
            auto data = split(query);
            char *env[] = {nullptr};
            if (execve(data.first, data.second, env) == -1) {
                response = "Call " + query + " failed: " + strerror(errno);
                cout << response << endl;
                exit(125);
            }
        } else if (pid) {
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 125) {
                    response = "Call " + query + " performed:\n" + "Exit code is " + to_string(WEXITSTATUS(status));
                } else {
                    continue;
                }
            } else {
                response = "Call " + query + "exited abnormally";
            }
            cout << response << endl;
        }
    }

    return 0;
}