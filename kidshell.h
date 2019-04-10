#ifndef KIDSHELL_KIDSHELL_H
#define KIDSHELL_KIDSHELL_H

// environment variables
void shell_env_vars_init();
void shell_env_vars_increase_number();
void shell_env_vars_free();
void shell_env_vars_set(char* name, char* val);
void shell_env_vars_unset(char* name);
char** shell_env_vars_get();
void shell_env_vars_print();

// builtin commands
int shell_builtin_export(char** args);
int shell_builtin_unset(char** args);
int shell_builtin_help(char** args);
int shell_builtin_exit(char** args);
int shell_builtins_num();

// shell insides
char* shell_read_line();
char** shell_split_line_to_args(const char* line);
int shell_execute(char** args);
void shell_loop();
int main(int argc, char** argv);

#endif //KIDSHELL_KIDSHELL_H
