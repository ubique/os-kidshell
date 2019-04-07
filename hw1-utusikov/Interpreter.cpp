#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <sstream>

#include "Enviroment.h"
#include "Program.h"

void print_info () {
    //Information on start
    std::cerr << std::endl;
    std::cerr << "It's a simple konsole." << std::endl;
    std::cerr << "If you want to known more then use \"help\"." << std::endl;
    std::cerr << std::endl;

}

void print_help () {
    //list of command with args
    std::cerr << std::endl;
    std::cerr << "Use \"help\" for print info about supported commands." << std::endl;
    std::cerr << "Use \"run name_of_program args_of_program\" to excute a program which called name_of_program and takes args_of_program." << std::endl;
    std::cerr << R"(Use "export env_var" to export environment variable. Format of "env_var" is "VARIABLE=VALUE".)" << std::endl;
    std::cerr << "Use \"unset env_var\" to delete env_var." << std::endl;
    std::cerr << "Use \"envs\" to print all variables." << std::endl;
    std::cerr << "Use \"exit\" to close Interpreter." << std::endl;
    std::cerr << std::endl;
}

std::vector<std::string> split(const std::string& str) { 
    std::vector<std::string> tokens;
    std::stringstream stream;
    stream << str;
    std::string tmp;
    while(stream >> tmp) {
        tokens.push_back(tmp);
    }
    return tokens;
}

int execute_program(std::vector<std::string> args, Enviroment enviroment) {
    pid_t pid = fork();
    int code = -1;
    if (pid == -1) {
        std::cerr << "Cann't create a new process. \"fork\" failed." << std::endl;
    } else if (pid == 0) {
        Program program(args[0], args, enviroment.get_variables());
        if (execve(program.get_name(), program.get_args(), program.get_envs()) < 0) {
         std::cerr << "Executing of program was failed" << std::endl;
         exit(EXIT_FAILURE);   
        }
    } else if (pid > 0) {
        int status = 0;
        pid_t p = waitpid(pid, &status, 0);
        if (p > 0) {
            if (WIFEXITED(status)) {
                code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                std::cerr << "Program was terminated by signal with code " << WTERMSIG(status) << std::endl;
            } else if (WCOREDUMP(status)) {
                std::cerr << "Program generated a error in core of system." << std::endl;
            } else { //process was stoped
                std::cerr << "Process was stopped by signal with code " << WSTOPSIG(status) << std::endl;
            }
        } else {
            std::cerr << "Program was terminated right away." << std::endl;
        }

    }
    return code;
}

void loop (char** env) {
    std::string line;
    Enviroment env_vars;
    for(size_t i = 0; env[i] != nullptr; i++) {
        env_vars.add_variable(env[i]);
    }
    while(std::getline(std::cin, line)) {
        std::vector<std::string> args = split(line);
        if (args.size() == 0) {
            continue;
        }
        const std::string command(args[0]);
        if (command == "exit") {
            return;
        } else if (command == "help") {
            if(args.size() == 1) {
                print_help();
            } else {
                std::cerr << "Command \"help\" doesn't expected any arguments." << std::endl;
            }
        } else if (command == "export") {
            if (args.size() == 2) {
                if (std::count(args[1].begin(), args[1].end(), '=') == 1) {
                    env_vars.add_variable(args[1]);
                } else {
                    std::cerr << R"(Incorrect format of argument for "export". Look "help".)" << std::endl;
                }
            } else {
                std::cerr << R"(Command "export" takes one argument - "VARIABLE=VALUE".)" << std::endl;
            }
        } else if (command == "unset") {
            env_vars.remove_variable(args[1]);
        } else if (command == "envs") {
            for(auto env : env_vars.get_variables()) {
                std::cerr << env.first << "=" << env.second << std::endl;
            }
        } else if (command == "run"){
            if (args.size() == 1) {
                std::cerr << "Program must have a path." << std::endl;
                continue;
            }
            args.erase(args.begin());
            int code = execute_program(args, env_vars);
            if (code >= 0) {
                std::cerr << "Return code of program is " << code << std::endl;
            } else {
                std::cerr << "Program doesn't finished correct(terminated by signal, etc.). Code is -1." << std::endl;
            }
        } else {
            std::cerr << "Unknown command." << std::endl;
        }
    }
}

int main(int argc, char** argv, char** env) {
    print_info();
    (void) argv;
    loop(env);
    return 0;
}