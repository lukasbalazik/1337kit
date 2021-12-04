#include "runshell.h"


void run_custom_bash(char *commands)
{
    char *argv[] = {"/bin/bash","-c","", NULL};
    char *envp[] = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL };
    argv[2] = commands;
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

