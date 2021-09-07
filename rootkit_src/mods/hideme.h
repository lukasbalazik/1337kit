#ifndef HIDEME_H
#define HIDEME_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

void hideme(char *);
void showme(char *);

#endif
