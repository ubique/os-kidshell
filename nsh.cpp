#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>

#include "command.hpp"
#include "strings.hpp"

const int EXIT_IGNORE = -1;

void print_error(const char* reason) {
    std::cerr << "nsh: " << reason;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << std::endl;
}

int invoke(const Command& command) {
    pid_t pid = fork();

    if (pid == -1) {
        print_error("cannot fork process");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        execvpe(command.name_chars(), command.argv(), command.envp());

        print_error("cannot execute program");
        exit(EXIT_FAILURE);
    } else {
        int status;
        
        if (waitpid(pid, &status, 0) == -1) {
            print_error("fatal: waitpid() failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            std::cerr << "nsh: process got signal " << WTERMSIG(status) << ": " << strsignal(WTERMSIG(status)) << std::endl;
            return 128 + WTERMSIG(status);
        } else {
            print_error("fatal: unknown waitpid() result");
            exit(EXIT_FAILURE);
        }
    }
}

int process(const std::string& command_line, char** envp) {
    Command command;
    command.add_env(envp);
    command.parse(command_line);

    if (command.argc() == 0) {
        return EXIT_IGNORE;
    } 
    
    if (command.name() == "help") {
        if (command.argc() == 1) {
            std::cout << HELP_MESSAGE << std::endl;
            return EXIT_IGNORE;
        } else {
            print_error("help: invalid arguments");
            return EXIT_IGNORE;
        }
    } else if (command.name() == "exit") {
        if (command.argc() == 1) {
            exit(EXIT_SUCCESS);
        } else {
            print_error("exit: invalid arguments");
            return EXIT_IGNORE;
        }
    } else {
        command.commit();
        return invoke(command);
    }
}

void interactive(char **envp) {
    std::cout << WELCOME_MESSAGE << std::endl;

    while (true) {
        std::string command_line;

        std::cout << "> " << std::flush;
        if (std::getline(std::cin, command_line)) {
            int exit_code = process(command_line, envp);
            if (exit_code != EXIT_IGNORE) {
                std::cout << "[" << exit_code << "] ";
            }
        } else if (std::cin.eof()) {
            print_error("EOF found, exiting");
            exit(EXIT_SUCCESS);
        } else if (std::cin.bad()) {
            print_error("fatal: i/o error");
            exit(EXIT_FAILURE);
        } else if (std::cin.fail()) {
            print_error("fatal: couldn't read stdin");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv, char **envp) {
    if (argc <= 1) {
        interactive(envp);
        return EXIT_SUCCESS;
    } else {
        std::string arg = argv[1];
        if (arg == "-c") {
            if (argc == 2) {
                print_error("command was not specified");
                return EXIT_FAILURE;
            } else if (argc == 3) {
                return process(std::string(argv[2]), envp);
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << HELP_MESSAGE << std::endl;
            return EXIT_SUCCESS;
        }
    }

    print_error("unknown command line arguments");
    std::cerr << USAGE_MESSAGE << std::endl;
    return EXIT_FAILURE;
}
