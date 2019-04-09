#pragma once
#include <string.h>
#include <string>
#include <vector>

struct ExecArgumentsHolder {
    ExecArgumentsHolder(const std::vector<std::string>& data);

    ~ExecArgumentsHolder(); 

    char* const* arguments;
};

