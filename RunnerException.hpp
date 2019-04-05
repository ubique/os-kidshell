#pragma once
#include <exception>
#include <string>

class RunnerException : std::exception {
public:
    RunnerException(const std::string& desc) : desc(desc) {}

    virtual const char* what() const throw() override {
        return desc.c_str();
    }

private:
    std::string desc;
};
