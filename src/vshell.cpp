#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>

#include "parser.h"
#include "vshell.h"

namespace cli {

// extern char **environ;

env_storage env_variables;

const std::string shell_name = "vshell";
const std::string prompt = shell_name + ">";

static std::string local_errno;

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

void print_error(const std::string &err = "", bool with_errno = true)
{
    std::cerr << shell_name + ": " << err << (with_errno ? strerror(errno) : "") << std::endl;
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
            const auto res_code = env_variables.set_env(cmd.arguments[i]);
            if (res_code != 0)
            {
                print_error("\"" + cmd.arguments[i] + "\": this is an invalid identifier", false);
            }
        }
        return true;
    }
    else if (cmd.executable == "unset")
    {
        for (std::size_t i = 1; i < cmd.arguments.size(); ++i)
        {
            const auto res_code = env_variables.unset_env(cmd.arguments[i]);
            if (res_code != 0)
            {
                print_error("\"" + cmd.arguments[i] + "\": this is an invalid variable name", false);
            }
        }
        return true;
    }
    else if (cmd.executable == "printenv")
    {
        const auto env_vars = env_variables.get_envs();
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
        const auto exec_result = execve(filename, argv, env);
        if (exec_result == -1)
        {
            print_error(std::string(filename)  + ": ");
            exit(EXIT_FAILURE);
        }
    }
    else // parent process
    {
        int child_status;
        const auto wait_result = waitpid(pid, &child_status, 0);
        if (wait_result == -1)
        {
            print_error();
        }
        else
        {
            std::cout << "<exit code> " << WEXITSTATUS(child_status) << std::endl;
        }
    }
}

void execute(const command& cmd)
{
    if (execute_builtin(cmd))
    {
        return;
    };
    const auto env_vector = env_variables.get_envs();
    const auto args = get_pointers_vector(cmd.arguments);
    const auto env_pointer_vector = get_pointers_vector(env_vector);
    execute_external(cmd.executable.c_str(), args.data(), env_pointer_vector.data());
}

void vsh_loop()
{
    std::string command_line;
    for (;;)
    {
        print_prompt();
        std::getline(std::cin, command_line);

        if(std::cin.eof())
        {
            break;
        }

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
    // no envp[] because of POSIX.1
    using namespace cli;

    env_variables.add_environ(environ);

    vsh_loop();
}
