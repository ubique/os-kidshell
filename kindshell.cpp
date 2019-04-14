#include <iostream>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

string welcomeMessage = "KindShell created by Baykaov Vladimir\nGroup M3234\n";
string helpMessage = "Commands:\nHelp: Shows help\nSet <var_name> <var_value>: Creates or Rewrites variable\nVariables: Shows all variables\nExit: Shuts down program\n";
string unexpectedSetMessage = "Unexpected input\nExpected set <var_name> <var_value>\n";
string exitMessage = "Exit program\n";

unordered_map<string, string> environmentVariables;


vector<string> getTokens(string &line) {
    vector<string> result;
    string tmpToken;

    for (auto ch : line) {
        if (isspace(ch)) {
            if (!tmpToken.empty()) {
                result.push_back(tmpToken);
                tmpToken = "";
            }
        } else {
            tmpToken += ch;
        }
    }

    if (!tmpToken.empty()) {
        result.push_back(tmpToken);
    }

    return result;
}


void printErrorMessage(const string &message) {
    cout << message << endl;
}


void executeCommand(vector<string> &tokens) {
    pid_t pid = fork();

    if (pid == -1) {
        printErrorMessage("Error occurred while forking");
    } else {
        if (pid == 0) {
            char *args[tokens.size() + 1];

            for (int i = 0; i < tokens.size(); ++i) {
                args[i] = (char *) tokens[i].c_str();
            }
            args[tokens.size()] = nullptr;

            char *variables[environmentVariables.size() + 1];

            int i = 0;
            for (auto &pair : environmentVariables) {
                variables[i] = (char *) (pair.first + "=" + pair.second).c_str();
            }
            variables[environmentVariables.size()] = nullptr;

            int executeResult = execve(args[0], args, variables);
            if (executeResult == -1) {
                printErrorMessage("Error occurred while executing process given");
                exit(EXIT_FAILURE);
            }

        } else {
            int result;
            pid_t childResult = waitpid(pid, &result, 0);

            if (childResult == -1) {
                printErrorMessage("Error occurred while executing child process");
            } else {
                cout << "Process result: " << WEXITSTATUS(result) << endl;
            }
        }
    }
}


int main(int argc, char *argv[]) {
    cout << welcomeMessage;
    string command;

    cout << ">> ";

    while (getline(cin, command)) {
        cout.flush();

        vector<string> lineTokens = getTokens(command);

        if (lineTokens.empty()) {
            continue;
        }

        if (lineTokens[0] == "Exit" || cin.eof()) {
            cout << exitMessage;
            return 0;
        }

        if (lineTokens[0] == "Help") {
            cout << helpMessage;
            continue;
        }

        if (lineTokens[0] == "Set") {
            if (lineTokens.size() != 3) {
                cout << unexpectedSetMessage;
                continue;
            }
            string varName = lineTokens[1];
            string varValue = lineTokens[2];

            environmentVariables[varName] = varValue;
            continue;
        }

        if (lineTokens[0] == "Variables") {
            for (auto &variable : environmentVariables) {
                cout << variable.first << ": " << variable.second << endl;
            }
            continue;
        }

        executeCommand(lineTokens);

        cout << ">> ";
    }
}