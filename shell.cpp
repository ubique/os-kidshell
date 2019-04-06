#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <unistd.h>
#include "map"
#include <string>
using namespace std;
static map<string, string> envpt;

const static string SEPARATOR = "=============";

void printHelp(){
   cout<<R"BLOCK(
   util guide :
   [pathname] [arguments ...] - execute file located at given path
   export [name]=[value] - adds new environment variables
   export - view added environment variables
   unset [varnames]  - erase environment variables
   )BLOCK"<<endl;
}

void printHeader() {
    cout << "shell : ";
}

void printSeparator(const string &message) {
    cout << SEPARATOR + message + SEPARATOR << endl;
}

void printWelcomeMessage() {
    cout << "->WELCOME TO TOY INTERPRETER<-" << endl;
}


vector<string> split_line(const string &line) {
    istringstream iss(line);
    vector<string> results(istream_iterator<string>{iss},
                           istream_iterator<string>());
    return results;
};

char **convert(vector<string> &vector) {
    char **arr = new char *[vector.size() + 1];
    arr[vector.size()] = nullptr;
    for (size_t i = 0; i < vector.size(); ++i) {
        arr[i] = const_cast<char *>(vector[i].data());
    }
    return arr;
}

void printErrorMessage(const string &message, int type) {
    cerr << message + " : " << strerror(type) << endl;
}

void printStatus(int status) {
    cout << "exit code : " << WEXITSTATUS(status) << endl;
}

vector<string> getEnvp() {
    vector<string> result;
    result.reserve(envpt.size());
    for (auto& x : envpt) {
        result.push_back(x.first + "=" + x.second);
    };
    return result;
}

void printEnvp() {
    if(envpt.empty()){
        cout<<"no exported environment variables"<<endl;
    }
    for (auto& x : getEnvp()) {
        cout << x << endl;
    }
}

void run(vector<string> args, char **envp) {
    pid_t pid = fork();
    auto ptr = unique_ptr<char *[]>(convert(args));
    auto exec_args = ptr.get();
    if (pid == 0) {
        if (execve(exec_args[0], exec_args,envp) == -1) {
            printErrorMessage("Execution error",errno);
            exit(-1);
        }
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) != -1) {
            printStatus(status);
        } else {
            printErrorMessage("Error occurred during execution",errno);
        }
    } else {
        printErrorMessage("Folk error",errno);
    }
    printSeparator("=====");
}

void setParameter(string const &argument) {
    for (size_t i = 0; i < argument.size(); i++) {
        if (argument[i] == '=') {
            envpt[argument.substr(0, i)] = argument.substr(i + 1);
            return;
        }
    }
    printErrorMessage("Bad argument \'" + argument + "\'",EINVAL);
}

void eraseParameter(string const &var_name) {
    auto itemIterator = envpt.find(var_name);
    if (itemIterator != envpt.end()) {
        envpt.erase(itemIterator);
    }
}
void printHi(){
    using namespace this_thread;     // sleep_for, sleep_until
    using namespace chrono_literals; // ns, us, ms, s, h, etc.
    using chrono::system_clock;
    sleep_for(0.3s);
    printSeparator("HELLO");
    sleep_for(0.3s);
    printWelcomeMessage();
    sleep_for(0.3s);
    printSeparator("=====");
    sleep_for(0.3s);
    printHeader();
}

int main(int i, char **argv, char **env) {
    string line;
    printHi();
    while (getline(cin, line)) {
        vector<string> data = split_line(line);
        if(line == "-help" || line == "") {
         printHelp();
        } else if (data[0] == "export") {
            if (data.size() == 1) {
                printEnvp();
            } else {
                for (int i = 1; i < data.size(); i++) {
                    setParameter(data[i]);
                }
            }
        } else if (data[0] == "unset") {
            if (data.size() < 2) {
               printErrorMessage("unset must contains names of variables",EINVAL);
            } else {
                for (int i = 1; i < data.size(); i++) {
                    eraseParameter(data[i]);
                }
            }
        }  else if (line == ":q") {
            printSeparator("BYE:)");
            return 0;
        } else {
            auto tmp = getEnvp();
            auto envpFull = convert(tmp);
            run(data, envpFull);
        }
        printHeader();
    }
    return 0;
}
