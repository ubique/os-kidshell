#ifndef H_ENVWRAPPER
#define H_ENVWRAPPER

#include <map>
#include <string>
#include <vector>

namespace cli
{

class env_wrapper
{                                   	

public:

    env_wrapper() = default;

    void set_env(const std::string &name_value);
    void unset_env(const std::string &name);
    std::vector<std::string> get_envs() const;

private:

    std::pair<std::string, std::string> split_env_var(const std::string &s);

    std::map<std::string, std::string> envs;

};

} // namespace cli

#endif // H_ENVWRAPPER
