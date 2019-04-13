#include "Environment.h"

void Environment::deleteEnvVar(std::string key) {
    m_envVars.erase(key);
}

std::vector<std::string> Environment::envToVector() {
    std::vector<std::string> variables;
    for (auto env : m_envVars) {
        variables.emplace_back(env.first + "=" + env.second);
    }
    return variables;
}

void Environment::printEnvVars() {
    if (m_envVars.empty()) {
        std::cout << "There are no environment varibales" << std::endl;
        return;
    }
    std::cout << "Environment varibales:" << std::endl;
    for (auto env : m_envVars) {
        std::cout << env.first + "=" + env.second << std::endl;
    }
}

void Environment::addEnvVar(std::string& varString) {
    size_t eqSign = varString.find('=');
    if (eqSign == std::string::npos) {
        m_envVars[varString] = "";
        return;
    }
    std::string varName = varString.substr(0, eqSign);
    std::string varValue = varString.substr(eqSign + 1);
    while ((varValue[0] == '\'' || varValue[0] == '\"') && varValue.front() == varValue.back()) {
        varValue = varValue.substr(1, varValue.size() - 2);
    }
    m_envVars[varName] = varValue;
}
