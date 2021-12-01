#include "hideip.h"
#include "../hooker/tcp_seq_show.h"

void hide_ip(char *content)
{
	add_hidden_ip(content);
}

void show_hidden_ip(char *content) 
{
	remove_hidden_ip(content);
}
