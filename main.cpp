#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>


//std::map<std::string, std::string> envPars;
bool running = true;


void printHelpMessage(){
    std::cout << "some information" << std::endl;
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
}

void execute(std::string const & command, char * env){
    std::vector<std::string> parsedCommand = parseCommand(command);
    char * argv[parsedCommand.size()];
    for (int i = 0; i < parsedCommand.size(); i++){
        argv[i] = const_cast<char *>(parsedCommand[i].data());
    }

    switch(pid_t pid = fork()) {
        case -1:
            std::cout << "cant't fork" << std::endl;
            break;
        case 0: {
            if (execve(argv[0], argv, env) == -1) {
                std::cout << "can't execute";
                exit(-1);
            }
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

void parseShCommand(std::string const & command, char * env){
    if (command[0] == '-'){
        if (command == "-h" || command == "-help"){
            printHelpMessage();
            return;
        }
        else{
            printUnknownCommand(command);
            return;
        }
    }
    execute(command, env);
}

int main(int argc, char * env[]) {
    printHelpMessage();
    while (running){
        std::string command;
        getline(std::cin, command);
        parseShCommand(command, env);
    }
    return 0;
}