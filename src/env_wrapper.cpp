#include <algorithm>

#include "env_wrapper.h"

namespace cli
{

const std::string delimiter = "=";

void env_wrapper::set_env(const std::string &name_value)
{
    const auto var = split_env_var(name_value);
    envs[var.first] = var.second;
}

void env_wrapper::unset_env(const std::string &name)
{
    envs.erase(name);
}

std::vector<std::string> env_wrapper::get_envs() const
{
    std::vector<std::string> res(envs.size());
    std::transform(envs.begin(), envs.end(), res.begin(),
                   [](const auto& entry) {
        return entry.first + "=" + entry.second;
    });
    return res;
}

std::pair<std::string, std::string> env_wrapper::split_env_var(const std::string &s)
{
    const auto delim_pos = s.find(delimiter);
    const auto value = s.substr(std::min(delim_pos + 1, s.size()));
    return {s.substr(0, delim_pos), value};
}

}
