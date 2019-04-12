#pragma once

#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <algorithm>
#include "ParserException.h"

//
// Created by Roman aka ifkbhit (https://github.com/aprox13) on 01.04.2019.
//

namespace parser {

    class ArgParser {

    public:
        ArgParser() = default;
        void addArgument(std::string& name, int paramsCount, bool required = false);
        void addArgument(const char* name, int paramsCount, bool required = false);
        void setOptionalCount(int count, bool firstIsProgram = true);
        std::vector<std::string>& split(std::string& line);
        void parse(char** args);
        void parse(const char* line);
        std::string& operator[](std::string& name);
        std::string& operator[](const char* name);
        std::vector<std::string>& get(std::string& name);
        std::vector<std::string>& get(const char* name);
        bool exist(const char* name);
        bool exist(std::string& name);
        char** toArgsArray(int& size, int from = 0, int to = -1); // if to is -1 take all to end
        std::vector<std::string>& getOptional();

    private:
        std::vector<std::string> parsed;
        std::vector<std::string> optional;
        int optionalCount = 1; // only program name
        std::map<std::string, std::pair<int, bool>> rules;
        std::map<std::string, std::vector<std::string>> result;

        void throwUnexpectedToken(std::string& token, int position);
        void throwUnexpectedToken(char token, int position);
        void throwUnexpectedArg(std::string& name);
        bool isWs(char c);
        bool isQuote(char c);
        bool isSlash(char c);
        bool isParamName(std::string&);
        void parse();
        std::string trimParamName(std::string& name);
    };
}