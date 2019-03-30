#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

std::vector<std::string> split_whitespace(std::string const &str)
{
    std::vector<std::string> terms;
    std::stringstream stream(str);
    std::string current_term;
    while (stream >> current_term) {
        terms.push_back(current_term);
        terms.back() += '\0';
    }
    return terms;
}

void collect_environment(
    std::unordered_map<std::string, std::string> &collector, char *envp[])
{
    for (char **env = envp; *env != nullptr; ++env) {
        std::string envname;
        std::string envmeaning;
        std::string *pointer = &envname;
        for (size_t i = 0; i < strlen(*env); ++i) {
            char ch = (*env)[i];
            if (ch == '=') {
                pointer = &envmeaning;
            } else {
                pointer->push_back(ch);
            }
        }
        collector[envname] = envmeaning;
    }
}

void change_env(std::vector<std::string> const &args,
                std::unordered_map<std::string, std::string> &environment)
{
    std::string name;
    std::string value;
    std::string *pointer = &name;
    for (size_t i = 0; i < args[1].size(); ++i) {
        if (args[1][i] == '\0') {
            break;
        }
        if (args[1][i] == '=') {
            pointer = &value;
        } else {
            pointer->push_back(args[1][i]);
        }
    }
    if (strcmp(&(args[0][0]), "export") == 0) {
        environment[name] = value;
    } else if (strcmp(&(args[0][0]), "unset") == 0) {
        environment.erase(name);
    }
}

void print_env(std::unordered_map<std::string, std::string> const &environemnt)
{
    for (auto &c : environemnt) {
        std::cout << c.first << "=" << c.second << std::endl;
    }
}

void process(char *envp[])
{
    std::unordered_map<std::string, std::string> environment;
    collect_environment(environment, envp);
    while (true) {
        std::cout << "> " << std::flush;
        std::string s;
        getline(std::cin, s);
        if (s.empty()) {
            continue;
        }
        std::vector<std::string> terms = split_whitespace(s);
        if (strcmp(&(terms[0][0]), ":q") == 0) {
            break;
        }
        if (strcmp(&(terms[0][0]), "export") == 0 ||
            strcmp(&(terms[0][0]), "unset") == 0) {
            change_env(terms, environment);
            continue;
        }
        if (strcmp(&(terms[0][0]), "env") == 0) {
            print_env(environment);
            continue;
        }
        const pid_t pid = fork();
        if (pid == -1) {
            std::cout << strerror(errno) << std::endl;
            continue;
        }
        if (pid == 0) {
            std::vector<char *> c_args;
            for (size_t i = 0; i < terms.size(); ++i) {
                c_args.push_back(&(terms[i][0]));
            }
            c_args.push_back(nullptr);
            std::vector<std::string> storage;
            std::vector<char *> c_env;
            for (auto &pair : environment) {
                storage.push_back(pair.first + "=" + pair.second + '\0');
                c_env.push_back(&(storage.back()[0]));
            }
            c_env.push_back(nullptr);
            int result = execve(c_args[0], c_args.data(), c_env.data());
            if (result == -1) {
                std::cout << strerror(errno) << std::endl;
            }
            return;
        } else {
            int status = 0;
            waitpid(pid, &status, 0);
            std::cout << "Process finished with exitcode " << status
                      << std::endl;
        }
    }
}

const std::string greeting =
    R"BLOCK(
Kidshell for ITMO os course.
Available commands:
full_executable_path [args...] -- executes file located at full_executable_path with given args
:q                             -- quits the shell
export NAME=VALUE              -- adds NAME to environment variables with value VALUE
env                            -- lists all current environment variables
unset NAME                     -- unsets environment variable with name of NAME
)BLOCK";

int main(int argc, char *argv[], char *envp[])
{
    std::cout << greeting << std::endl;
    process(envp);
}
