#include <iostream>
#include <string.h>
#include <vector>
#include "Program.h"

Program::Program() {
    name = nullptr;
    args = nullptr;
    envs = nullptr;
}

Program::Program(const std::string& name, 
                const std::vector<std::string>& args, 
                const std::unordered_map<std::string, std::string> &envs){
    this->name = new char[name.size()];
    strcpy(this->name, name.c_str());
    this->args = new char*[args.size() + 1];
    this->args[args.size()] = nullptr;
    for(size_t i = 0; i < args.size(); i++) {
        this->args[i] = new char[args[i].size()];
        strcpy(this->args[i], args[i].c_str());
    }
    this->envs = new char*[envs.size() + 1];
    this->envs[envs.size()] = nullptr;
    size_t ind = 0;
    for(const auto &envi : envs)  {
        std::string str(envi.first + "=" + envi.second);
        this->envs[ind] = new char[str.size()];
        strcpy(this->envs[ind], str.c_str());
        ind++;
    }
}

Program::~Program() {
    delete[] name;
    for(size_t i = 0; args[i] != nullptr; i++) {
        delete[] args[i];
    }
    for(size_t i = 0; envs[i] != nullptr; i++) {
        delete[] envs[i];
    }
    delete[] args;
    delete[] envs;
}

char* Program::get_name() {
    return name;
}

char** Program::get_envs() {
    return envs;
}

char** Program::get_args() {
    return args;
}
