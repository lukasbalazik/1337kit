#include "hidedent.h"
#include "../hooker/getdents.h"


void hide_dent(char *content)
{
	if (*(content + strlen(content) - 1) == '\n') {
		*(content + strlen(content) - 1) = 0x00;
	}
	add_hidden_dent(content);
}

void show_hidden_dent(char *content)
{
	if (*(content + strlen(content) - 1) == '\n') {
		*(content + strlen(content) - 1) = 0x00;
	}
	remove_hidden_dent(content);
}
