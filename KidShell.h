#ifndef OS_KIDSHELL_KIDSHELL_H
#define OS_KIDSHELL_KIDSHELL_H


#include <vector>
#include <string>
#include <memory>
#include "EnvironmentManager.h"

class KidShell {
public:
    KidShell();

    KidShell(char * envp[]);

    void run();

private:
    std::vector<std::string> getArgs(std::string const& line);

    std::vector<std::vector<std::string>> getPipedArgs(std::string line);

    std::unique_ptr<char * const[]> getSyscallArgs(std::vector<std::string>& args);

    void handlePipelessRequest(std::vector<std::string> &args);

    void handleRequestWithPipes(std::string const &line);

    void execute(char * const argv[], char * const envp[], bool withExitCode = true);

    bool constainPipes(std::string const& line);

    void closeAllDescriptors(int fds[], size_t fdNumber);

    void closeFileDescriptor(int fd);

    void handleExport(std::vector<std::string> &args);

    void handleUnset(std::vector<std::string> &args);

    EnvironmentManager env;
};


#endif //OS_KIDSHELL_KIDSHELL_H
