#include <iostream>
#include <string>
#include "vector"


#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>


#include "argparser/ArgParser.h"
#include "argparser/ParserException.h"


int EXIT = 0;
int CONTINUE = 1;

std::map<std::string, std::string> env;

/* Built-In */

size_t BUILT_IN_COUNT = 5;

std::string builtin[] = {
        "help",
        "exit",
        "env",
        "export",
        "unset"
};


int builtinHelp(std::vector<std::string>& args) {
    std::cout << "Type 'help' for info\n";
    std::cout << "Type 'exit' for stop terminal\n";
    std::cout << "Type 'env [NAME]' for show all environment variables or show only NAME value\n";
    std::cout << "Type 'export NAME=VALUE' for save VALUE as NAME\n";
    std::cout << "Type 'unset NAME' for remove NAME from variables\n";
    return CONTINUE;
}


void builtinArgsError(const char* name, int count) {
    std::cerr << name << ": Excepted 0 or 1 parameter, found: " << count << ". Please, type help" << std::endl;

}


int builtinExit(std::vector<std::string>& args) {
    std::cout << "Shutdown\n";
    return EXIT;
}


char** getEnv() {
    char** res = new char* [env.size()];
    int idx = 0;
    for (const auto& p: env) {
        res[idx] = new char[p.first.size() + 1 + p.second.size() + 1];
        std::string val = p.first + "=" + p.second;
        strcpy(res[idx], val.c_str());\

    }
    return res;
}


int builtinEnvWatch(std::vector<std::string>& args) {

    if (args.size() == 1) {
        for (const auto& p: env) {
            std::cout << p.first << "=" << p.second << std::endl;
        }
        if (env.empty()) {
            std::cout << "No variables found\n";
        }
    } else if (args.size() == 2) {
        if (env.count(args[1]) != 0) {
            std::cout << args[1] << "=" << env[args[1]] << std::endl;
        } else {
            std::cout << "Not exist\n";
        }
    } else {
        builtinArgsError("env-watch", args.size() - 1);
    }
    return CONTINUE;
}


int builtinExport(std::vector<std::string>& args) {
    if (args.size() != 2) {
        builtinArgsError("export", args.size() - 1);
    } else {
        auto var = args[1];
        auto eq = var.find('=');
        if (eq == std::string::npos) {
            std::cerr << "Wrong argument, type 'help' for more info\n";
            return CONTINUE;
        }

        auto name = var.substr(0, eq);
        auto value = var.substr(eq + 1);

        env[name] = value;

    }
    return CONTINUE;
}


int builtinUnset(std::vector<std::string>& args) {
    if (args.size() != 2) {
        builtinArgsError("unset", args.size() - 1);
    } else {
        if (env.count(args[1]) != 0) {
            env.erase(args[1]);
        }
    }
    return CONTINUE;
}

int (* builtinExecute[])(std::vector<std::string>&) = {
        &builtinHelp,
        &builtinExit,
        &builtinEnvWatch,
        &builtinExport,
        &builtinUnset
};


int exec(std::vector<std::string> args) {
    pid_t pid;
    int status = 0;

    char** callArguments = new char* [args.size() - 1];
    for (int i = 0; i < args.size(); i++) {
        callArguments[i] = new char[args[i].size() + 1];
        strcpy(callArguments[i], args[i].c_str());
    }

    pid = fork();
    if (pid == 0) {
        if (execve(callArguments[0], callArguments, getEnv()) == -1) {
            perror("kidshell@execve");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("kidshell@negpid");
    } else {
        do {
            if(waitpid(pid, &status, WUNTRACED) == -1) {
	        perror("kidshell@waitpid");
	        return 1;
	    }
        } while (WIFEXITED(status) == EXIT_SUCCESS &&
                 WIFSIGNALED(status) == EXIT_SUCCESS); // NOLINT(hicpp-signed-bitwise)
        std::cout << "status: " << status << std::endl;
    }
    return status;
}

void loop() {
    int status;
    do {
        status = 0;
        parser::ArgParser parser;
        std::cout << ">> ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nShutdown\n";
            return;
        }
        try {
            auto args = parser.split(line);

            bool builtinCommand = false;

            for (int i = 0; i < BUILT_IN_COUNT; i++) {
                auto command = builtin[i];
                if (command == args[0]) {
                    int exitCode = builtinExecute[i](args);
                    builtinCommand = true;
                    if (exitCode == EXIT) {
                        return;
                    }
                }
            }
            if (!builtinCommand) {
                exec(args);
            }

        } catch (parser::ParserException& e) {
            std::cout << "Exception while parsing: " << std::endl;
            std::cout << e.what() << std::endl;
            status = 0;
        } catch (int blank) {
            status = 0;
        }
    } while (status >= 0);
}


int main(int argc, char** argv) {


    loop();
    return 0;
}
