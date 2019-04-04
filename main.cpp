#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cassert>
#include <cstring>

using argv_t = std::vector<char*>;

static volatile sig_atomic_t current_pid = 0;

enum
{
    buffer_size = 255
};

template<typename ForwardIt>
void tokenize(ForwardIt const& str_begin,
              ForwardIt const& str_end,
              argv_t& result)
{
    auto last_p = str_begin;
    for (decltype(last_p) pos;
         last_p != str_end
      && (pos = std::find_if_not(last_p, str_end, [](char c){ return std::isspace(c) || c == 0; })) != str_end; )
    {
        auto sec_iter = std::find_if(pos, str_end, [](char c){ return std::isspace(c) || c == 0; });
        *sec_iter = '\0';

        result.push_back(pos);
        last_p = sec_iter;
        ++last_p;
    }
}

void sigint_handler(int)
{
    if (current_pid != 0)
    {
        kill(static_cast<pid_t>(current_pid), SIGINT);
    }
}

int main()
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigint_handler;
//    action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action, nullptr);

    char buff[buffer_size];
    char* buff_end = buff;
    argv_t arg_list;

    while (true)
    {
        auto const msg = ">>> ";

        write(fileno(stdout), "\n", 1);
        write(fileno(stdout), msg, strlen(msg));
        auto cnt = read(fileno(stdin), buff, buffer_size - 1);
        buff_end = buff + cnt;

        if (errno != 0 && errno != EINTR)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        }

        if (cnt == 0 && errno != EINTR)
            return 0;

        errno = 0;

        arg_list.clear();
        *buff_end = '\0';
        tokenize(static_cast<char*>(buff), buff_end, arg_list);
        arg_list.push_back(nullptr);

        if (arg_list.size() < 2)
            continue;

        if (arg_list.size() == 2
         && memcmp(arg_list[0], "exit", std::min(strlen(arg_list[0]), static_cast<size_t>(4))) == 0)
        {
            return 0;
        }

        extern char** environ;

        current_pid = fork();
        if (current_pid == 0)
        {
            return execve(arg_list[0], arg_list.data(), environ);
        } else if (current_pid < 0)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return -1;
        } else
        {
            int status;
            auto pd = wait(&status);
            current_pid = 0;
            if (errno == EINTR)
            {
                assert(pd == -1);
                errno = 0;
                fputs("Interruped\n", stderr);
                continue;
            }

            if (pd == -1)
            {
                fprintf(stderr, "%s\n", strerror(errno));
                return -1;
            }

            if (WIFEXITED(status))
            {
                auto exit_code = WEXITSTATUS(status);
                if (exit_code)
                    fprintf(stderr, "Exited with code %d\n", exit_code);
                continue;
            }

            if (WIFSIGNALED(status))
            {
                fprintf(stderr, "Was killed by %s\n", strsignal(WTERMSIG(status)));
                continue;
            }
        }
    }
}
