PWD := $(shell pwd)
KERNEL := /home/thanhtung/linux/
CROSS := /opt/gcc-arm-linux/bin/arm-linux-gnueabihf-
obj-m += led.o

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean
