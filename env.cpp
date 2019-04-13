#include "env.hpp"

#include <cstring>

Env::Env(char **envp): raw(nullptr) {
    while (*envp != nullptr) {
        char* pos = strstr(*envp, "=");
        *pos = '\0';
        ++pos;
        
        data[std::string(*envp)] = std::string(pos);
        
        ++envp;
    }
    
    generate_raw();
}

Env::~Env() {
    erase_raw();
}

char* const* Env::get_raw() const {
    return raw;
}

void Env::set(const std::string& key, const std::string& value) {
    data[key] = value;
    generate_raw();
}

void Env::unset(const std::string& key) {
    data.erase(key);
    generate_raw();
}

void Env::erase_raw() {
    if (raw == nullptr) {
        return;
    }
    
    char **ptr = raw;
    
    while (*ptr != nullptr) {
        delete[] *ptr;
        ++ptr;
    }
    
    delete[] raw;
    raw = nullptr;
}

void Env::generate_raw() {
    erase_raw();
    
    raw = new char*[data.size() + 1];
    
    size_t i = 0;
    for (const auto& [key, value]: data) { 
        std::string entry = key + "=" + value;
        raw[i] = new char[entry.size() + 1];
        strcpy(raw[i], entry.c_str());
        ++i;
    }
    raw[i] = nullptr;
}
