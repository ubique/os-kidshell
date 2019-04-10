/*
 * Exit messages generator
 */

#ifndef EXIT_MESSAGES_H
#define EXIT_MESSAGES_H

#define N_CODES 256

void init_messages();

void get_exit_message(char *str, int status_code);

#endif

