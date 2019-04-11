#include <iostream>
#include <vector>
#include <unistd.h>
#include <cerrno>
#include <memory>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>


using namespace std;


class ArgList {
public:
    ArgList() = default;
    explicit ArgList(const string &commandString) : ArgList() {
        for (size_t i = 0; i < commandString.size(); i++) {
            string arg;
            while (i < commandString.size() && commandString[i] != ' ') {
                arg.push_back(commandString[i]);
                i++;
            }
            argv.push_back(new char[arg.size() + 1]);
            copy(arg.begin(), arg.end(), argv.back());
            argv.back()[arg.size()] = '\0';
        }
        argv.push_back(nullptr);
    }


    ~ArgList() {
        for (auto arg : argv) {
            delete[] arg;
        }
    }


    char *const *data() const {
        return argv.data();
    }

private:
    vector<char *> argv;
};


struct Command {
    explicit Command(const string &commandString) : argv(commandString) {
        filename = argv.data()[0];
    }

    string filename;
    ArgList argv;
};


void print_error(const string &message) {
    cerr << message << endl;
    cerr << strerror(errno) << endl;
}


void print_code(int code) {
    cout << "Program exited with code: " << code << endl;
}

void print_signal(int sig) {
    cout << "Program stopped by signal: " << strsignal(sig) << endl;
}


void print_greet() {
    cout << ">>> ";
    cout.flush();
}


void execute(const Command &command, char *envp[]) {
    pid_t pid = fork();
    if (pid == 0) {
        auto path = command.filename.c_str();
        auto argv = command.argv.data();
        if (execve(path, argv, envp) == -1) {
            print_error("Cannot execute \'" + command.filename + "\':");
            exit(EXIT_FAILURE);
        }
    } else if (pid == -1) {
        print_error("Cannot create subprocess.");
    } else {
        int status;
        pid_t result = waitpid(pid, &status, 0);

        if (result == -1) {
            print_error("Error during execution");
        } else if (WIFEXITED(status)) {
            print_code(WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            print_signal(WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            print_signal(WSTOPSIG(status));
        }
    }
}


int main(int argc, char *argv[], char *envp[]) {
    string input;
    print_greet();
    while (getline(cin, input)) {
        Command command(input);
        execute(command, envp);
        print_greet();
    }
    return 0;
}
