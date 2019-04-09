#include "ExecArgumentsHolder.h"


ExecArgumentsHolder::ExecArgumentsHolder(const std::vector<std::string>& data) {
    char * * d = new char*[data.size() + 1];

    for (size_t i = 0; i < data.size(); ++i) {
        char* s = new char[data[i].size() + 1];
        memcpy(s, data[i].c_str(), data[i].size() + 1);
        d[i] = s;
    }

    d[data.size()] = 0;

    arguments = d;
}

ExecArgumentsHolder::~ExecArgumentsHolder() {
    for (size_t i = 0; arguments[i] != nullptr; ++i) {
        delete[](arguments[i]);
    }
    delete[](arguments);
}
