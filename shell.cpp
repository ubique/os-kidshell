#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <map>

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
using std::pair;
using std::map;

map<string, string> env;

bool check_var(const string& env) {
    return env.find("=") != string::npos;
}

pair<string, string> resolve_var(const string& var) {
    int i = 0;
    while (var[i] != '=') {
        ++i;
    }
    return {var.substr(0, i), var.substr(i + 1)};
}

void export_var(const string& var) {
    if (check_var(var)) {
        auto entry = resolve_var(var);
        auto it = env.find(entry.first);
        if (it != env.end()) {
            it->second = entry.second;
        } else {
            env.insert(entry);
        }
    }
}

void unset_var(const string& var) {
    auto it = env.find(var);
    if (it != env.end()) {
        env.erase(it);
    }
}

void set_env(char* envp[]) {
    while(*envp) {
        env.insert(resolve_var(*envp++));
    }
}

void print_env() {
    for (auto&& e : env) {
        cout << e.first << "=" << e.second << endl;
    }
}

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

    int m = env.size();
    char** env_args = new char*[m + 1];
    env_args[m] = NULL;
    for (auto it = env.begin(); it != env.end(); ++it) {
        env_args[--m] = const_cast<char*>((it->first + " " + it->second).data());
    }

    if (execve(c_args[0], c_args, env_args) == -1) {
        delete[] c_args;
        delete[] env_args;
        resolve_error();
        exit(-1);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    set_env(envp);
    while (true) {
        cout << "[" << getlogin() << "] ";
        string command;
        getline(cin, command);
        if (cin.eof()) {
            break;
        }
        if (command.length() == 0) {
            continue;
        }
        vector<string> args = parse(command);
        if (args[0] == "exit") {
            break;
        }
        if (args[0] == "unset") {
            for (int i = 1; i < args.size(); ++i) {
                unset_var(args[i]);
            }
        } else if (args[0] == "export") {
            if (args.size() < 2) {
                cout << "Usage: man export" << endl;
            } else if (args[1] == "-p") {
                print_env();
            } else {
                for (int i = 1; i < args.size(); ++i) {
                    export_var(args[i]);
                }
            }
        } else {
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
}

