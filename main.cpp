#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

using std::vector;
using std::unordered_map;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;

unordered_map<string, string> enviroment;

vector<string> getInput() {
    vector<string> result;

    do {
        string s;
        getline(cin, s);

        std::stringstream stream;
        stream << s;

        string tmp;
        while (stream >> tmp) {
            result.push_back(tmp);
        }
    } while (result.empty() && !cin.eof());
    return result;
}

void execute(vector<string> const& argsv, vector<string> const& envv) {
    vector<char*> args;
    args.reserve(argsv.size() + 1);
    for (const auto &arg : argsv) {
        args.push_back(const_cast<char*>(arg.c_str()));
    }
    args.push_back(nullptr);

    vector<char*> env;
    env.reserve(envv.size() + 1);
    for (const auto& var : envv) {
        env.push_back(const_cast<char*>(var.c_str()));
    }
    env.push_back(nullptr);

    auto pid = fork();

    if (pid == -1) {
        cerr << "Fork failed: " << strerror(errno) << endl;
    } else if (pid == 0) {
        execve(args[0], args.data(), env.data());
        cerr << "Execve failed: " << strerror(errno) << endl;
        _exit(-1);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            cerr << "Wait failed: " << strerror(errno) << endl;
        }

        if (WIFEXITED(status)) {
            cout << "Program finished with exit code " << WEXITSTATUS(status) << endl;
        } else if (WIFSIGNALED(status)) {
            cout << "Program killed by signal " << WTERMSIG(status) << endl;
        } else {
            cout << "Program finished in unknown way" << endl;
        }
    }
}

int main(int argc, char* argv[], char* env[]) {
    //vector<string> enviroment;
    auto ptr = env;
    while (*ptr != nullptr) {
        string tmp = *ptr;
        auto pos = tmp.find('=');
        string key = tmp.substr(0, pos);
        string value = tmp.substr(pos + 1);
        enviroment[key] = value;
        ptr++;
    }

    while (true) {
        cout << "kid_shell$ ";

        auto res = getInput();

        if (res.empty() || res[0] == "exit") {
            return 0;
        } else if (res[0] == "set") {
            if (res.size() != 2 && res.size() != 3) {
                cout << "Usage: set VARIABLE [VALUE]" << endl;
            } else {
                enviroment[res[1]] = (res.size() == 3) ? res[2] : "";
            }
        } else if (res[0] == "unset") {
            if (res.size() != 2) {
                cout << "Usage: unset VARIABLE" << endl;
            } else {
                enviroment.erase(res[1]);
            }
        } else {
            vector<string> tmp_env;
            tmp_env.reserve(enviroment.size());
            for (const auto &entry : enviroment) {
                tmp_env.push_back(entry.first + "=" + entry.second);
            }
            execute(res, tmp_env);
        }
    }
}