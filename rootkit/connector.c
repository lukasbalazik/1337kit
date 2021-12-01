#include "connector.h"

int size = 0;

void (**funcp)(char *);

struct socket *conn_socket = NULL;

struct entry {
    char *str;
    int n;
};

struct entry *commands;

unsigned int inet_addr(char *str)
{
    int a, b, c, d;
    char arr[4];
    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
    return *(unsigned int *)arr;
}

int key_value(struct entry *c, char *key)
{
    int i = 0;
    char *name = c[i].str;
    while (name) {
        if (strcmp(name, key) == 0)
            return c[i].n;
        name = c[++i].str;
    }
    return -1;
}

void add_function(char *c, void *func)
{
	struct entry command = {c, size};
	size++;
	commands = (struct entry *) krealloc(commands, size * sizeof(struct entry), GFP_KERNEL);
	funcp = (void *) krealloc(funcp, size*(sizeof(void(*)(char))), GFP_KERNEL);
	*(funcp + size - 1) = func;
	*(commands + size - 1) = command;
}

void clear_functions(void)
{
	kfree(funcp);
	kfree(commands);
}

u32 create_address(u8 *ip)
{
	u32 addr = 0;
	int i;

	for(i=0; i<4; i++) {
		addr += ip[i];
		if(i==3)
			break;
		addr <<= 8;
	}
	return addr;
}

int receive_message(struct socket *sock, char *str, unsigned long flags)
{
	struct msghdr msg;

	struct kvec vec;
	int len;
	int max_size = MAX_SIZE;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;

	vec.iov_len = max_size;
	vec.iov_base = str;

read_again:
	len = kernel_recvmsg(sock, &msg, &vec, max_size, max_size, flags);

	if (len == -EAGAIN || len == -ERESTARTSYS) {
		goto read_again;
	}

	return len;
}

int connector(char *ip, int port)
{
	struct sockaddr_in saddr;
	char *response;
	int ret = -1;
	char *first_part;
	int size;
	int i;

  long command_number;

	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &conn_socket);
	if (ret < 0) {
		return -1;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	ret = conn_socket->ops->connect(conn_socket, (struct sockaddr *)&saddr, sizeof(saddr), O_RDWR);
	if (ret && (ret != -EINPROGRESS)) {
		return -1;
	}

	response = (char *) kmalloc(MAX_SIZE, GFP_KERNEL);

	while (1) {
		memset(response, 0, MAX_SIZE);
		receive_message(conn_socket, response, MSG_WAITFORONE);

		printk(KERN_INFO "Received Message: %s", response);
		size = strlen(response);
        if (size < 1)
            break;
		first_part = (char *) kmalloc(size, GFP_KERNEL);

		for (i = 0; i < size; i++) {
			if (*(response+i) == '\n' || *(response+i) == ' ') {
				break;
			}
			*(first_part + i) = *(response + i);
		}
		*(first_part+i) = 0x00;
		i = kstrtol(first_part, 10, &command_number);

		if (i < 0) {
			command_number = key_value(commands, first_part);
			if (command_number < 0) {
				break;
			}
		}
		kfree(first_part);

		for (i = 0; i < size; i++) {
			if (*(response+i) == '\n' || *(response+i) == ' ') {
				response += i + 1;
				break;
			}
		}

		if (command_number == 9) {
			break;
		}
		if (command_number <= size && command_number >=0) {
			funcp[command_number](response);
		}
	}

	kfree(response);
	return 0;
}

