#include <algorithm>
#include <cstddef>
#include <regex>

#include "env_storage.h"

namespace cli
{

const std::string delimiter = "=";

const std::regex vshell_var_name_pattern{R"(^[a-zA-z\_][a-zA-Z\d\_]*$)"};
const std::regex vshell_variable_pattern{R"(^[a-zA-z\_][a-zA-Z\d\_]*=.*$)"};

void env_storage::add_environ(char ** env)
{
    auto p = env;
    while (*p)
    {
        set_env(std::string(*p++));
    }
}

int env_storage::set_env(const std::string &name_value)
{
    if (!std::regex_match(name_value, vshell_variable_pattern))
    {
        return -1;
    }
    const auto var = split_env_var(name_value);
    variables[var.first] = var.second;
    return 0;
}

int env_storage::unset_env(const std::string &name)
{
    if (!std::regex_match(name, vshell_var_name_pattern))
    {
        return -1;
    }
    variables.erase(name);
    return 0;
}

std::vector<std::string> env_storage::get_envs() const
{
    std::vector<std::string> res(variables.size());
    std::transform(variables.begin(), variables.end(), res.begin(),
                   [](const auto& entry) {
        return entry.first + "=" + entry.second;
    });
    return res;
}

std::pair<std::string, std::string> env_storage::split_env_var(const std::string &s) const
{
    const auto delim_pos = s.find(delimiter);
    const auto value = s.substr(std::min(delim_pos + 1, s.size()));
    return {s.substr(0, delim_pos), value};
}

}
