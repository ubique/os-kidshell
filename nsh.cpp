#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>

#include "command.hpp"
#include "env.hpp"

const int EXIT_IGNORE = -1;

void print_banner() {
   std::cout << R"(Welcome to nsh!

Type `help' to get information about the shell.)" << std::endl; 
}

void print_usage() {
    std::cerr << "Usage: nsh [-c command | -h | --help]" << std::endl;
}

void print_help() {
    std::cout << R"(nsh, version 1.1

nsh is Not-a-SHell, simple command interpreter.

SYNTAX

To launch a program, just type path to it:
    /home/user/binary

To pass command line arguments, type them after the program name:
    /opt/website/server --host=0.0.0.0

To use space inside path or argument, escape it with a backslash:
    /home/user/My\ Travel\ Blog/program welcome

To use backslash, type two backslashes:
    /bin/echo \\

INTERNAL COMMANDS

nsh provides four commands:
    set name value  Set environment variable
    unset name      Unset environment variable
    help            Displays this help message
    exit            Quits the shell

COMMAND LINE INTERFACE

When running without arguments, interactive shell is launched.

You can use these arguments:
    -h, --help    Displays this help message
    -c <command>  Runs provided command and returns its exit code)" << std::endl;
}

void print_error(const char* reason) {
    std::cerr << "nsh: " << reason;
    if (errno) {
        std::cerr << ": " << std::strerror(errno);
    }
    std::cerr << std::endl;
}

int invoke(const Command& command, const Env& env) {
    pid_t pid = fork();

    if (pid == -1) {
        print_error("cannot fork process");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        execve(command.name_chars(), command.argv(), env.get_raw());

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

int process(const std::string& command_line, Env& env) {
    Command command;
    command.parse(command_line);

    if (command.argc() == 0) {
        return EXIT_IGNORE;
    } 
    
    if (command.name() == "help") {
        if (command.argc() == 1) {
            print_help();
        } else {
            print_error("help: invalid arguments");
        }
        return EXIT_IGNORE;
    } else if (command.name() == "exit") {
        if (command.argc() == 1) {
            exit(EXIT_SUCCESS);
        } else {
            print_error("exit: invalid arguments");
            return EXIT_IGNORE;
        }
    } else if (command.name() == "set") {
        if (command.argc() == 3) {
            env.set(command.argv()[1], command.argv()[2]);
        } else {
            print_error("set: invalid arguments");
        }
        return EXIT_IGNORE;
    } else if (command.name() == "unset") {
        if (command.argc() == 2) {
            env.unset(command.argv()[1]);
        } else {
            print_error("unset: invalid arguments");
        }
        return EXIT_IGNORE;
    } else {
        command.commit();
        return invoke(command, env);
    }
}

void interactive(Env& env) {
    print_banner();

    while (true) {
        std::string command_line;

        std::cout << "> " << std::flush;
        if (std::getline(std::cin, command_line)) {
            int exit_code = process(command_line, env);
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
    Env env(envp);
    if (argc <= 1) {
        interactive(env);
        return EXIT_SUCCESS;
    } else {
        std::string arg = argv[1];
        if (arg == "-c") {
            if (argc == 2) {
                print_error("command was not specified");
            } else if (argc == 3) {
                return process(std::string(argv[2]), env);
            } else {
                print_error("unknown command line arguments");
            }
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            return EXIT_SUCCESS;
        } else {
            print_error("unknown command line arguments");
        }
    }

    print_usage();
    return EXIT_FAILURE;
}
