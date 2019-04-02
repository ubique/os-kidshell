//
// Created by Павел Пономарев on 2019-03-31.
//

#include "ShellHelper.h"
#include <unistd.h>


std::string ShellHelper::getCommand(std::vector<std::string> const& paths, std::string const& file) {
    for (auto const& path: paths) {
        std::string possiblePath = path;
        possiblePath.append("/");
        possiblePath.append(file);
        if (!access(possiblePath.c_str(), 1)) {
            return possiblePath;
        }
    }
    return file;
}

std::vector<char*> ShellHelper::getCharVector(std::vector<std::string>& cmd) {
    std::vector<char*> arguments;
    arguments.reserve(cmd.size());
    for (auto& str: cmd) {
        arguments.push_back(&str.front());
    }
    arguments.push_back(nullptr);

    return arguments;
}

std::vector<std::string> ShellHelper::getEnvironmentVector(std::map<std::string, std::string> const& env) {
    std::vector<std::string> result;
    std::transform(env.begin(), env.end(), std::back_inserter(result),
                   [](std::pair<std::string, std::string> const& p) {
                       return p.first + "=" + p.second;
                   });
    return result;
}






