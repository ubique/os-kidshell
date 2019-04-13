#include <cstring>
#include "command.hpp"
#include <iostream>

Command::Command(): _argv() {
}

Command::~Command() {
    for (char* arg: _argv) {
        delete[] arg;
    }
}

void Command::parse(const std::string& command) {
    std::string arg = "";
    
    bool escaped = false;

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
                    add(arg);
                    arg = "";
                }   
            } else {
                arg += command[i];
            }
        }
    }
    if (arg.size() > 0) {
        add(arg);
    }
}

void Command::commit() {
    _argv.push_back(nullptr);
}


void Command::add(const std::string &arg) {
    char *data = new char[arg.size() + 1];
    strcpy(data, arg.c_str());
    
    _argv.push_back(data);
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
