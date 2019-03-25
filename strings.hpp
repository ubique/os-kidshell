#ifndef NSH_STRINGS_HPP
#define NSH_STRINGS_HPP

const char* WELCOME_MESSAGE = R"(Welcome to nsh!

Type `help' to get information about the shell.)";

const char* USAGE_MESSAGE = R"(Usage: nsh [-c command | -h | --help])";

const char* HELP_MESSAGE = R"(nsh, version 1.0

nsh is Not-a-SHell, simple command interpreter.

SYNTAX

To launch a program, just type path to it:
    /home/user/binary

To find executable in PATH, use program name instead of full path:
    passwd

To pass custom environment variables, type them before the program name:
    PORT=4000 ./server

To pass command line arguments, type them after the program name:
    ./server --host=0.0.0.0

To use space inside path, argument or environment variable, escape it with a backslash:
    /home/user/My\ Travel\ Blog/program welcome

To use backslash, type two backslashes:
    echo \\

To use character `=' in command name, escape it with a backslash:
    DB_HOST=127.0.0.1 /home/app\=name

INTERNAL COMMANDS

nsh provides two commands:
    help          Displays this help message
    exit          Quits the shell

COMMAND LINE INTERFACE

When running without arguments, interactive shell is launched.

You can use these arguments:
    -h, --help    Displays this help message
    -c <command>  Runs provided command and returns its exit code)";

#endif
