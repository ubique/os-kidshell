#include <iostream>
#include <vector>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>

using std::string;

static const std::string help = "Usage:"
                                "\n\tfull path [args ...] - execute program by path with args;"
                                "\n\tcd DIR - change dir to DIR;"
                                "\n\texport [NAME=VALUE ...] - associate environment variable NAME with VALUE;"
                                "\n\tunset NAME - erase NAME from env vars;"
                                "\n\thelp - show this message;"
                                "\n\tenv - show env vars;"
                                "\n\t:q - exit program;";


void show_help() {
    std::cout << help << std::endl;
}

void show_env_vars(std::unordered_map<std::string, std::string>& environment_variables) {
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

void add_variable(std::unordered_map<std::string, std::string>& environment_variables, std::string& env_var) {
    std::string::size_type index = env_var.find('=');
    if (index != std::string::npos && index != env_var.size() - 1) {
        environment_variables[env_var.substr(0, index)] = env_var.substr(index + 1);
    } else {
        std::string value;
        if (index == env_var.size() - 1) {
            environment_variables[env_var.substr(0, index)] = value;
        } else {
            environment_variables[env_var] = value;
        }
    }
}

int main(int argc, char* argv[], char* envp[]) {
    std::unordered_map<std::string, std::string> environment_variables;
    show_help();
    get_dir();
    for (char **env = envp; *env != nullptr; ++env) {
        std::string env_var(*env);
        add_variable(environment_variables, env_var);
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
                add_variable(environment_variables, tokens[j]);
            }
        } else if (tokens[0] == "unset") {
            for (int j = 1; j < tokens.size(); ++j) {
                environment_variables.erase(tokens[j]);
            }
        } else if (tokens[0] == "help") {
            show_help();
        } else if (tokens[0] == "env") {
            show_env_vars(environment_variables);
        } else {
            int status;
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                const char *filename = tokens[0].data();
                std::vector<char *> args;
                args.reserve(tokens.size());
                for (auto& t : tokens) {
                    args.push_back(&(t[0]));
                }
                args.emplace_back(nullptr);

                std::vector<std::string> env;
                env.reserve(environment_variables.size());
                for (auto& p : environment_variables) {
                    env.push_back(p.first + "=" + p.second);
                }

                std::vector<char *> c_env;
                c_env.reserve(env.size());
                for (auto& e : env) {
                    c_env.push_back(&(e[0]));
                }
                c_env.emplace_back(nullptr);

                int err = execve(filename, args.data(), c_env.data());
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