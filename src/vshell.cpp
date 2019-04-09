#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>

#include "parser.h"
#include "vshell.h"

namespace cli {

const std::string shell_name = "vshell";
const std::string prompt = shell_name + ">";

std::vector<char*> get_pointers_vector(const std::vector<std::string> &v)
{
    std::vector<char*> res(v.size() + 1, nullptr);
    std::transform(v.begin(), v.end(), res.begin(),
                   [](const auto& s) {
        return const_cast<char*>(s.c_str());
    });
    return res;
}

void print_prompt()
{
    std::cout << prompt;
}

bool execute_builtin(const command& cmd)
{
    if (cmd.executable == "exit")
    {
        exit(EXIT_SUCCESS);
    }
    else if (cmd.executable == "export")
    {
        for (std::size_t i = 1; i < cmd.arguments.size(); ++i)
        {
            envs.set_env(cmd.arguments[i]);
        }
        return true;
    }
    else if (cmd.executable == "unset")
    {
        for (std::size_t i = 1; i < cmd.arguments.size(); ++i)
        {
            envs.unset_env(cmd.arguments[i]);
        }
        return true;
    }
    else if (cmd.executable == "printenv")
    {
        const auto env_vars = envs.get_envs();
        for (auto& v : env_vars)
        {
            std::cout << v << std::endl;
        }
        return true;
    }
    return false;
}

void execute_external(const char *filename, char *const argv[], char *const env[])
{
    pid_t pid = fork();
    if (pid <= -1) // error
    {
        std::cerr << "fork: " << strerror(errno) << std::endl;
    }
    else if (pid == 0) // child process
    {
        execve(filename, argv, env);
        std::cerr << shell_name + ": " << argv[0] << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    else // parent process
    {
        int child_status;
        waitpid(pid, &child_status, 0);
        std::cout << "<exit code> " << WEXITSTATUS(child_status) << std::endl;
    }
}

void execute(const command& cmd)
{
    if (execute_builtin(cmd))
    {
        return;
    };
    const auto env_vector = envs.get_envs();
    const auto args = get_pointers_vector(cmd.arguments);
    const auto env_pointer_vector = get_pointers_vector(env_vector);
    execute_external(cmd.executable.c_str(), args.data(), env_pointer_vector.data());
}

void vsh_loop()
{
    std::string command_line;
    for (;;)
    {
        if(std::cin.eof())
        {
            break;
        }

        print_prompt();
        std::getline(std::cin, command_line);
        const auto cmd = parse_command_line(command_line);

        if (cmd.has_value())
        {
            execute(cmd.value());
        }
    }
}

} // namespace cli

int main()
{
    using namespace cli;

    vsh_loop();
}
