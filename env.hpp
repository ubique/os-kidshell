#ifndef NSH_ENV_HPP
#define NSH_ENV_HPP

#include <map>

class Env {
private:
    std::map<std::string, std::string> data;
    
    char** raw;
    
    void generate_raw();
    void erase_raw();
public:
    Env(char**);
    ~Env();
    
    char* const* get_raw() const;
    
    void set(const std::string&, const std::string&);    
    void unset(const std::string&);
};

#endif
