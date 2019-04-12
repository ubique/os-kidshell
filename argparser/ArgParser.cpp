//
// Created by Roman aka ifkbhit (https://github.com/aprox13) on 01.04.2019.
//

#include "ArgParser.h"
#include "ParserException.h"
#include <sstream>
#include <iostream>
#include <set>

void parser::ArgParser::addArgument(std::string &name, int paramsCount, bool required) {
    rules.insert(std::make_pair(name, std::make_pair(paramsCount, required)));
}

std::vector<std::string> &parser::ArgParser::split(std::string &base) {
    bool foundNonSpace = false;
    for (char c: base) {
        if (!std::isspace(c)) {
            foundNonSpace = true;
            break;
        }
    }
    if (!foundNonSpace) {
        throw 0;
    }

    std::stringstream token;
    int index = 0;
    while (index < base.size()) {
        while (index < base.size() && isWs(base[index])) {
            index++;
        }
        if (index < base.size()) {
            char current = base[index];

            if (isSlash(current) || isQuote(current)) {
                index++;
                if (index == base.size() || base[index] == ' ') {
                    throwUnexpectedToken(current, index - 1);
                }
                token << base[index++];
                char endChar;
                if (isSlash(current)) {
                    endChar = ' ';
                } else {
                    endChar = current;
                }

                while (index < base.size() && base[index] != endChar) {
                    if (isSlash(base[index])) {
                        index++;
                        if (base.size() == index) {
                            throwUnexpectedToken('\\', index);
                        }
                    }
                    char c = base[index];
                    token << c;
                    index++;
                }
                if (index == base.size() && isQuote(endChar) && !isQuote(base[index])) {// endchar is ' or "
                    throw parser::ParserException(
                            "Excepted token " + std::string(1, endChar) + " but end of line reached");
                }
                if (isQuote(endChar)) {
                    index++;
                }
            } else {
                while (index < base.size() && !isWs(base[index])) {
                    if (isQuote(base[index])) {
                        throwUnexpectedToken(base[index], index);
                    }
                    if (isSlash(base[index])) {
                        index++;
                        if (index == base.size()) {
                            throwUnexpectedToken('\\', index - 1);
                        }
                    }
                    token << base[index++];
                }
            }
            auto s = token.str();
            if (!s.empty()) {
                parsed.push_back(s);
                token.str(std::string());
            }
        }
    }

    return parsed;
}

void parser::ArgParser::throwUnexpectedToken(std::string &token, int position) {
    throw parser::ParserException("Unexpected token " + token + " at position " + std::to_string(position));
}

void parser::ArgParser::throwUnexpectedToken(char token, int position) {
    auto stringToken = std::string(1, token);
    this->throwUnexpectedToken(stringToken, position);
}

bool parser::ArgParser::isWs(char c) {
    return c == ' ';
}

bool parser::ArgParser::isQuote(char c) {
    return c == '\'' || c == '"';
}

bool parser::ArgParser::isSlash(char c) {
    return c == '\\';
}

void parser::ArgParser::setOptionalCount(int count, bool firstIsProgram) {
    optionalCount = count;
    if (firstIsProgram) {
        optionalCount += 1;
    }
}

void parser::ArgParser::parse() {
    int argIndex = 0;

    std::set<std::string> found;
    if (optionalCount > 0) {
        for (int i = 0; i < optionalCount; i++) {
            if (argIndex == parsed.size()) {
                throw parser::ParserException("Too few optional arguments");
            }
            optional.emplace_back(parsed[argIndex++]);
        }
    }


    while (argIndex < parsed.size()) {
        auto element = parsed[argIndex];
        if (isParamName(element)) {
            auto trim = trimParamName(element);
            if (trim.empty()) {
                throwUnexpectedArg(element);
            }
            int count = rules[trim].first;
            argIndex++;
            for (int i = 0; i < count; i++) {
                if (argIndex == parsed.size()) {
                    throw parser::ParserException("Unexpected reach of arguments list for name " + trim);
                }
                if (isParamName(parsed[argIndex])) {
                    std::cout << "Argi " << argIndex << ", arg: " << parsed[argIndex] << '\n';
                    throw parser::ParserException(
                            "For argument " + trim + " excepted " + std::to_string(count) + " parameters, found: " +
                            std::to_string(i));
                }
                if (result.count(trim) == 0) {
                    result.insert(std::make_pair(trim, std::vector<std::string>()));
                }
                result[trim].push_back(parsed[argIndex++]);
            }
        } else {
            throwUnexpectedArg(element);
        }
    }


    for (auto &rule : rules) {
        if (result.count(rule.first) == 0 &&
            rule.second.second) { // name in rules with "required = true" nad not in result
            throw parser::ParserException("Argument " + rule.first + " excepted but not found");
        }
    }

}

void parser::ArgParser::parse(char **args) {
    parsed.clear();
    int i = 0;
    while (args[i] != nullptr) {
        parsed.emplace_back(args[i]);
        i++;
    }
    parse();
}

void parser::ArgParser::parse(const char *line) {
    auto s = std::string(line);
    split(s);
    parse();
}

void parser::ArgParser::throwUnexpectedArg(std::string &name) {
    throw parser::ParserException("Unexpected argument " + name);
}

bool parser::ArgParser::isParamName(std::string &name) {
    return !trimParamName(name).empty();
}


std::vector<std::string> &parser::ArgParser::get(std::string &name) {
    if (result.count(name) == 0) {
        throw parser::ParserException("No argument " + name + " found");
    }
    if (result[name].empty()) {
        throw parser::ParserException("Argument " + name + " has no values");
    }
    return result[name];
}

std::string &parser::ArgParser::operator[](std::string &name) {
    return get(name)[0];
}

void parser::ArgParser::addArgument(const char *name, int paramsCount, bool required) {
    auto s = std::string(name);
    addArgument(s, paramsCount, required);
}

std::string parser::ArgParser::trimParamName(std::string &name) {
    auto trim = std::string(name);
    while (!trim.empty() && trim[0] == '-') {
        trim.erase(trim.begin());
    }
    if (trim.empty() || rules.count(trim) == 0) {
        return "";
    }
    return trim;
}

std::string &parser::ArgParser::operator[](const char *name) {
    auto s = std::string(name);
    return this->operator[](s);
}

std::vector<std::string> &parser::ArgParser::get(const char *name) {
    auto s = std::string(name);
    return this->get(s);;
}

char **parser::ArgParser::toArgsArray(int &size, int from, int to) {

    return nullptr;
}

bool parser::ArgParser::exist(std::string &name) {
    return result.count(name) != 0;
}

bool parser::ArgParser::exist(const char *name) {
    auto s = std::string(name);
    return exist(s);
}

std::vector<std::string> &parser::ArgParser::getOptional() {
    return optional;
}



