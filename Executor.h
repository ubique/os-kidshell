//
// Created by jetbrains on 29.03.19.
//

#ifndef OS_EXECUTOR_H
#define OS_EXECUTOR_H


#include <vector>
#include <string>
#include "Logger.h"

namespace os {
    class Executor {
        Logger logger;

        std::vector<std::string> split(const char *string);

        std::vector<char *> c_cast(std::vector<std::string> &string);

    public:
        int execute(std::string_view string, char *const *env = nullptr);
    };
}


#endif //OS_EXECUTOR_H
