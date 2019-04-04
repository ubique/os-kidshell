#include <unistd.h>
#include <sys/wait.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>

using argv_t = std::vector<char*>;

void tokenize(std::string& str,
              argv_t& result)
{
    std::string::iterator last_p = str.begin();

    for (std::string::iterator pos;
         last_p != str.end()
      && (pos = std::find_if_not(last_p, str.end(), [](char c){ return std::isspace(c); })) != std::end(str); )
    {
        auto sec_iter = std::find_if(pos, str.end(), [](char c){ return std::isspace(c);});
        if (sec_iter == str.end())
            str.push_back('\0');
        else
            *sec_iter = '\0';

        result.push_back(pos.base());
        last_p = sec_iter;
        ++last_p;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::string buffer;
    argv_t arg_list;

    while (true)
    {
        buffer.clear();

        std::cout << ">>> ";
        std::getline(std::cin, buffer);
        if (buffer.empty()) { return 0; }

        arg_list.clear();
        tokenize(buffer, arg_list);
        arg_list.push_back(nullptr);

        auto pid = fork();
        if (pid == 0)
        {
            return execve(arg_list[0], arg_list.data(), nullptr);
        } else if (pid < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            return -1;
        } else
        {
            int status;
            auto pd = wait(&status);
            if (pd == -1)
            {
                std::cerr << strerror(errno) << std::endl;
                return -1;
            }

            if (WIFEXITED(status))
            {
                auto exit_code = WEXITSTATUS(status);
                if (exit_code)
                    std::cerr << "Exited with code " << exit_code << std::endl;
                continue;
            }

            if (WIFSIGNALED(status))
            {
                std::cerr << "Was killed by " << WTERMSIG(status) << std::endl;
                continue;
            }
        }
    }
}
