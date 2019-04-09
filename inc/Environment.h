#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string.h>
#include <string>
#include <vector>

#include "Command.h"
#include "ExecArgumentsHolder.h"
#include "RunnerException.hpp"

class Environment {
public:
    void set(std::string expr); 

    bool unset(std::string var); 

    std::vector<std::string> augment(const std::vector<std::string>& add); 

    int run(Command cmd); 

    int run_exec(Command cmd); 
private:
    std::map<std::string, std::string> env;

};
