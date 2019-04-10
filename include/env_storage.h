#ifndef H_ENVWRAPPER
#define H_ENVWRAPPER

#include <map>
#include <string>
#include <vector>

namespace cli
{

class env_storage
{                                   	

public:

    env_storage() = default;

    void add_environ(char ** env);
    int set_env(const std::string &name_value);
    int unset_env(const std::string &name);

    std::vector<std::string> get_envs() const;

private:

    std::pair<std::string, std::string> split_env_var(const std::string &s) const;

    std::map<std::string, std::string> variables;

};

} // namespace cli

#endif // H_ENVWRAPPER
