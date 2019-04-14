#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <wait.h>


using namespace std;

const static string EXIT = "exit";
const static string HELP = "help";
const static string SET = "set";
const static string REMOVE = "remove";
const static string ENV = "env";
static unordered_map<string, string> env;

void printErrorMsg(const string &msg) {
    cerr << msg << endl;
}

void printMsg(const string &msg) {
    cout << msg << endl;
}

void run(const vector<char *> &variables, const vector<char *> &args) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execve(args[0], args.data(), variables.data()) == -1) {
            printErrorMsg("Error while executing");
            exit(-1);
        }
    } else if (pid > 0) {
        int res;
        if (waitpid(pid, &res, 0) != -1) {
            printMsg("result code: " + to_string(WEXITSTATUS(res)));
        } else {
            printErrorMsg("Error while executing child");
        }
    } else {
        printErrorMsg("Error while forking");
    }
}

void printHello() {
    cout << "Hello, I'm ready" << endl;
}

void printEnv() {
    for (auto &el: env) {
        cout << el.first << '=' << el.second << endl;
    }
}

void setVariable(const string &arg) {
    auto splitterPos = arg.find('=');
    if (splitterPos == string::npos) {
        printErrorMsg("Wrong env command format");
    }
    auto key = arg.substr(0, splitterPos);
    auto val = arg.substr(splitterPos + 1);
    env[key] = val;
}

void unsetVariable(const string &arg) {
    env.erase(arg);
}


void printHelp() {
    const char *message =
            "Supported commands:\n"
            "set [key]=[value] - add environmental variable\n"
            "remove [key] - remove environmental variable\n"
            "env - show all environmental variables\n"
            "[path] [arguments] - execute file with arguments at path";
    cout << message << endl;

}

vector<string> parseInput(const string &input) {
    istringstream ss(input);
    string word;
    vector<string> words;
    while (ss >> word) {
        words.push_back(word);
    }
    return words;

}

vector<char *> convertVariables() {
    vector<char *> res;
    res.reserve(env.size() + 1);
    for (auto &el:env) {
        auto str = el.first + '=' + el.second;
        res.push_back(&(str.front()));
    }
    res.push_back(nullptr);
    return res;
}

vector<char *> convertArgs(vector<string> &args) {
    vector<char *> res;
    res.reserve(args.size() + 1);
    for (auto &el:args) {
        res.push_back(&(el.front()));
    }
    res.push_back(nullptr);
    return res;
}


int main() {
    printHello();
    string line;
    while (getline(cin, line)) {
        if (line == EXIT) {
            break;
        } else if (line == HELP) {
            printHelp();
        } else if (line == ENV) {
            printEnv();
        } else {
            auto input = parseInput(line);
            if (input.size() == 2 && input[0] == SET) {
                setVariable(input[1]);
            } else if (input.size() == 2 && input[0] == REMOVE) {
                unsetVariable(input[1]);
            } else {
                run(convertVariables(), convertArgs(input));
            }
        }
    }
    return 0;
}



