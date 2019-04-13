#include "kidshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE_BUFFER_SIZE (1024)
#define NUMBER_OF_TOKENS (16)
#define TOKEN_BUFFER_SIZE (32)
#define MAX_ENV_VARS_SIZE (16)
#define ERROR(message)  ;fprintf(stderr, (message));exit(EXIT_FAILURE);

// environment variables

int cur_env_vars_size;
int max_env_vars_size;
char** env_vars_keys;
char** env_vars_values;

void shell_env_vars_init() {
    cur_env_vars_size = 0;
    max_env_vars_size = MAX_ENV_VARS_SIZE;
    env_vars_keys = malloc(max_env_vars_size * sizeof(char*));
    if (!env_vars_keys) { ERROR("shell: allocation error\n") }
    env_vars_values = malloc(max_env_vars_size * sizeof(char*));
    if (!env_vars_values) { ERROR("shell: allocation error\n") }
}

void shell_env_vars_increase_number() {
    max_env_vars_size += MAX_ENV_VARS_SIZE;
    env_vars_keys = realloc(env_vars_keys, max_env_vars_size * sizeof(char*));
    if (!env_vars_keys) { ERROR("shell: allocation error\n") }
    env_vars_values = realloc(env_vars_values, max_env_vars_size * sizeof(char*));
    if (!env_vars_values) { ERROR("shell: allocation error\n") }
}

void shell_env_vars_free() {
    for (int i = 0; env_vars_keys[i] && env_vars_values[i]; ++i) {
        free(env_vars_keys[i]);
        free(env_vars_values[i]);
    }
    free(env_vars_keys);
    free(env_vars_values);
}

void shell_env_vars_set(char* name, char* val) {
    for (int i = 0; env_vars_keys[i]; ++i) {
        if (strcmp(env_vars_keys[i], name) == 0) {
            free(env_vars_values[i]);
            env_vars_values[i] = malloc(strlen(val) * sizeof(char));
            strcpy(env_vars_values[i], val);
            return;
        }
    }
    env_vars_keys[cur_env_vars_size] = malloc(strlen(name) * sizeof(char));
    strcpy(env_vars_keys[cur_env_vars_size], name);
    env_vars_values[cur_env_vars_size] = malloc(strlen(val) * sizeof(char));
    strcpy(env_vars_values[cur_env_vars_size], val);
    ++cur_env_vars_size;
    if (cur_env_vars_size >= max_env_vars_size) {
        shell_env_vars_increase_number();
    }
}

void shell_env_vars_unset(char* name) {
    for (int i = 0; env_vars_keys[i]; ++i) {
        if (strcmp(name, env_vars_keys[i]) == 0) {
            free(env_vars_keys[i]);
            free(env_vars_values[i]);
            --cur_env_vars_size;
            env_vars_keys[i] = env_vars_keys[cur_env_vars_size];
            env_vars_values[i] = env_vars_values[cur_env_vars_size];
            env_vars_keys[cur_env_vars_size] = NULL;
            env_vars_values[cur_env_vars_size] = NULL;
            return;
        }
    }
    printf("Environment variable '%s' not exist.\n", name);
}

char** shell_env_vars_get() {
    char** res = malloc(cur_env_vars_size * sizeof(char*));
    if (!res) { ERROR("shell: allocation error\n") }
    int i = 0;
    for (; env_vars_keys[i]; ++i) {
        res[i] = malloc((strlen(env_vars_keys[i]) + 1 + strlen(env_vars_values[i]) + 1) * sizeof(char));
        if (!res[i]) { ERROR("shell: allocation error\n") }
        strcpy(res[i], env_vars_keys[i]);
        strcat(res[i], "=");
        strcat(res[i], env_vars_values[i]);
    }
    res[i] = NULL;
    return res;
}

void shell_env_vars_print() {
    for (int i = 0; env_vars_keys[i] && env_vars_values[i]; ++i) {
        printf("%s=%s\n", env_vars_keys[i], env_vars_values[i]);
    }
}

// builtin commands

char* builtin_str[] = {
        "export",
        "unset",
        "help",
        "exit"
};

int (* builtin_func[])(char**) = {
        &shell_builtin_export,
        &shell_builtin_unset,
        &shell_builtin_help,
        &shell_builtin_exit
};

int shell_builtins_num() {
    return sizeof(builtin_str) / sizeof(char*);
}

int shell_builtin_exit(char** args) {
    return 0;
}

int shell_builtin_help(char** args) {//todo
    printf("RubyUmbra's kidshell.\n");
    printf("Type full program name and arguments then press enter.\n");
    printf("The list of builtin commands:\n");
    for (int i = 0; i < shell_builtins_num(); ++i) {
        printf("\t-> %s\n", builtin_str[i]);
    }
    return 1;
}

