#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;

void resolve_error() {
    int err = errno;
    cout << "Error: " << strerror(err) << endl;
}

vector<string> parse(const string& command) {
    vector<string> ans;
    stringstream ss(command);
    while (!ss.eof()) {
        string temp;
        ss >> temp;
        ans.push_back(temp);
    }
    return ans;
}

void execute(const vector<string>& args) {
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
        vector<string> args = parse(command);
        if (cin.eof() || args[0] == "exit") {
            break;
        }
        const pid_t pid = fork();
        if (pid == -1) {
            resolve_error();
        } else if (!pid) {
            execute(args);
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

