#include <iostream>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>

static std::unordered_map<std::string, std::string> environment_variables;

static const std::string help = "Usage:"
                                "\n\tfull path [args ...] - execute program by path with args;"
                                "\n\tcd DIR - change dir to DIR;"
                                "\n\texport [NAME=VALUE ...] - associate environment variable NAME with VALUE;"
                                "\n\tunset NAME - erase NAME from env vars;"
                                "\n\thelp - show this message;"
                                "\n\texp - show env vars;"
                                "\n\t:q - exit program;";


void show_help() {
    std::cout << help << std::endl;
}

void show_env_vars() {
    for (auto& env_var : environment_variables) {
        std::cout << env_var.first + "=" + env_var.second << std::endl;
    }
}

void get_dir() {
    char cur_dir[200];
    if (getcwd(cur_dir, 190) == nullptr) {
        perror("getcwd");
    }
    printf("Cur directory is %s\n", cur_dir);
}

void add_variable(std::string& env_var) {
    unsigned long index = env_var.find('=');
    if (index == std::string::npos) {
        perror("export");
    }
    environment_variables[env_var.substr(0, index)] = env_var.substr(index + 1);
}

int main(int argc, char* argv[], char* envp[]) {
    show_help();
    get_dir();
    for (char **env = envp; *env != nullptr; ++env) {
        std::string env_var(*env);
        add_variable(env_var);
    }
    std::string command;
    std::cout << ">> ";
    while (std::getline(std::cin, command)) {
        if (command == ":q") {
            break;
        }

        std::vector<std::string> tokens;
        std::istringstream stream(command);
        std::string token;
        while (stream >> token) {
            tokens.push_back(std::move(token));
        }

        if (tokens[0] == "cd") {
            int dir = chdir(tokens[1].data());
            if (dir == -1) {
                perror(tokens[1].data());
            }
            get_dir();
        } else if (tokens[0] == "export") {
            for (int j = 1; j < tokens.size(); ++j) {
                add_variable(tokens[j]);
            }
        } else if (tokens[0] == "unset") {
            for (int j = 1; j < tokens.size(); ++j) {
                environment_variables.erase(tokens[j]);
            }
        } else if (tokens[0] == "help") {
            show_help();
        } else if (tokens[0] == "env") {
            show_env_vars();
        } else {
            int status;
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                const char *filename = tokens[0].data();
                char *args[tokens.size()];
                for (int i = 1; i < tokens.size(); ++i) {
                    args[i - 1] = const_cast<char *>(tokens[i].data());
                }
                args[tokens.size() - 1] = nullptr;

                std::vector<std::string> buffer;
                std::vector<char *> variables;
                variables.reserve(environment_variables.size());
                for (auto& env_var : environment_variables) {
                    buffer.push_back(env_var.first + "=" + env_var.second + '\0');
                    variables.push_back(&buffer.back()[0]);
                }
                variables.push_back(nullptr);

                int err = execve(filename, args, variables.data());
                if (err == -1) {
                    perror("execve");
                    exit(EXIT_FAILURE);
                }
            } else {
                do {
                    pid_t wait_pid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
                    if (wait_pid == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }

                    if (WIFEXITED(status)) {
                        printf("Normal exited, status = %d\n", WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("Was killed by signal %d\n", WTERMSIG(status));
                    } else if (WIFSTOPPED(status)) {
                        printf("Was stopped by signal %d\n", WSTOPSIG(status));
                    } else if (WIFCONTINUED(status)) {
                        printf("Was continued\n");
                    }
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }
        }
        std::cout << ">> ";
    }

    return 0;
}