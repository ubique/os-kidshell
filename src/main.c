#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "exit_messages.h"
#include "hashmap.h"

#define MAX_LEN 10000

int main(int argc, char *argv[], char *envp[]) {
    char command[MAX_LEN];
    pid_t child_pid;
    char exit_status_message[MAX_LEN];
    int wstatus;
    wordexp_t p;
    size_t i, j, len;
    char *variable, *value;
    char *word;
    data_item *env_var;
    char **command_split;
    char *str_new;
    size_t split_c;

    init_messages();
    power_arr = generate_power_arr(POWER_ARR_SIZE, P);
    data_item *hash_map = init_hashmap();
    command_split = malloc(MAX_LEN * sizeof(char *));

    /* add all environment variables to hash map */
    for (i = 0; envp[i] != NULL; ++i) {
        j = 0;
        while (envp[i][j] != '=') {
            j++;
        }

        variable = malloc(j * sizeof(char));
        strncpy(variable, envp[i], j);

        len = strlen(envp[i]) - j - 1;
        value = malloc(len * sizeof(char));
        strncpy(value, envp[i] + j + 1, len);

        add_hm(variable, value, hash_map, HASH_MAP_SIZE);
    }

    printf(">> ");

    while (fgets(command, sizeof(command), stdin)) {

        /* exit command */
        if (!strcmp(command, "exit\n"))
            return EXIT_SUCCESS;

        command[strlen(command) - 1] = '\0';

        for (i = 0; command[i] != '\0'; ++i) {
            if (command[i] == '=') {
                command[i] = ' ';
            }
        }

        wordexp(command, &p, 0);

        /* splitting command string */
        split_c = 0;
        word = strtok(command, " ");
        while (word != NULL) {
            free(command_split[split_c]);
            str_new = (char *)malloc(strlen(word) * sizeof(char));
            strcpy(str_new, "");
            strcat(str_new, word);
            word = strtok(NULL, " ");
            command_split[split_c] = str_new;
            ++split_c;
        }

        if (!strcmp(p.we_wordv[0], "export") && p.we_wordc == 3) {
            printf("Setting variable %s to \"%s\"\n", p.we_wordv[1],
                   p.we_wordv[2]);

            env_var = find_hm(p.we_wordv[1], hash_map, HASH_MAP_SIZE);
            if (env_var) {
                free(env_var->value);
                env_var->value = malloc(sizeof(char) * strlen(p.we_wordv[2]));
                strcpy(env_var->value, p.we_wordv[2]);
            } else {
                variable = malloc(sizeof(char) * strlen(p.we_wordv[1]));
                strcpy(variable, p.we_wordv[1]);
                value = malloc(sizeof(char) * strlen(p.we_wordv[2]));
                strcpy(value, p.we_wordv[2]);
                add_hm(variable, value, hash_map, HASH_MAP_SIZE);
            }

            printf(">> ");
            continue;
        }

        if (!strcmp(p.we_wordv[0], "unset") && p.we_wordc == 2) {
            printf("Unset variable %s\n", p.we_wordv[1]);
            remove_hm(command_split[1], hash_map, HASH_MAP_SIZE);
            printf(">> ");
            continue;
        }

        strcpy(command, "");
        for (i = 0; i < split_c; ++i) {
            if (command_split[i][0] == '$') {
                env_var =
                    find_hm(command_split[i] + 1, hash_map, HASH_MAP_SIZE);
                if (env_var != NULL) {
                    strcat(command, env_var->value);
                } else {
                    strcat(command, "");
                }
            } else {
                strcat(command, command_split[i]);
            }
            strcat(command, " ");
        }

        wordfree(&p);
        wordexp(command, &p, 0);

        child_pid = fork();
        switch (child_pid) {
        case -1:
            perror("forking error");
            exit(EXIT_FAILURE);
            break;

        case 0:
            /* Child */
            (void)execve(p.we_wordv[0], p.we_wordv, envp);
            printf("Error on program start\n");
            exit(EXIT_FAILURE);
            break;
            /* End child */

        default:
            /* Parent */
            do {
                /* man 2 wait */
                if (waitpid(child_pid, &wstatus, WUNTRACED) == -1) {
                    perror("waitpid() failed");
                    exit(EXIT_FAILURE);
                }

                if (WIFEXITED(wstatus)) {
                    /* Child terminated normally */
                    get_exit_message(exit_status_message, WEXITSTATUS(wstatus));
                    puts(exit_status_message);
                } else if (WIFSIGNALED(wstatus)) {
                    /* Child process was terminated by a signal */
                    get_exit_message(exit_status_message,
                                     128 + WTERMSIG(wstatus));
                    puts(exit_status_message);
                } else if (WIFSTOPPED(wstatus)) {
                    printf("Stopped by signal %d\n", WSTOPSIG(wstatus));
                }
            } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

            wordfree(&p);
            printf("\n");
            break;
            /* End parent */
        }
        printf(">> ");
    }

    free(power_arr);
    free(command_split);
    free_hasmap(hash_map);

    return EXIT_SUCCESS;
}