int shell_builtin_export(char** args) {
    if (!args[1]) {
        shell_env_vars_print();
    } else {
        for (int i = 1; args[i]; ++i) {
            int flag = 1;
            int buf_size = TOKEN_BUFFER_SIZE;
            char* arg = args[i];
            char* name = malloc(buf_size * sizeof(char));
            if (!name) { ERROR("shell: allocation error\n") }
            char* val = malloc(buf_size * sizeof(char));
            if (!val) { ERROR("shell: allocation error\n") }
            int k = 0;
            for (int j = 0; arg[j]; ++j) {
                if (arg[j] == '=') {
                    name[k] = '\0';
                    flag = 0;
                    k = 0;
                    buf_size = TOKEN_BUFFER_SIZE;
                } else if (flag) {
                    name[k] = arg[j];
                    ++k;
                    if (k >= buf_size) {
                        buf_size += TOKEN_BUFFER_SIZE;
                        name = realloc(name, sizeof(char) * buf_size);
                        if (!name) { ERROR("shell: allocation error\n") }
                    }
                } else {
                    val[k] = arg[j];
                    ++k;
                    if (k >= buf_size) {
                        buf_size += TOKEN_BUFFER_SIZE;
                        val = realloc(val, sizeof(char) * buf_size);
                        if (!val) { ERROR("shell: allocation error\n") }
                    }
                }
            }
            val[k] = '\0';
            shell_env_vars_set(name, val);
            free(name);
            free(val);
        }
    }
    return 1;
}

int shell_builtin_unset(char** args) {
    if (!args[1]) {
        printf("Unset command should have one or more arguments.\n");
    } else {
        for (int i = 1; args[i]; ++i) {
            shell_env_vars_unset(args[i]);
        }
    }
    return 1;
}

// shell insides

char* shell_read_line() {
    int c;
    int i = 0;
    int cur_buf_size = LINE_BUFFER_SIZE;
    char* buf = malloc(sizeof(char) * cur_buf_size);
    if (!buf) { ERROR("shell: allocation error\n") }
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buf[i] = '\0';
            return buf;
        } else {
            buf[i] = c;
        }
        ++i;
        if (i >= cur_buf_size) {
            cur_buf_size += LINE_BUFFER_SIZE;
            buf = realloc(buf, sizeof(char) * cur_buf_size);
            if (!buf) { ERROR("shell: allocation error\n") }
        }
    }
}

char** shell_split_line_to_args(const char* line) {
    int cur_num_of_tokens = NUMBER_OF_TOKENS;
    int cur_token_buf_size = TOKEN_BUFFER_SIZE;
    int i = 0;
    char** tokens = malloc(cur_num_of_tokens * sizeof(char*));
    if (!tokens) { ERROR("shell: allocation error\n") }
    char* token = malloc(cur_token_buf_size * sizeof(char));
    if (!token) { ERROR("shell: allocation error\n") }
    int flag_single_quote = 0;
    int flag_quote = 0;
    int k = 0;
    for (int j = 0; line[j]; ++j) {
        if (flag_single_quote) {
            if (line[j] == '\'') {
                flag_single_quote = 0;
            } else if (line[j] == '\\') {
                ++j;
                if (!line[j]) { ERROR("shell: bad escaping") }
                token[k] = line[j];
                ++k;
            } else {
                token[k] = line[j];
                ++k;
            }
        } else if (flag_quote) {
            if (line[j] == '\"') {
                flag_quote = 0;
            } else if (line[j] == '\\') {
                ++j;
                if (!line[j]) { ERROR("shell: bad escaping") }
                token[k] = line[j];
                ++k;
            } else {
                token[k] = line[j];
                ++k;
            }
        } else {
            if (line[j] == '\'') {
                flag_single_quote = 1;
            } else if (line[j] == '\"') {
                flag_quote = 1;
            } else if (line[j] == '\\') {
                ++j;
                if (!line[j]) { ERROR("shell: bad escaping") }
                token[k] = line[j];
                ++k;
            } else if (line[j] == ' ' || line[j] == '\t' || line[j] == '\r' || line[j] == '\n' || line[j] == '\a' ||
                       line[j] == EOF) {
                if (token[0]) {
                    token[k] = '\0';
                    tokens[i] = token;
                    ++i;

                    k = 0;
                    cur_token_buf_size = NUMBER_OF_TOKENS;
                    token = malloc(cur_token_buf_size * sizeof(char*));

                    if (!token) { ERROR("shell: allocation error\n") }
                }
            } else {
                token[k] = line[j];
                ++k;
            }
        }
        if (i >= cur_num_of_tokens) {
            cur_num_of_tokens += NUMBER_OF_TOKENS;
            tokens = realloc(tokens, cur_num_of_tokens * sizeof(char*));
            if (!tokens) { ERROR("shell: allocation error\n") }
        }
        if (k >= cur_token_buf_size) {
            cur_token_buf_size += TOKEN_BUFFER_SIZE;
            token = realloc(token, cur_token_buf_size * sizeof(char*));
            if (!token) { ERROR("shell: allocation error\n") }
        }
    }
    if (flag_single_quote || flag_quote) { ERROR("shell: bad quotes escaping") }
    if (token[0]) {
        token[k] = '\0';
        tokens[i] = token;
        ++i;
    }
    if (i >= cur_num_of_tokens) {
        cur_num_of_tokens += 1;
        tokens = realloc(tokens, cur_num_of_tokens * sizeof(char*));
        if (!tokens) { ERROR("shell: allocation error\n") }
    }
    tokens[i] = NULL;
    return tokens;
}

int shell_execute(char** args) {
    if (args[0] == NULL) {
        return 1;
    }
    for (int i = 0; i < shell_builtins_num(); ++i) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0) {
        if (execve(args[0], args, shell_env_vars_get()) == -1) { perror("shell"); }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        printf("%d\n", status);
    }
    return 1;
}

void shell_loop() {
    do {
        if (feof(stdin)) { return; }
        printf("$ ");
    } while (shell_execute(shell_split_line_to_args(shell_read_line())));
}

int main(int argc, char** argv) {
    shell_env_vars_init();
    shell_loop();
    shell_env_vars_free();
    return EXIT_SUCCESS;
}
