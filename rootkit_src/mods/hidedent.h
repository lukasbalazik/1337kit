#ifndef HIDEDENT_H
#define HIDEDENT_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

void hide_dent(char *);
void show_hidden_dent(char *);
//void showme(char *);

#endif
