#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>

std::map<std::string, std::string> envPars;
bool running = true;


void printHelpMessage(){
    std::cout << ":help for this message\n"
                 ":exit for exit\n"
                 ":set <name> <val> set par with <name> to <val>\n"
                 ":unset <name> for unset\n"
                 ":unsetall fot clear all env pars" << std::endl;
}

void printUnknownCommand(std::string command){
    std::cout << "Unknown command: " << command << std::endl;
}

std::vector<std::string> parseCommand(std::string const & command){
    int last = 0;
    std::vector<std::string> result;
    for (int i = 0; i < command.size(); i++){
        if (command[i] == ' '){
            if (i > last) {
                result.push_back(command.substr(static_cast<unsigned int>(last), static_cast<unsigned int>(i - last)));
            }
            last = i + 1;
        }
    }
    return result;
}



char ** getEnvs(){
    char * ans[envPars.size()];
    int pos = 0;
    for (std::pair<std::string, std::string> const & it : envPars) {
        ans[pos] = static_cast<char *>(std::malloc((it.first + "=" + it.second).size()));
        std::strcpy(ans[pos], (it.first + "=" + it.second).c_str());
        pos++;
    }
    return ans;
}

void execute(std::string const & command){
    std::vector<std::string> parsedCommand = parseCommand(command);
    char * argv[parsedCommand.size()];
    for (int i = 0; i < parsedCommand.size(); i++){
        std::strcpy(argv[i], (parsedCommand[i]).c_str());
    }

    switch(pid_t pid = fork()) {
        case -1:
            std::cout << "cant't fork" << std::endl;
            break;
        case 0: {
            char ** envs = getEnvs();
            if (execve(argv[0], argv, envs) == -1) {
                std::cout << "can't execute";
            }
            for (int i = 0; i < envPars.size(); i++){
                std::free(envs[i]);
            }
            std::free(envs);
            break;
        }
        default:
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                std::cout << "can't execute\n";
            } else {
                std::cout << "\nreturn code: " << WEXITSTATUS(status) << '\n';
            }
            break;
    }
}

void finish(){
    running = true;
}

void parseShCommand(std::string const & command){
    if (command[0] == ':'){
        if (command == ":help"){
            printHelpMessage();
            return;
        }
        std::vector<std::string> parsed = parseCommand(command);
        if (parsed[0] == ":exit"){
            finish();
            return;
        }
        else if (parsed[0] == ":set"){
            if (parsed.size() == 2){
                envPars[parsed[1]] = "";
            }
            else if (parsed.size() == 3){
                envPars[parsed[2]] = parsed[3];
            }
        }
        else if (parsed[0] == ":unset" && parsed.size() == 2) {
            envPars.erase(parsed[1]);
        }
        else if (parsed[0] == ":unsetall"){
            envPars.clear();
        }
        else{
            printUnknownCommand(command);
            return;
        }
    }
    execute(command);
}

int main() {
    printHelpMessage();
    while (running){
        std::string command;
        getline(std::cin, command);
        parseShCommand(command);
    }
    return 0;
}