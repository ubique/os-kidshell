#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <string>
#include <vector>
using std::string;

/*!
 * Manages shell environment variables
 * For PATH, keeps an up-to-date searchable paths list
 */
class Environment
{
public:
    explicit Environment(char *envp[]);

    void update_paths(const string& path_value);
    void set_variable(const string& assignment);
    void unset_variable(const string& var);
    const std::map<string, string>& get_variables();
    string find_executable(const string& filename);
private:
    bool executable_exists(const string& filename);

    std::map<string, string> m_environ;
    std::vector<string> m_paths;
};

#endif // ENVIRONMENT_H
