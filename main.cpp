#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <limits>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>

std::map<std::string, std::string> environment;

void execute(char **argv, char **env) {
    pid_t pid = fork();
    if (pid < 0) {            //:(
        std::cout << "Can't create  process" << '\n';
    } else if (pid == 0) {         //child
        if (execve(argv[0], argv, env) == -1) {
            perror("Execution failed");
            exit(-1);
        }
        exit(0);
    } else {         //parent
        int res;
        if (waitpid(pid, &res, 0) == -1) {
            perror("Can't finish process");
        } else {
            std::cout << "Return: " << res << '\n';
        }
    }
}

void add_environment(char *varAndValue) {
    std::string envir(varAndValue);
    if(envir.size()==1){
        std::string variable = envir;
        environment[variable];
        return;
    }
    int indexx = std::numeric_limits<int>::max();
    for (size_t i = 0; i < envir.size(); i++) {
        if (envir[i] == '=') {
            indexx = i;
        }
    }
    if (indexx == std::numeric_limits<int>::max()) {
        std::cout << "Write variables and values in a right format:[variable] = [value]" << "\n";
        return;
    }
    std::string variable = envir.substr(0, indexx);
    std::string name = envir.substr(indexx + 1);
    environment[variable] = name;
}

std::vector<char *> parse(std::string &command) {
    std::stringstream ss;
    ss << command;
    std::string arg;
    std::vector<std::string> args;
    while (ss >> arg) {
        args.push_back(arg);
    }
    std::vector<char *> result;
    for (auto &i : args) {
        char *a = new char[128];
        strcpy(a, i.data());
        result.push_back(a);
    }
    result.push_back(nullptr);
    return result;
}

void executeExport(std::vector<char *> args) {
    if (args.size() == 2) { //output export
        for (auto &i : environment) {
            if(i.second == ""){
                std::cout<<i.first<<"="<<"\n";
                continue;
            }
            std::cout << i.first << "=" << "\"" << i.second << "\"" << "\n";
        }
    } else if (args.size() > 2) { //add anvironment
        for (size_t i = 1; i < args.size()-1; ++i) {
            add_environment(args[i]);
        }
    }
}

void executeUnset(std::vector<char *> args) {
    if (args.size() != 3) {
        std::cout << "Write command in a right format:  unset [variable]" << '\n';
    } else {
        environment.erase(args[1]);
    }
}

void prepare_command(std::string &command) {
    std::vector<char *> args = parse(command);
    if (args.size()==1) { //nothing
        return;
    } else if ((strcmp(args[0], "export") == 0)) { //export
        executeExport(args);
    } else if (strcmp(args[0], "unset") == 0) { //unset
        executeUnset(args);
    } else { //execve
        std::vector<char *> envir;
        for (auto &i : environment) {
            std::string s = i.first + "=" + i.second;
            char *tmp = new char[s.size()+1];
            strcpy(tmp, s.data());
            envir.push_back(tmp);
        }
        envir.push_back(nullptr);
        execute(args.data(), envir.data());
        envir.clear();
    }
    args.clear();
}

void getEnvoronmentVariables() {
    for (char **current = environ; *current; ++current) {
        add_environment(*current);
    }
}

int main(int argc, char const *argv[]) {
    getEnvoronmentVariables();
    while (true) {
        char currentdirectory[128];
        if (getcwd(currentdirectory, 128) == nullptr) {
            perror("Can't get current dir");
            std::cout << ":$ ";
        } else {
            printf("%s: $ ", currentdirectory);
        }
        std::string command;
        std::getline(std::cin, command);
        if (std::cin.eof()||command == "exit") {
            std::cout << "bye" << "\n";
            break;
        }
        prepare_command(command);
    }
}