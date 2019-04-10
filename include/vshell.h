#ifndef H_VSHELL
#define H_VSHELL

#include <cstddef>
#include <set>

#include "command.h"
#include "env_storage.h"

namespace cli
{

std::vector<char*> get_pointers_vector(const std::vector<std::string> &v);
void print_prompt();
void print_error(const std::string &err, bool with_errno);

bool execute_builtin(const command& cmd);
void execute_external(const char *filename, char *const argv[], char *const env[]);
void execute(const command& cmd);

void vsh_loop();

} // namespace cli

#endif // H_VSHELL
