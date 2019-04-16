#include <iostream>
#include <vector>
#include <exception>

#include <unordered_map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

const string welcomeMessage = "KindShell created by Baykaov Vladimir\nGroup M3234\n";
const string helpMessage = "Commands:\nHelp: Shows help\nSet <var_name>=<var_value>: Creates or Rewrites variable\nRemove <var_name>: Removes variable\nVariables: Shows all variables\nExit: Shuts down program\n";
const string unexpectedSetMessage = "Unexpected input\nExpected: Set <var_name>=<var_value>\n";
const string unexpectedRemoveMessage = "Unexpected input\nExpected: Remove <var_name>\n";
const string exitMessage = "Exit program\n";

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
                ++i;
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
    cout << welcomeMessage << helpMessage;
    string command;

    cout << ">> ";

    while (getline(cin, command)) {
        cout.flush();

        vector<string> lineTokens = getTokens(command);

        if (lineTokens.empty()) {
            continue;

        } else if (lineTokens[0] == "Exit" || cin.eof()) {
            cout << exitMessage;
            exit(0);

        } else if (lineTokens[0] == "Help") {
            cout << helpMessage;

        } else if (lineTokens[0] == "Set") {
            if (lineTokens.size() == 2) {
                string varDefinition = lineTokens[1];
                int equalSignPlace = varDefinition.find('=');

                if (equalSignPlace == -1) {
                    cout << unexpectedSetMessage;
                } else {
                    string varName = varDefinition.substr(0, equalSignPlace);
                    string varValue = varDefinition.substr(equalSignPlace + 1, varDefinition.size() - equalSignPlace - 1);

                    environmentVariables[varName] = varValue;
                }

            } else {
                cout << unexpectedSetMessage;
            }

        } else if (lineTokens[0] == "Remove") {
            if (lineTokens.size() == 2) {
                string varName = lineTokens[1];

                environmentVariables.erase(varName);
            } else {
                cout << unexpectedRemoveMessage;
            }

        } else if (lineTokens[0] == "Variables") {
            if (environmentVariables.empty()) {
                cout << "There is on env variables" << endl;
            } else {
                for (auto &variable : environmentVariables) {
                    cout << variable.first << ": " << variable.second << endl;
                }
            }

        } else {
            executeCommand(lineTokens);
        }

        cout << ">> ";
    }
}