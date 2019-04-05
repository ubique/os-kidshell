#pragma once
#include <exception>
#include <string>

class ParserException : std::exception {
public:
    ParserException(const std::string& desc) : desc(desc) {}

    virtual const char* what() const throw() override {
        return desc.c_str();
    }

private:
    std::string desc;
};
