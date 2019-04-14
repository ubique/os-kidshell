#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

const int DIR_LEN = 100;

bool supports_color(map<string, string> &envp_map) {
    auto it = envp_map.find("TERM");
    if (it != envp_map.end()) {
        const char *const term[] = {
            "xterm", "xterm-256", "xterm-256color", "vt100",
            "color", "ansi",      "cygwin",         "linux"};
        for (auto &t: term) {
            if(std::strcmp((it->second).c_str(), t) == 0) return true;
        }
    }
    return false;
}

void exit_(vector<char *> &task) {
    if (task.size() == 2) {
        cout << "Wrong number of arguments\nUsage: exit <status>\n";
    } else {
        exit(atoi(task[1]));
    }
}

void cd(vector<char *> &task) {
    if (task.size() == 2) {
        if (chdir("..") < 0) {
            cout << "Cannot change directory to .. : " << strerror(errno) << '\n';
        }
    } else {
        if (chdir(task[1]) < 0) {
            cout << "Cannot change directory to " << task[1] << " : " << strerror(errno) << '\n';
        }
    }
}

void export_(vector<string> &task_str, map<string, string> &envp_map) {
    if (task_str.size() == 1) {
        for (auto &it: envp_map) {
            cout << it.first << "=\"" << it.second << "\"\n";
        }
    } else {
        for (int i = 1; i < task_str.size(); i++) {
            int pos = task_str[i].find('=');
            if (pos > 0) {
                if (pos < task_str[i].size() - 2 && (task_str[i][pos + 1] == '\'' || task_str[i][pos + 1] == '\"') && task_str[i].back() == task_str[i][pos + 1]) {
                    envp_map[task_str[i].substr(0, pos)] = task_str[i].substr(pos + 2, task_str[i].size() - pos - 3);
                } else {
                    envp_map[task_str[i].substr(0, pos)] = task_str[i].substr(pos + 1);
                }
            } else if (pos != 0){
                envp_map[task_str[i]] = "";
            } else {
                cout << "Wrong export argument " << task_str[i] << '\n';
                cout << "Usage: export [<key>[=<value>] ...]\n";
            }
        }
    }
}

void unset(vector<string> &task_str, map<string, string> &envp_map) {
    if (task_str.size() > 1) {
        for (int i = 1; i < task_str.size(); i++) {
            if (envp_map.find(task_str[i]) != envp_map.end()) {
                envp_map.erase(task_str[i]);
            }
        }
    } else {
        cout << "Wrong number of arguments\nUsage: unset <key> ...\n";
    }
}

void execute(vector<char *> &task, vector<char *> &envp) {
    int st;
    pid_t child_pid = fork();
    if (child_pid < 0) {
        cout << "Cannot fork : " << strerror(errno) << '\n';
    } else if (child_pid == 0) {
        if (execve(task[0], task.data(), envp.data()) < 0) {
            cout << "Cannot execute task : " << strerror(errno) << '\n';
            exit(EXIT_FAILURE);
        }
    } else {
        if (waitpid(child_pid, &st , WUNTRACED) < 0) {
            cout << "Error while executing task : " << strerror(errno) << '\n';
        } else {
            cout << "Process termination code: " << WEXITSTATUS(st) << '\n';
        }
    }
}

void get_args(vector<string> &task_str, vector<char *> &task,
              map<string, string> &envp_map, vector<string> &envp_str, vector<char *> &envp) {
    string str, word;
    getline(cin, str);
    if (cin.eof()) {
        cout << '\n';
        exit(EXIT_SUCCESS);
    }
    istringstream ss(str);
    while (ss >> word) {
        task_str.push_back(word);
    }

    task.reserve(task_str.size() + 1);
    for (auto &it: task_str) {
        task.push_back(const_cast<char *>(it.c_str()));
    }
    task.push_back(NULL);

    envp_str.reserve(envp_map.size() + 1);
    for (auto &it: envp_map) {
        envp_str.push_back(it.first + "=" + it.second);
    }

    envp.reserve(envp_str.size() + 1);
    for (auto &it: envp_str) {
        envp.push_back(const_cast<char *>(it.c_str()));
    }
    envp.push_back(NULL);
}

void set_base_envp(map<string, string> &envp) {
    vector<string> envp_base;
    auto ptr = environ;
    while (*ptr != NULL) {
        envp_base.push_back(*ptr);
        ptr++;
    }
    export_(envp_base, envp);
}

int main() {
    map<string, string> envp_map;
    set_base_envp(envp_map);
    bool color = supports_color(envp_map);
    while (true) {
        char dir[DIR_LEN];
        if (color) {
            cout << "\033[1;32m";
        }
        if (getcwd(dir, sizeof(dir)) != NULL) {
            cout << dir;
        }
        cout << "> ";
        if (color) {
            cout << "\033[0;39m";
        }

        vector<char *> task, envp;
        vector<string> task_str, envp_str;
        get_args(task_str, task, envp_map, envp_str, envp);
        if (task.size() > 1) {
            if (strcmp(task[0], "export") == 0) {
                export_(task_str, envp_map);
            } else if (strcmp(task[0], "unset") == 0) {
                unset(task_str, envp_map);
            } else if (strcmp(task[0], "exit") == 0) {
                exit_(task);
            } else if (strcmp(task[0], "cd") == 0) {
                cd(task);
            } else {
                execute(task, envp);
            }
        }
    }
    return 0;
}
