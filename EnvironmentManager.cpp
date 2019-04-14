#include "EnvironmentManager.h"


#include <utility>
#include <iostream>

#include <unistd.h>

EnvironmentManager::EnvironmentManager() = default;

EnvironmentManager::EnvironmentManager(char * envp[]) {
    for (size_t i = 0; envp[i] != nullptr; i++) {
        exportVariable(std::string(envp[i]));
    }
}

std::string EnvironmentManager::getExecutable(std::string const& filepath) {
    if (checkExecutable(filepath.c_str())) {
        return filepath;
    }
    for (auto path : paths) {
        path.append('/' + filepath);
        if (checkExecutable(path.c_str())) {
            return path;
        }
    }
    return filepath;
}

void EnvironmentManager::exportVariable(std::string const &var) {
    std::pair<std::string, std::string> keyValue;
    size_t index = var.find('=');
    keyValue = (index == std::string::npos) ? std::make_pair(var, "") :
               std::make_pair(var.substr(0, index), var.substr(index + 1));

    vars.insert_or_assign(keyValue.first, keyValue.second);
    varsString.insert_or_assign(keyValue.first, var);
    if (keyValue.first == "PATH") {
        setPaths(keyValue.second);
    }
}

void EnvironmentManager::unsetVariable(std::string const &var) {
    vars.erase(var);
    varsString.erase(var);
}

void EnvironmentManager::printVariables() {
    for (auto it = varsString.begin(); it != varsString.end(); it++) {
        std::cout << it->second << std::endl;
    }
}

void EnvironmentManager::printValueByKey(std::string const &key) {
    auto entry = vars.find(key);
    if (entry == vars.end()) {
        return;
    }
    std::cout << entry->second << std::endl;
}

std::unique_ptr<char * const[]>  EnvironmentManager::getVariablesArray() {
    std::unique_ptr<char * []> result(new char * [varsString.size() + 1]);
    auto it = varsString.begin();
    for (size_t i = 0; i < varsString.size(); i++, it++) {
        result[i] = it->second.data();
    }
    result[varsString.size()] = nullptr;
    return result;
}

bool EnvironmentManager::checkExecutable(const char *pathname) {
    return access(pathname, X_OK) != -1;
}

void EnvironmentManager::setPaths(std::string const& line) {
    std::istringstream stream(line);
    std::string path;

    paths.clear();

    while (std::getline(stream, path, ':')) {
        paths.push_back(path);
    }
}
