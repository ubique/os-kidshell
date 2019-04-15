#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

const string EXIT = "exit";

void toCharArray(string& str, char*& arr) {
	arr = new char[str.length() + 1];
	for (int i = 0; i < str.length(); i++) {
		arr[i] = str[i];
	}
	arr[str.length()] = '\0';
}

void freeMemory(char * const * arr, int size) {
	for (int i = 0; i < size; i++) {
		delete[] arr[i];
	}
	delete[] arr;
}

void launchProgram(vector <string>& args, char* envp[]) {
	int argsAmount = args.size();
	char** arguments = new char *[argsAmount + 1];

	for (int i = 0; i < argsAmount; i++) {
		toCharArray(args[i], arguments[i]);
		const_cast <char* const> (arguments[i]);
	}
	arguments[argsAmount] = nullptr;
	const_cast <char* const> (arguments[argsAmount]);

	if (execve(arguments[0], arguments, envp) == -1) {
		perror("execve");
		freeMemory(arguments, argsAmount + 1);
		exit(1);
	}

	freeMemory(arguments, argsAmount + 1);
}

void getArguments(string& input, vector<string>& args) {
	int i = 0;
	while (true) {
		if (i == input.length()) {
			args.push_back(input.substr(0, i));
			break;
		}

		if (input[i] == ' ') {
			args.push_back(input.substr(0, i));
			input = input.substr(i + 1);
			i = 0;
		}
		else {
			i++;
		}
	}
}

int main(int argc, char* argv[], char* envp[]) {
	cout << "> ";

	string input;

	while (getline(cin, input)) {
		if (input == EXIT) {
			return 0;
		}

		vector <string> args;

		getArguments(input, args);

		pid_t pid = fork();

		if (pid == -1) {
			perror("fork()");
			break;
		}
		else if (pid == 0) {
			launchProgram(args, envp);
		}
		else if (pid > 0) {
			int status;
			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid");
				break;
			}
			else {
				int exitStatus = WEXITSTATUS(status);
				cout << "Exit status: " << exitStatus << endl;
			}
		}

		cout << endl;
		cout << "> ";
	}
}
