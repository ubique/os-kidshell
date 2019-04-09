#include "KidShell.h"

int main(int argc, char *argv[], char *envp[]) {
    KidShell kidShell(envp);
    kidShell.run();
}