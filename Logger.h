//
// Created by jetbrains on 27.03.19.
//

#ifndef OS_LOGGER_H
#define OS_LOGGER_H

#include <string_view>
#include <iostream>

namespace os {

    class Logger {
    public:
        enum class Level {
            NONE, 
            ERROR,
            WARN,
            INFO,
            DEBUG,
            TRACE
        };

        void error(std::string_view message);

        void warn(std::string_view message);

        void info(std::string_view message);

        void debug(std::string_view message);

        void trace(std::string_view message);

        inline static Level state = Level::INFO;
    private:
        std::string_view to_string(Level status);

        void log(std::string_view s, Level status);
    };
}


#endif //OS_LOGGER_H
