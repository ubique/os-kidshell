#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <unordered_map>

const std::string HELP = R"SEQ(This program is needed for execute files.
Expect arguments: <path> - path to the file and <arguments> - arguments for execute.
Type: 'exit' - for exit this program.
Type: 'help' - to see this list.
Type: 'set' <variable>=<value> (may be more than one) - for set environment variables.
Type: 'unset' <variable> (may be more than one) - for unset environment variables.
Type: 'watch-env' - for watching environment variables
)SEQ";


std::unordered_map<std::string, std::string> env;

std::vector<std::string> split_tokens(std::string s) {
    std::vector<std::string> tokens;

    std::string token;
    for (auto c : s) {
        if (std::isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> get_env_str() {
    std::vector<std::string> ret;

    for (auto i : env) {
        std::string s = i.first + "=" + i.second;
        ret.push_back(s);
    }

    return ret;
}

std::vector<char *> to_vector_char(std::vector<std::string> &v) {
    std::vector<char *> ret;

    for (auto &i : v) {
        ret.push_back(i.data());
    }

    return ret;
}

void error(std::string const &message) {
    std::cout << "Error, " << message << " " << strerror(errno) << std::endl;
}

void execute(std::vector<std::string> tokens) {
    pid_t pid = fork();

    if (pid == 0) { // child
        char path[tokens[0].size() + 1];
        strcpy(path, tokens[0].c_str());

        std::vector<char *> args = to_vector_char(tokens);
        args.emplace_back(nullptr);

        std::vector<std::string> env_str = get_env_str();
        std::vector<char *> vec_env = to_vector_char(env_str);
        vec_env.emplace_back(nullptr);

        if (execve(tokens[0].c_str(), args.data(), vec_env.data()) == -1) {
            error("can't execute file");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) { // bad fork
        error("can't fork");
    } else { //parent
        int status;
        pid_t wait_pid = waitpid(pid, &status, 0);

        if (wait_pid == -1) {
            error("problem this execution");
        } else {
            std::cout << "Exit code: " << WEXITSTATUS(status) << std::endl;
        }
    }
}

//return true if has problems
bool add_env(const std::string &s) {
    bool correct = false;
    std::string name, value;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '=') {
            correct = true;
            name = s.substr(0, i);
            value = s.substr(i + 1);

            if (value.size() > 0 && ((value[0] == '\'' && value[value.size() - 1] == '\'') ||
                (value[0] == '\"' && value[value.size() - 1] == '\"'))) {
                value = value.substr(1, value.size() - 2);
            }
        }
    }

    if (!correct) {
        return true;
    }

    env[name] = value;
    return false;
}

int main() {
    std::cout << HELP << std::endl;

    while (true) {
        std::cout << ">> ";
        std::cout.flush();

        std::string s;
        getline(std::cin, s);

        if (std::cin.eof()) {
            std::cout << "Exit program" << std::endl;
            return 0;
        }

        std::vector<std::string> tokens = split_tokens(s);

        if (tokens.empty()) {
            continue;
        } else if (tokens[0] == "help") {
            std::cout << HELP;
            continue;
        } else if (tokens[0] == "exit") {
            std::cout << "Exit program" << std::endl;
            return 0;
        } else if (tokens[0] == "set") {
            if (tokens.size() == 1) {
                std::cout << "expected <variable>=<value>" << std::endl;
                continue;
            } else {
                for (size_t i = 1; i < tokens.size(); ++i) {
                    bool is_error = add_env(tokens[i]);

                    if (is_error) {
                        std::cout << "Can't add environment variable: " << tokens[i]
                                  << ", expected <variable>=<value>" << std::endl;
                    }
                }
                continue;
            }
        } else if (tokens[0] == "unset") {
            for (size_t i = 1; i < tokens.size(); ++i) {
                if (env.count(tokens[i])) {
                    env.erase(tokens[i]);
                }
            }
            continue;
        } else if (tokens[0] == "watch-env") {
            if (env.empty()) {
                std::cout << "No environment variables" << std::endl;
                continue;
            }
            std::cout << "Environment variables:" << std::endl;
            for (auto &kv : env) {
                std::cout << kv.first << "=" << kv.second << std::endl;
            }
            continue;
        } else {
            execute(tokens);
            continue;
        }
    }
}