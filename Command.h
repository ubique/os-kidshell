#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>

class Command {
public:
    void parse(std::string line);
    
    std::vector<std::string> getArguments();
    std::vector<std::string> getEnvironment();

    friend std::ostream& operator<<(std::ostream& os, const Command& cmd);

private:
    std::vector<std::string> args;
    std::vector<std::string> env;
};

std::ostream& operator<<(std::ostream& os, const Command& cmd);
