/*
 * Exit messages generator
 */

#include "exit_messages.h"

#include <string.h>
#include <stdio.h>


/* Exit codes */
char *message_texts[N_CODES];

void init_messages() {
    int i;

    for (i = 0; i < N_CODES; ++i) {
        message_texts[i] = NULL;
    }


    message_texts[0] = "Successful termination";

    /* Advanced Bash-Scripting Guide:
     * Exit Codes With Special Meanings */
    message_texts[1]   = "Catchall for general errors";
    message_texts[2]   = "Misuse of shell builtins";
    message_texts[126] = "Command invoked cannot execute";
    message_texts[127] = "Command not found";
    message_texts[128] = "Invalid argument to exit";
    message_texts[255] = "Exit status out of range";
    

    /* <sysexits.h> */
    message_texts[64] = "Command line usage error";
    message_texts[65] = "Data format error";
    message_texts[66] = "Cannot open input";
    message_texts[67] = "Addressee unknown";
    message_texts[68] = "Host name unknown";
    message_texts[69] = "Service unavailable";
    message_texts[70] = "Internal software error";
    message_texts[71] = "System error";
    message_texts[72] = "Critical OS file missing";
    message_texts[73] = "Can't create (user) output file";
    message_texts[74] = "Input/output error";
    message_texts[75] = "Temp failure; user is invited to retry";
    message_texts[76] = "Remote error in protocol";
    message_texts[77] = "Permission denied";
    message_texts[78] = "Configuration error";


    /* man 7 signal */
    message_texts[128 + 1]  = "Hangup detected on controlling terminal"\
                              "or death of controlling process";
    message_texts[128 + 2]  = "Interrupt from keyboard";
    message_texts[128 + 3]  = "Quit from keyboard";
    message_texts[128 + 4]  = "Illegal Instruction";
    message_texts[128 + 5]  = "";
    message_texts[128 + 6]  = "Abort signal from abort(3)";
    message_texts[128 + 7]  = "Bus error (bad memory access)";
    message_texts[128 + 8]  = "Floating-point exception";
    message_texts[128 + 9]  = "Kill signal";

    message_texts[128 + 11] = "Invalid memory reference";

    message_texts[128 + 13] = "Broken pipe: write to pipe with no readers; see pipe(7)";
    message_texts[128 + 14] = "Timer signal from alarm(2)";
    message_texts[128 + 15] = "Termination signal";
}


/* Exit message generator 
 * converts status_code to
 * message string *str  */
void get_exit_message(char *str, int status_code) {
    if (status_code < 0 || status_code >= N_CODES) {
        sprintf(str, "[Exited with code %3d]: %s", status_code, message_texts[255]);
        return;
    }

    if (message_texts[status_code]) {
        sprintf(str, "[Exited with code %3d]: %s", status_code, message_texts[status_code]);
        return;
    }

    if (status_code > 128) {
        sprintf(str, "[Exited with code %3d]: Fatal error signal %d", status_code, status_code - 128);
        return;
    }

    sprintf(str, "[Exited with code %3d]", status_code);
}
