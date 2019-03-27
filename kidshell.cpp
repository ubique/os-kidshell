#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>

#include <unistd.h>
#include <wait.h>
#include <cstring>

void error(std::string const& cause) {
  std::cerr << "Error! Cause: " << cause << " - " << strerror(errno)
            << std::endl;
}

// ---- enviroment variables sector

static std::map<std::string, std::string> ENV;

std::vector<std::string> get_env_vars() {
  std::vector<std::string> result;
  std::transform(ENV.begin(), ENV.end(), std::back_inserter(result),
                 [](std::pair<std::string, std::string> const& p) {
                   return p.first + "=" + p.second;
                 });
  return result;
}

void print_vars() {
  for (std::string const& x : get_env_vars()) {
    std::cout << x << std::endl;
  }
}

void set_var(std::string const& settlement) {
  for (size_t i = 0; i < settlement.size(); i++) {
    if (settlement[i] == '=') {
      std::string value = settlement.substr(i + 1);
      if ((value.front() == '\'' || value.front() == '\"') &&
          value.back() == value.front())
        value = value.substr(1, value.length() - 2);
      ENV[settlement.substr(0, i)] = value;
      return;
    }
  }
  std::cerr << "No \'=\' in variable set: " << settlement << std::endl;
}

void unset_var(std::string const& var_name) {
  auto var_it = ENV.find(var_name);
  if (var_it != ENV.end()) {
    ENV.erase(var_it);
  }
}

// ---- arguments and executing sector

std::vector<std::string> parse_args(std::string const& command) {
  std::istringstream command_stream(command);
  return std::vector<std::string>{
      std::istream_iterator<std::string>(command_stream),
      std::istream_iterator<std::string>()};
}

char* converter(std::string const& s) { return const_cast<char*>(s.data()); }

std::vector<char*> get_ptrs(std::vector<std::string>& args) {
  std::vector<char*> result;
  std::transform(args.begin(), args.end(), std::back_inserter(result),
                 converter);
  return result;
}

void execute(char* argv[], char* envp[]) {
  switch (auto pid = fork()) {
    case -1:
      error("Can not fork");
      break;
    case 0:
      if (execve(argv[0], argv, envp) == -1) {
        error("Execution failed");
        exit(-1);
      }
      exit(0);
    default:
      int status;
      if (waitpid(pid, &status, 0) == -1) {
        error("Error in execution");
      } else {
        std::cout << "Executed. Return code: " << WEXITSTATUS(status)
                  << std::endl;
      }
  }
}

void pre_execute(std::vector<std::string>& args) {
  auto vars = get_env_vars();
  auto prepared = get_ptrs(args);
  auto enviroment = get_ptrs(vars);
  prepared.push_back(nullptr);
  enviroment.push_back(nullptr);
  execute(prepared.data(), enviroment.data());
}

void run(std::string const& command) {
  auto args = parse_args(command);
  if (args.size() > 0) {
    if (args[0] == "export") {
      if (args.size() == 1) {
        print_vars();
      } else {
        for (size_t i = 1; i < args.size(); i++) {
          set_var(args[i]);
        }
      }
    } else if (args[0] == "unset") {
      if (args.size() != 2) {
        std::cerr << "unset command usage : unset VARNAME" << std::endl;
      } else {
        for (size_t i = 1; i < args.size(); i++) {
          unset_var(args[i]);
        }
      }
    } else {
      pre_execute(args);
    }
  } else {
    pre_execute(args);
  }
}

int main() {
  while (true) {
    std::cout << ">>> ";
    std::cout.flush();
    std::string command;
    getline(std::cin, command);
    if (std::cin.eof() || command == "exit") {
      break;
    }
    run(command);
  }
}
