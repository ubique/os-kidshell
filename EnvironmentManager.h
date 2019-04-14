#ifndef OS_KIDSHELL_ENVIRONMENTMANAGER_H
#define OS_KIDSHELL_ENVIRONMENTMANAGER_H

#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>


class EnvironmentManager {
public:
    EnvironmentManager();

    explicit EnvironmentManager(char * envp[]);

    std::string getExecutable(std::string const& filepath);

    void exportVariable(std::string const &var);

    void unsetVariable(std::string const &var);

    void printVariables();

    void printValueByKey(std::string const &key);

    std::unique_ptr<char * const[]>  getVariablesArray();

private:

    bool checkExecutable(const char *pathname);

    void setPaths(std::string const& line);

    void printEntry(std::unordered_map<std::string, std::string>::iterator it);

    std::string entryToString(std::unordered_map<std::string, std::string>::iterator it);

    std::unordered_map<std::string, std::string> vars;

    std::map<std::string, std::string> varsString;

    std::vector<std::string> paths;
};


#endif //OS_KIDSHELL_ENVIRONMENTMANAGER_H
