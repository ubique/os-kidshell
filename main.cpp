//
// Created by rbkn99 on 15.04.19.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cerrno>
#include <unistd.h>
#include <memory.h>
#include <wait.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::copy;
using std::string;
using std::vector;
using std::pair;
using std::istringstream;

pair<string, vector<char*>> split_args(string const& raw_str) {
    vector<char*> list_of_args;
    istringstream iss(raw_str);
    string path_str, argv_str;
    iss >> path_str;
    while (iss >> argv_str) {
        argv_str += '0';
        list_of_args.push_back(const_cast<char*>(argv_str.c_str()));
    }
    return std::make_pair(path_str, list_of_args);
}

void log_error(string const& msg) {
    cerr << msg << endl;
    cerr << strerror(errno) << endl;
    cerr.flush();
}

void exec(string const& exec_path, vector<char*> const& args) {
    pid_t pid;
    switch (pid = fork()) {
        case -1: // error
            log_error("fork failed.");
            break;
        case 0: //child subprocess
            if (execv(exec_path.c_str(), args.data()) == -1) {
                log_error("program finished with -1 code.");
                exit(EXIT_FAILURE);
            }
            break;
        default: //parent process
            int child_status;
            if (waitpid(pid, &child_status, 0) == -1) {
                log_error("execution failed.");
            }
            else {
                cout << "execution finished with " << WEXITSTATUS(child_status) << " code." << endl;
            }
            break;
    }
}

int main(int argc, char *argv[]) {
    string raw_str;
    while (true) {
        cout << "$ ";
        cout.flush();
        getline(cin, raw_str);
        if (raw_str == "exit" || cin.eof())
            break;
        auto args = split_args(raw_str);
        exec(args.first, args.second);
    }
    return 0;
}
