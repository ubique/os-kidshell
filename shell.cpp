#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
using namespace std;

void resolve_error() {
    int err = errno;
    cout << "Error: " << strerror(err) << endl;
}

vector<string> parse(const string command) {
    vector<string> ans;
    stringstream ss(command);
    while (!ss.eof()) {
        string temp;
        ss >> temp;
        ans.push_back(temp);
    }
    return ans;
}

void execute(const string command) {
    vector<string> args = parse(command);
    int n = args.size();
    char** c_args = new char*[n + 1];
    for (int i = 0; i < n; ++i) {
        c_args[i] = const_cast<char*>(args[i].data());
    }
    c_args[n] = NULL;
    if (execve(c_args[0], c_args, {NULL}) == -1) {
        delete[] c_args;
        resolve_error();
        exit(-1);
    }
}

int main() {
    while (true) {
        cout << "[" << getlogin() << "] ";
        string command;
        getline(cin, command);
        if (cin.eof() || command == "exit") {
            cout << endl;
            break;
        }
        const pid_t pid = fork();
        if (pid == -1) {
            cout << "Error: cannot fork process" << endl;
        } else if (!pid) {
            execute(command);
        } else {
            int wstatus;
            if (wait(&wstatus) == -1) {
                resolve_error();
            } else {
                cout << "Program exited with code " << WEXITSTATUS(wstatus) << endl;
            }
        }
    }
}

