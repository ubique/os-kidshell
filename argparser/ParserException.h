#pragma once

#include <exception>
#include <string>
//
// Created by Roman aka ifkbhit (https://github.com/aprox13) on 01.04.2019.
//



namespace parser {
    class ParserException : std::runtime_error {
    public:
        explicit ParserException(const std::string &message) : std::runtime_error(message.c_str()) {}

        virtual std::string what() {
            return std::runtime_error::what();
        }
    };
}
