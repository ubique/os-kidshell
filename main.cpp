#include <sstream>
#include <iostream>

#include <sys/wait.h>
#include <unistd.h>

#include <vector>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char *argv[], char *envp[]) {
    string cmd;
    while ((cout << "~$ ") && getline(cin, cmd)) {
        if (cin.eof() || cmd.empty()){
            cout << "Breaking..." << endl;
            break;
        }

        stringstream inputStream(cmd);
        vector<string> args;
        while (inputStream >> cmd) {
            args.push_back(cmd);
        }
        if (args.size() == 0){
            cout << "Empty input" << endl;
            continue;
        }

        vector<char*> argsPointers;
        for (string& i : args) {
            char* str = new char[i.size() + 1];
            strcpy(str, i.data());
            argsPointers.push_back(str);
        }
        argsPointers.push_back(NULL);

        char** correctArgs = argsPointers.data();
        pid_t process = fork();

        if (process == -1) {
            cout << "Cannot create child process" << endl;
        } else if (process == 0) {
            if (execve(correctArgs[0], correctArgs, envp) == -1){
                cout << "Cannot execute program" << endl;
                exit(-1);
            } else {
                exit(0);
            }
        } else {
            int status = 0;
            if (waitpid(process, &status, 0) == -1) {
                cout << "Time limit exceeded" << endl;
            } else {
                cout << "Program returned " << status << '\n';
            }
        }

        for (char*& i : argsPointers){
            delete[] i;
        }
    }
}