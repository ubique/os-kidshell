#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cerrno>

int main() {
  while (true) {
    std::string command;
    getline (std::cin, command);

    std::vector<std::string> s_args;
    std::string tmp = "";
    int i = 0;
    while (i < command.size()) {
      if (command[i] == ' ') {
        if (tmp != "") {
          s_args.push_back(tmp);
          tmp = "";
        }
      } else {
        tmp += command[i];
      }
      i++;
    }
    if (tmp != "") {
      s_args.push_back(tmp);
      tmp = "";
    }

    char** args = new char*[s_args.size() + 1];
    for(size_t i = 0; i < s_args.size(); i++) {
      args[i] = new char[s_args[i].size() + 1];
      strcpy(args[i], s_args[i].c_str());
    }

    // std::cout << "[interpretator] args counter: " << s_args.size() << std::endl;
    // for (size_t i = 0; (i + 1) < s_args.size(); i++) {
    //   std::cout << "\"" << args[i] << "\"," << ' ';
    // }
    // if (s_args.size() > 0) {
    //   std::cout << "\"" << args[s_args.size() - 1] << "\"";
    // }
    // std::cout << std::endl;

    args[s_args.size()] = NULL;
    pid_t p = fork();
    if (p == 0) {
      execv(args[0], args);
    }

    int code = -1;
    pid_t err_check  = waitpid(p, &code, 0);
    std::cout << "return code: " << code;
    if (err_check == -1) {
      std::cout << std::endl << "Error while starting program: ";
      std::cout << std::strerror(errno);
    }
    std::cout << std::endl;
  }
  return 0;
}