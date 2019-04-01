#include <errno.h>     // errno
#include <error.h>     // error TODO gnu extension, replace with portable code
#include <stdbool.h>   // true
#include <stdio.h>     // stdin, stderr, getline
#include <stdlib.h>    // free, setenv, unsetenv
#include <string.h>    // strerror, strcmp, strdup, strsep
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // waitpid
#include <unistd.h>    // fork, execve
#include <wordexp.h>   //

extern char **environ;

void do_export(char *const argv[]) {
    for (; *argv != NULL; ++argv) {
        char *to_free, *value;
        value = to_free = strdup(*argv);
        if (!to_free) {
            error(EXIT_FAILURE, errno, "Couldn't copy a string");
        }
        char *key = strsep(&value, "=");
        if (value) {
            if (setenv(key, value, true) == -1) {
                error(0, errno, "Couldn't set %s", *argv);
            }
        }
        free(to_free);
    }
}

void do_unset(char *const argv[]) {
    for (; *argv != NULL; ++argv) {
        if (unsetenv(*argv) == -1) {
            error(0, errno, "Couldn't unset %s", *argv);
        }
    }
}

void launch(char const *program, char *const argv[]) {
    pid_t pid = fork();
    if (pid == -1) {
        error(0, errno, "Failed to fork");
        return;
    }
    if (pid == 0) {
        int ec = execve(program, argv, environ);
        if (ec == -1) {
            error(0, errno, "Failed to exec %s", program);
            return;
        }
    } else {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1) {
            error(0, errno, "Failed to wait for child %d", pid);
            return;
        }
        printf("Status information: %d\n", wstatus);
    }
}

void trim_newline(char *line, ssize_t *len) {
    if (*len <= 0) {
        return;
    }
    if (line[*len - 1] == '\n') {
        line[--*len] = '\0';
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    wordexp_t p;

    // init wordexp for future reuse
    wordexp("", &p, 0);

    while ((nread = getline(&line, &len, stdin)) != -1) {
        trim_newline(line, &nread);
        int ec = wordexp(line, &p, WRDE_REUSE | WRDE_SHOWERR | WRDE_UNDEF);
        switch (ec) {
        case WRDE_BADCHAR:
            error(0, 0, "Illegal occurrence of newline or one of |, &, ;, <, >, (, ), {, }.");
            continue;
        case WRDE_BADVAL:
            error(0, 0, "Undefined shell variable was referenced");
            continue;
        case WRDE_CMDSUB:
            error(0, 0, "Command line substitution is prohibited");
            continue;
        case WRDE_NOSPACE:
            error(0, 0, "Out of memory");
            continue;
        }
        if (p.we_wordc == 0) {
            continue;
        }
        char const *action = p.we_wordv[0];
        if (strcmp("export", action) == 0) {
            do_export(p.we_wordv + 1);
        } else if (strcmp("unset", action) == 0) {
            do_unset(p.we_wordv + 1);
        } else if (strcmp("exit", action) == 0) {
            break;
        } else {
            launch(p.we_wordv[0], p.we_wordv);
        }
    }
    if (ferror(stdin)) {
        error(EXIT_FAILURE, errno, "Failed to read next line");
    }
    wordfree(&p);
    free(line);
}
