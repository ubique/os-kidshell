#ifndef NSH_COMMAND_HPP
#define NSH_COMMAND_HPP

#include <string>
#include <vector>

class Command {
private:
    std::vector<char*> _argv;
    
    void add(const std::string&);
public:
    Command();
    ~Command();

    void parse(const std::string&);
    
    void commit();

    size_t argc() const;

    char* const name_chars() const;
    const std::string name() const;    
    
    char* const* argv() const;
};

#endif
