//
// Created by jetbrains on 27.03.19.
//

#include "Logger.h"

#include <cstring>

namespace os {
    void Logger::debug(std::string_view message) {
        log(message, Level::DEBUG);
    }

    void Logger::info(std::string_view message) {
        log(message, Level::INFO);
    }

    void Logger::error(std::string_view message) {
        log(message, Level::ERROR);
    }

    std::string_view Logger::to_string(Logger::Level status) {
        switch (status) {
            case Level::TRACE:
                return "TRACE";
            case Level::DEBUG:
                return "DEBUG";
            case Level::INFO:
                return "INFO";
            case Level::WARN:
                return "WARN";
            case Level::ERROR:
                return "ERROR";
            default:
                return "";
        }
    }

    void Logger::log(std::string_view s, Logger::Level status) {
        if (status <= state) {
            std::cerr << to_string(status) << " " << s << std::endl;
            std::cerr << "ERRNO " << strerror(errno) << std::endl;
        }
    }

    void Logger::warn(std::string_view message) {
        log(message, Level::WARN);
    }

    void Logger::trace(std::string_view message) {
        log(message, Level::TRACE);
    }

}