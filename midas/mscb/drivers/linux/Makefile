#
# Makefile for scs_250 kernel driver
#

# Location of kernel sources, change if necessary
KERNEL_SRC = /usr/src/linux/
KERNEL_RELEASE = $(shell uname -r)

# Location where driver module will be placed
# Following line is for RedHat 7.1
MODULE_DIR = /lib/modules/$(KERNEL_RELEASE)/kernel/drivers/char/

all: kscs_250.o

kscs_250.o: kscs_250.c
	cc -O -c -I$(KERNEL_SRC)include kscs_250.c

install: kscs_250.o
	rm -f /dev/usb/scs_250
	mknod /dev/usb/scs_250 c 180 193
	install -m 644 kscs_250.o $(MODULE_DIR)
	@if [ -n "`/sbin/lsmod | grep kscs_250`" ]; then \
	  echo rmmod kscs_250; \
	  rmmod kscs_250; \
	fi
	insmod kscs_250.o

clean:
	rm -f *.o *~ core kscs_250.o

