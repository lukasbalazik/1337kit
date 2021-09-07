#ifndef HIDEIP_H
#define HIDEIP_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

void hide_ip(char *);
void show_hidden_ip(char *);

#endif
