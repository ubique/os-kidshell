#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <map>
#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include <system_error>
#include <algorithm>

#include "environment.h"
#include "programconditions.h"

int execute_program(const std::string& path,
                    const std::vector<string>& args,
                    const std::map<string, string>& env)
{
    pid_t pid = fork();
    if (!pid) {
        // We are a child
        ProgramConditions conditions(args, env);
        execve(path.c_str(), conditions.get_argv(), conditions.get_envp());
        // This code will only execute if execve has failed
        std::cerr << "Failed to execute the program: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // We are a parent
        int status;
        pid_t result = waitpid(pid, &status, 0);
        if (result > 0) {
            if (WIFEXITED(status)) {
                // terminated normally
                int exit_code = WEXITSTATUS(status);
                return exit_code;
            } else { // WIFSIGNALED(status)
                // terminated due to an uncaught signal
                int signal_number = WTERMSIG(status);
                return -signal_number;
            }
        } else {
            // Process wait failure
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, "Failed to wait for child return");
        }
    } else {
        // Fork failure
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Failed to spawn child process");
    }
}

std::string execution_result(int result)
{
    if (result >= 0) {
        return "Program returned " + std::to_string(result);
    } else {
        return "Uncaught signal " + std::to_string(-result);
    }
}

void display_prompt()
{
    std::cout << "$ " << std::flush;
}

void display_help()
{
    std::cerr << "kidshell - A simple POSIX shell"      << std::endl
              << "Usage: kidshell"                      << std::endl
              << "Built-ins: help, export, unset, exit" << std::endl;
}

std::vector<std::string> parse_args(const std::string& cmd)
{
    std::vector<std::string> args;
    std::istringstream ss(cmd);
    std::string argument;
    while (ss >> argument) {
        args.push_back(argument);
    }
    return args;
}

void run_shell(char *envp[])
{
    Environment environ(envp);

    display_prompt();

    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        auto args = parse_args(cmd);

        if (args.empty()) {
            display_prompt();
            continue;
        }

        const auto tool = args[0];

        if (tool == "help") {
            display_help();
        } else if (tool == "export") {
            // New environmental variable
            if (args.size() != 2 || std::count(args[1].begin(), args[1].end(), '=') > 1) {
                std::cerr << "expected: export VAR[=VALUE]" << std::endl;
            } else {
                environ.set_variable(args[1]);
            }
        } else if (tool == "unset") {
            if (args.size() != 2 || std::count(args[1].begin(), args[1].end(), '=') > 0) {
                std::cerr << "expected: unset VAR" << std::endl;
            } else {
                environ.unset_variable(args[1]);
            }
        } else if (tool == "exit") {
            if (args.size() != 1) {
                std::cerr << "No arguments expected" << std::endl;
            } else {
                return;
            }
        } else {
            auto executable_name = environ.find_executable(tool);
            if (!executable_name.empty()) {
                try {
                    int result = execute_program(executable_name, args, environ.get_variables());
                    std::cout << execution_result(result) << std::endl;
                } catch (std::system_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Cannot find executable '" << tool << "' in PATH" << std::endl;
            }
        }

        display_prompt();
    }
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 1) {
        display_help();
        return EXIT_FAILURE;
    }
    // Arguments are not used yet
    (void) argv;

    run_shell(envp);

    return EXIT_SUCCESS;
}
