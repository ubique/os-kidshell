#ifndef NSH_COMMAND_HPP
#define NSH_COMMAND_HPP

#include <string>
#include <vector>

class Command {
private:
    std::vector<char*> _argv, _envp;
    
    void add(const std::string&, bool contains_eq);
public:
    Command();
    ~Command();

    void add_env(char**);
    void parse(const std::string&);

    void commit();

    size_t argc() const;

    char* const name_chars() const;
    const std::string name() const;    
    
    char* const* argv() const;
    char* const* envp() const;
};

#endif
