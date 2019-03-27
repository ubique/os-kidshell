#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

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
        cout << "Some error occured" << endl;
        exit(0);
    }
}

int main() {
    while (true) {
        cout << "[" << "greeting should be there" << "] ";
        string command;
        getline(cin, command);
        if (cin.eof() || command == "exit") {
            break;
        }
        const pid_t pid = fork();

        if (pid == -1) {
            cout << "Error: cannot fork process" << endl;
        } else if (!pid) {
            execute(command);
        } else {
            wait(NULL);
        }
    }
}

