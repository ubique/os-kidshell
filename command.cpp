#include <cstring>
#include "command.hpp"
#include <iostream>

Command::Command(): _argv(), _envp() {
}

Command::~Command() {
    for (char* arg: _argv) {
        delete[] arg;
    }

    for (char* env: _envp) {
        delete[] env;
    }
}

void Command::add_env(char** envp) {
    while (*envp != nullptr) {
        char* env = new char[strlen(*envp)];
        strcpy(env, *envp);
        _envp.push_back(env);
        ++envp;
    }
}

void Command::parse(const std::string& command) {
    std::string arg = "";
    
    bool escaped = false,
         contains_eq = false;

    for (size_t i = 0; i < command.size(); i++) {
        if (escaped) {
            arg += command[i];
            escaped = false;
        } else if (command[i] == '\\') {
            escaped = true;
        } else {
            escaped = false;
            if (command[i] == ' ') {
                if (arg.size() > 0) {
                    add(arg, contains_eq);
                    arg = "";
                    contains_eq = false;
                }   
            } else {
                arg += command[i];
                if (command[i] == '=') {
                    contains_eq = true;
                }
            }
        }
    }
    if (arg.size() > 0) {
        add(arg, contains_eq);
    }
}

void Command::commit() {
    _argv.push_back(nullptr);
    _envp.push_back(nullptr);
}


void Command::add(const std::string &arg, bool contains_eq) {
    char *data = new char[arg.size()];
    strcpy(data, arg.c_str());
    
    if (contains_eq && argc() == 0) {
        _envp.push_back(data);
    } else {
        _argv.push_back(data);
    }
}

size_t Command::argc() const {
    return _argv.size();
}

char* const Command::name_chars() const {
    if (argc() == 0)
        return nullptr;
    return _argv[0];
}

const std::string Command::name() const {
    return _argv[0];
}

char* const* Command::argv() const {
    return &_argv[0];
}

char* const* Command::envp() const {
    return &_envp[0];
}
