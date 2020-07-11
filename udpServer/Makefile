obj-m := udpServer.o
   
KVERSION = $(shell uname -r)
   
udpServer-objs += coreApp.o
udpServer-objs += UDPServer.o
 
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean