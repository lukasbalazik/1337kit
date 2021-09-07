#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>

#define MAX_SIZE 1024

int connector(char *, int);
void clear_functions(void);
void add_function(char *, void *); 

#endif
