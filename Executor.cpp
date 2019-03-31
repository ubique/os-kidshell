//
// Created by jetbrains on 29.03.19.
//

#include "Executor.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <cstring>

namespace os {
    int Executor::execute(std::string_view string, char *const *env) {
        errno = 0;

        std::vector<std::string> command = split(string.data());

        logger.debug("input splitted to " + std::to_string(command.size()) + " parts");

        if (command.empty()) {
            logger.warn("input is empty");
            return 0;
        }
        if (strcmp(command[0].c_str(), "exit") == 0) {
            return 1;
        }
        if (strcmp(command[0].c_str(), "cd") == 0) {
            return chdir(command[1].c_str());
        }
        int exit_codes_pipe[2];
        if (pipe(exit_codes_pipe) == -1 || fcntl(exit_codes_pipe[0], F_SETFL, O_NONBLOCK) == -1) {
            logger.error("Couldn't start up pipes");
            return -1;
        }

        pid_t pid = fork();

        if (pid == -1) {
            logger.error("Fork failed");
            return -1;
        } else if (pid == 0) {
            logger.info("Child's process created: " + std::to_string(getpid()));
            auto c_command = c_cast(command);
            logger.debug("command: " + std::string(c_command[0]));
            int code = execve(c_command[0], (c_command.data()), env);
            if (code == -1) {
                logger.error("execve returned -1");

                close(exit_codes_pipe[0]);
                int exit_code = -1;
                write(exit_codes_pipe[1], &exit_code, sizeof(exit_code));
                close(exit_codes_pipe[1]);

                return exit_code;
            }
            return 0;
            //returns on success anyway
        } else {
            logger.info("Parent waits for child");
            pid_t child = waitpid(pid, nullptr, 0);
            if (child == -1) {
                logger.error("Parent's not waited for child");
                return -1;
            } else {
                logger.info("Parent's waited for child " + std::to_string(child));

                int exit_code = 0;
                read(exit_codes_pipe[0], &exit_code, sizeof(exit_code));
                return exit_code;
            }
        }
    }

    std::vector<std::string> Executor::split(const char *string) {
        std::vector<std::string> res;
        std::istringstream ss(string);
        std::string tmp;
        while (ss >> tmp) {
            tmp += '\0';
            res.emplace_back(tmp);
        }
        return res;

    }

    std::vector<char *> Executor::c_cast(std::vector<std::string> &string) {
        std::vector<char *> res(string.size() + 1);
        std::transform(std::begin(string), std::end(string), std::begin(res), [](std::string &s) {
            return s.data();
        });
        return res;
    }
}