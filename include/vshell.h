#ifndef H_VSHELL
#define H_VSHELL

#include <cstddef>
#include <set>

#include "command.h"
#include "env_wrapper.h"

namespace cli
{

env_wrapper envs;

std::vector<char*> get_pointers_vector(const std::vector<std::string> &v);
void print_prompt();

bool execute_builtin(const command& cmd);
void execute_external(const char *filename, char *const argv[], char *const env[]);
void execute(const command& cmd);

void vsh_loop();

} // namespace cli

#endif // H_VSHELL
