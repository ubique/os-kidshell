#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void addEnvVar(std::string&);
    void deleteEnvVar(std::string);
    std::vector<std::string> envToVector();
    void printEnvVars();
private:
    std::unordered_map<std::string, std::string> m_envVars;
};
