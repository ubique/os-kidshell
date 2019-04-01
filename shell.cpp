//
// Created by Anton Shelepov on 2019-04-01.
//
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

using std::cout;
using std::string;
using std::unordered_map;
using std::cin;
using std::endl;
using std::vector;

void greet() {
   cout << "Kid shell created by Anton Shelepov for OS-course." << endl;
   cout << "Type \"--help\" for more information." << endl;
}

std::pair<string, string> divide_var(const char* var) {
    string key, value;
    bool key_build = false;
    for (size_t j = 0; j < strlen(var); j++) {
        if (var[j] == '=') {
            key_build = true;
            continue;
        }

        if (key_build) {
            value.push_back(var[j]);
        } else {
            key.push_back(var[j]);
        }
    }

    return {key, value};
}

unordered_map<string, string> parse_envp(char* envp[]) {
    unordered_map<string, string> environment;
    for (size_t i = 0; envp[i] != nullptr; i++) {

        auto divided_var = divide_var(envp[i]);

        environment[divided_var.first] = divided_var.second;
    }

    return environment;
}

vector<string> split(string const& command) {
    vector<string> res;

    std::istringstream iss(command);
    std::copy(std::istream_iterator<string>(iss),
            std::istream_iterator<string>(),
                    std::back_inserter(res));

    return res;
}

void print_usage() {
    cout << "Available commands:" << endl;
    cout << "\t[filepath] [args..] – executes executable with given filepath and args" << endl;
    cout << "\tenv – lists current environment variables" << endl;
    cout << "\techo [var_name] – displays a single variable with given name" << endl;
    cout << "\texport [var_name]=[var_value] – adds new variable with given name and value" << endl;
    cout << "\tunset [var_name] – unsets a variable with given name" << endl;
    cout << "\texit – terminates the shell" << endl;
}

void print_environment(unordered_map<string, string> const& environment) {
    for (auto& var : environment) {
        cout << var.first << "=" << var.second << endl;
    }
}

void print_var(unordered_map<string, string>& environment, string const& name) {
    string value = environment[name];

    if (value.empty()) {
        cout << "Variable with name " + name + " wasn't set" << endl;
    } else {
        cout << value << endl;
    }
}

void export_var(unordered_map<string, string>& environment, string const& var) {
    auto divided_var = divide_var(var.data());
    string name = divided_var.first, value = divided_var.second;
    if (name.empty()) {
        cout << "Variable name expected" << endl;
        return;
    }
    if (value.empty()) {
        cout << "Variable value expected" << endl;
        return;
    }

    environment[name] = value;
    cout << "Variable " + name + " was set with value " + value << endl;
}

void unset_var(unordered_map<string, string>& environment, string const& name) {
    environment.erase(name);
    cout << "Variable " + name + " was reset" << endl;
}


void print_error(string const& message) {
    cout << message << ": " << strerror(errno) << endl;
}

vector<string> collect_vars(unordered_map<string, string> const& environment) {
    vector<string> res;
    res.reserve(environment.size());
    for (auto& var : environment) {
        res.push_back(var.first + "=" + var.second);
    }

    return res;
}

vector<char*> extract_ptrs(vector<string> const& holder) {
    vector<char*> res;
    res.reserve(holder.size());
    for (auto& str : holder) {
        res.push_back(const_cast<char*>(str.data()));
    }
    return res;
}

void execute(vector<string> const& args, unordered_map<string, string> const& environment) {
    const pid_t pid = fork();
    if (pid == -1) {
        print_error("Fork failed");
        return;
    }
    if (pid == 0) {
        string filename = args[0];
        auto c_args = extract_ptrs(args);
        c_args.push_back(nullptr);

        vector<string> env_holder = collect_vars(environment);
        auto envp = extract_ptrs(env_holder);
        envp.push_back(nullptr);
        if (execve(filename.data(), c_args.data(), envp.data()) == -1) {
            print_error("Execution failed");
            exit(-1);
        }

        exit(0);
    }

    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
        print_error("Execution failed");
    } else {
        cout << "Program finished execution with exit code " << WEXITSTATUS(wstatus) << endl;
    }
}

int process_command(string const& command, unordered_map<string, string>& environment) {
    auto args = split(command);
    if (args.empty()) {
        return 0;
    }
    string command_name = args[0];


    if (command_name == "--help") {
        print_usage();
        return 0;
    }

    if (command_name == "env") {
        print_environment(environment);
        return 0;
    }

    if (command_name == "echo") {
        if (args.size() == 1) {
            cout << "Variable name was expected" << endl;
        } else {
            print_var(environment, args[1]);
        }
        return 0;
    }

    if (command_name == "export") {
        if (args.size() < 2) {
            cout << "Variable name and value were expected" << endl;
        } else {
            export_var(environment, args[1]);
        }
        return 0;
    }

    if (command_name == "unset") {
        if (args.size() < 2) {
            cout << "Variable name was expected" << endl;
        } else {
            unset_var(environment, args[1]);
        }
        return 0;
    }

    if (command_name == "exit") {
        return 1;
    }

    execute(args, environment);
    return 0;
}

int main(int argc, char* argv[], char* envp[]) {
    greet();

    auto environment = parse_envp(envp);
    while (true) {
        cout << ">> ";
        cout.flush();

        string command;
        getline(cin, command);

        if (process_command(command, environment)) {
            break;
        }
    }
}

