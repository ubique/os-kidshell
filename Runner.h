#pragma once
#include <exception>
#include <string>
#include <vector>

#include "Command.h"

class Runner {
public:
    Runner(Command cmd);

    int wait();

    ~Runner();

private:
    pid_t cpid;
    bool exited;
};

