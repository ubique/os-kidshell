#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

vector <string> split(string &s) {
    istringstream split_stream(s);
    vector <string> args((istream_iterator<string>(split_stream)), istream_iterator<string>());
    return args;
}

void invoke(vector <string> args, char *envp[]) {
    vector<char*> c_args(args.size() + 1);
    for (int i = 0; i < args.size(); i++) {
        c_args[i] = const_cast<char*>(args[i].c_str());
    }
    c_args[args.size()] = nullptr;

    pid_t pid = fork();
    if (pid == -1) {
        cerr << "Can't create fork" << endl;
        return;
    } else if (pid == 0) {
        if (execve(c_args[0], c_args.data(), envp) == -1) {
            cerr << "failed :(" << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Can't execute" << endl;
        } else {
            cout << "Return code: " << WEXITSTATUS(status) << '\n';
        }
    }
}

int main(int argc, char *argv[], char *envp[]) {
    string command;
    while (getline(cin, command)) {
        if (command == ":q" || command == "exit") {
            break;
        }
        invoke(split(command), envp);
    }
    return 0;
}