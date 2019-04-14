#include "KidShell.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <iostream>
#include <sstream>
#include <memory>


KidShell::KidShell(char *envp[]) : env(envp) {}

void KidShell::run() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::vector<std::string> args(getArgs(line));
        if (args.empty()) {
            continue;
        }
        if (args[0] == "exit") {
            std::cout << "Bye!" << std::endl;
            return;
        }
        if (constainPipes(line)) {
            handleRequestWithPipes(line);
        } else {
            handlePipelessRequest(args);
        }
    }
}

std::vector<std::string> KidShell::getArgs(std::string const& line) {
    std::vector<std::string> args;
    std::istringstream stream(line);
    std::string arg;

    while (stream >> arg) {
        args.push_back(arg);
    }
    return args;
}

std::unique_ptr<char * const[]> KidShell::getSyscallArgs(std::vector<std::string>& args) {
    std::unique_ptr<char *[]> result(new char * [args.size() + 1]);
    args[0] = env.getExecutable(args[0]);

    for (size_t i = 0; i < args.size(); i++) {
        result[i] = args[i].data();
    }
    result[args.size()] = nullptr;
    return result;
}

void KidShell::handlePipelessRequest(std::vector<std::string> &args) {
    if (args[0] == "--help") {
        std::cout << "Nobody will help you" << std::endl;
    } else if (args[0] == "export") {
        handleExport(args);
    } else if (args[0] == "unset") {
        handleUnset(args);
    } else {
        execute(getSyscallArgs(args).get(), env.getVariablesArray().get());
    }

}

void KidShell::handleExport(std::vector<std::string> &args) {
    if (args.size() == 1) {
        env.printVariables();
        return;
    }
    if (args[1] == "--help") {
        std::cout << "export: export [name[=value]] ...] [-v [key ...]]\n"
                     "    Set shell variables.\n"
                     "    If no arguments is provided, prints environment variables.\n"
                     "    Options:\n"
                     "      -v          for each KEY print value if variable with the key exists\n" << std::endl;
        return;
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "-v") {
            for (size_t j = i + 1; j < args.size(); j++) {
                env.printValueByKey(args[j]);
            }
            return;
        }
        env.exportVariable(args[i]);
    }
}

void KidShell::handleUnset(std::vector<std::string> &args) {
    if (args.size() == 1) {
        return;
    }
    if (args[1] == "--help") {
        std::cout << "unset: unset [name ...]\n"
                     "    Unset shell variables.\n"
                     "    \n"
                     "    For each NAME, remove the corresponding variable." << std::endl;
        return;
    }
    for (size_t i = 1; i < args.size(); i++) {
        env.unsetVariable(args[i]);
    }

}

void KidShell::execute(char *const *argv, char *const *envp, bool withExitCode) {
    const pid_t pid = fork();

    if (pid == -1) {
        perror("Could not create child process");

    } else if (pid == 0) {
        const int err = execve(argv[0], argv, envp);

        if (err == -1) {
            perror("Execution failed");
            exit(-1);
        }
    } else {
        int status;
        const pid_t id = waitpid(pid, &status, 0);
        if (withExitCode) {
            std::cout << "Process finished with exit code " << status << std::endl;
        }
        if (id == -1) {
            perror("Execution failed");
        }

    }
}

bool KidShell::constainPipes(std::string const& line) {
    return line.find('|') != std::string::npos;
}

void KidShell::closeFileDescriptor(int fd) {
    if (close(fd) == -1) {
        perror("Error occurred while closing file descriptor");
    }
}

void KidShell::closeAllDescriptors(int fds[], size_t fdNumber) {
    for (size_t i = 0; i < fdNumber; i++) {
        closeFileDescriptor(fds[i]);
    }
}

// at least, to greps together works
// don't try to use quotes!!
void KidShell::handleRequestWithPipes(std::string const &line) {
    auto args = getPipedArgs(line); // arguments for every process
    const size_t pipesNum = args.size() - 1;
    int pipes[pipesNum * 2]; // fd
    for (size_t i = 0; i < pipesNum; i++) {
        if (pipe(pipes + i * 2) == -1) {
            perror("Error occurred while creating pipes");
            for (size_t j = 0; j < i; j++) {
                closeFileDescriptor(pipes[j * 2]);
                closeFileDescriptor(pipes[j * 2 + 1]);
            }
            return;
        }
    }
    std::vector<pid_t> children;
    children.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++) {
        const pid_t pid = fork();
        if (pid == -1) {
            std::cout << "Piped execution failed :(" << std::endl;
            break;

        } else if (pid != 0) {
            children.push_back(pid);
        } else {
            if (i == 0) {
                // if fd is already used, dup2 will close it :)
                // the first process writes to the "end" of the first pipe
                dup2(pipes[1], STDOUT_FILENO);
            } else if (i + 1 == args.size()) {
                // the last process reads from the input of the last pipe
                dup2(pipes[pipesNum * 2 - 2], STDIN_FILENO);
            } else {
                // reads from input
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }

            closeAllDescriptors(pipes, pipesNum * 2);

            execute(getSyscallArgs(args[i]).get(), env.getVariablesArray().get(), i + 1 == args.size());
            // we are here if execve failed
            exit(-1);
        }
    }
    closeAllDescriptors(pipes, pipesNum * 2);
    int status;
    for (auto & c : children) {
        if (waitpid(c, &status, 0) == -1) {
            perror("Waiting for child process was interrupted");
        }
    }
}


std::vector<std::vector<std::string>> KidShell::getPipedArgs(std::string line) {
    size_t index;
    std::string toParse;
    std::vector<std::vector<std::string>> args;
    while ((index = line.find('|')) != std::string::npos) {
        toParse = line.substr(0, index);
        line = line.substr(index + 1);
        args.push_back(getArgs(toParse));
    }
    args.push_back(getArgs(line));
    return args;
}