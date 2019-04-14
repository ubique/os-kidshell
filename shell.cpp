#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <errno.h>


int main(int argc, char** argv, char** envp)
{
    const size_t ARGS_LENGTH = 128;

    std::string input;
    for (;;)
    {
        printf("shell:~$ ");
        std::getline(std::cin, input);
        if (input == "exit")
        {
            return 0;
        }
        if (input.empty())
        {
            continue;
        }
        else
        {
            char** args = new char* [ARGS_LENGTH + 1];
            std::stringstream is(input);
            std::string token;
            size_t counter = 0;

            while (is >> token && counter < ARGS_LENGTH)
            {
                args[counter] = new char[token.size() + 1];
                strcpy(args[counter++], token.c_str());
            }
            args[counter] = nullptr;

            switch (pid_t pid = fork())
            {
                case -1:
                {
                    perror("fork");
                    break;
                }
                case 0:
                {
                    //child
                    size_t i = 0;
                    execve(args[0], args, envp);
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
                default:
                {
                    //parent
                    int status;
                    if (wait(&status) == -1)
                    {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }

                    std::cout << "returned value: " <<  WEXITSTATUS(status) << '\n';
                    break;
                }
            }
        }
    }

}
