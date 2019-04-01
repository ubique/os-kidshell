#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define LINE_BUFFER_SIZE (1024)
#define NUMBER_OF_TOKENS (16)
#define TOKEN_BUFFER_SIZE (32)

#define ERROR(message)  ;fprintf(stderr, (message));exit(EXIT_FAILURE);
#define IS_DELIMITER(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' || (c) == '\a' || (c) == EOF)

char* _read_line() {
    int c;
    int i = 0;
    int current_buffer_size = LINE_BUFFER_SIZE;
    char* buffer = malloc(sizeof(char) * current_buffer_size);

    if (!buffer) { ERROR("shell: allocation error\n") }

    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[i] = '\0';
            return buffer;
        } else {
            buffer[i] = c;
        }
        ++i;
        if (i >= current_buffer_size) {
            current_buffer_size += LINE_BUFFER_SIZE;
            buffer = realloc(buffer, sizeof(char) * current_buffer_size);

            if (!buffer) { ERROR("shell: allocation error\n") }
        }
    }
}

char** _split_line_to_args(const char* line) {
    int current_number_of_tokens = NUMBER_OF_TOKENS;
    int current_token_buffer_size = TOKEN_BUFFER_SIZE;
    int i = 0;

    char** tokens = malloc(current_number_of_tokens * sizeof(char*));
    if (!tokens) { ERROR("shell: allocation error\n") }

    char* token = malloc(current_token_buffer_size * sizeof(char*));
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
            } else if (IS_DELIMITER(line[j])) {
                if (token[0]) {
                    token[k] = '\0';
                    tokens[i] = token;
                    ++i;

                    k = 0;
                    current_token_buffer_size = NUMBER_OF_TOKENS;
                    token = malloc(current_token_buffer_size * sizeof(char*));

                    if (!token) { ERROR("shell: allocation error\n") }
                }
            } else {
                token[k] = line[j];
                ++k;
            }
        }

        if (i >= current_number_of_tokens) {
            current_number_of_tokens += NUMBER_OF_TOKENS;
            tokens = realloc(tokens, current_number_of_tokens * sizeof(char*));
            if (!tokens) { ERROR("shell: allocation error\n") }
        }
        if (k >= current_token_buffer_size) {
            current_token_buffer_size += TOKEN_BUFFER_SIZE;
            token = realloc(token, current_token_buffer_size * sizeof(char*));
            if (!token) { ERROR("shell: allocation error\n") }
        }
    }

    if (flag_single_quote || flag_quote) { ERROR("shell: bad quotes escaping") }

    if (token[0]) {
        token[k] = '\0';
        tokens[i] = token;
        ++i;
    }

    if (i >= current_number_of_tokens) {
        current_number_of_tokens += 1;
        tokens = realloc(tokens, current_number_of_tokens * sizeof(char*));
        if (!tokens) { ERROR("shell: allocation error\n") }
    }

    tokens[i] = NULL;
    return tokens;
}

int _execute(char** args) {
    pid_t pid;
    pid_t wpid;
    int status = 0;

    pid = fork();
    if (pid == 0) {
        if (execve(args[0], args, NULL) == -1) { perror("shell"); }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        printf("%d\n", status);
    }
    return status;
}

void _loop() {
    char* line;
    char** args;
    int status;
    do {
        printf("$ ");
        line = _read_line();
        args = _split_line_to_args(line);
        status = _execute(args);
        free(line);
        free(args);
    } while (status >= 0);
}

int main(int argc, char** argv) {
    _loop();
    return EXIT_SUCCESS;
}
