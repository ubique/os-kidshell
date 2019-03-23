#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LINE_LENGTH 100000
#define MAX_ARGS 1024
#define MAX_RETRIES 100

char s[MAX_LINE_LENGTH];
char* parsed[MAX_ARGS];

int get_line(char *s) {
    int c = getchar();
    int pos = 0;
    while (c != EOF && (char)c != '\n') {
        s[pos] = (char)c;
        pos++;
        if (pos == MAX_LINE_LENGTH) {
            printf("Too long input\n");
            exit(0);
        }
        c = getchar();
    }   
    s[pos] = 0;
    if (c == EOF)
        return 0;
    else
        return 1;
}

void parse(char *s, char *ans[]) {
    int pos = 0;
    int has_quote = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\'') {
            has_quote ^= 1;
            s[i] = '\0';
        } else
        if (has_quote == 0 && (s[i] == ' ' || s[i] == '\t')) {
            s[i] = '\0'; 
        } else
        if ((i == 0 || s[i - 1] == '\0') && s[i] != '\0') {
            ans[pos] = &s[i];
            pos++;
            if (pos == MAX_ARGS) {
                printf("Too many arguments\n");
                ans[0] = NULL;
                return;
            }
        }
    }
    if (has_quote == 1) {
        printf("Illegal number of quotes\n");
        ans[0] = NULL;
        return;
    }
    ans[pos] = NULL;
}

int main() {
    while (get_line(s) == 1) {
        parse(s, parsed);
        if (parsed[0] == NULL) continue;
        pid_t child_pid;
        int tries = 0;
        do {
          child_pid = fork();  
          tries++;
        } while (child_pid == -1 && errno == EAGAIN && tries <= MAX_RETRIES);
        if (child_pid == -1) {
            printf("Can't fork process: %s\n", strerror(errno));
            continue;
        }
        if (child_pid == 0) {
            int status;
            int tries = 0;
            do {
              status = execv(parsed[0], parsed);
              tries++;
            } while (status == -1 && errno == EAGAIN && tries <= MAX_RETRIES);
            if (status == -1)
                printf("Errors while execution: %s\n", strerror(errno));
            exit(0);
        } else {
            int status;
            int result;
            do {
                result = waitpid(child_pid, &status, 0);
            } while (result == -1 && errno == EINTR);
            if (result == -1) {
                printf("Can't wait for the result of child: %s\n", strerror(errno));
                continue;
            }
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("Process finished with exit code %d\n", exit_code);
            } else if (WIFSIGNALED(status)) {
                int term_signal = WTERMSIG(status);
                printf("Process terminated with signal %s\n", strsignal(term_signal));
            } else {
                printf("Something strange happened with process\n");
                continue;
            }
        }
    }
}//