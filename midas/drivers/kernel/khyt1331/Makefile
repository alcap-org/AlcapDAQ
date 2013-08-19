#
# Makefile for camactest example
#

# Location of kernel sources, change if necessary
KERNEL_SRC = /usr/src/linux/
KERNEL_RELEASE = $(shell uname -r)

# Location where driver module will be placed
# Following line is for RedHat 7.1
MODULE_DIR = /lib/modules/$(KERNEL_RELEASE)/kernel/drivers/char/

# Following line is for RedHat 6.2, outcomment if necessary
#MODULE_DIR = /lib/modules/$(KERNEL_RELEASE)/misc/

all: camactest esonetest khyt1331.o libcamac.a

camactest: camactest.c libcamac.a
	cc -g -o camactest camactest.c libcamac.a

esonetest: esonetest.c libcamac.a
	cc -g -o esonetest esonetest.c libcamac.a

libcamac.a: camac.c esone.c
	cc -g -c camac.c
	cc -g -c esone.c
	rm -f libcamac.a
	ar -cr libcamac.a camac.o esone.o

khyt1331.o: khyt1331.c
	cc -O -c -I$(KERNEL_SRC)include khyt1331.c

install: khyt1331.o
	rm -f /dev/camac
	mknod /dev/camac c 60 0
	install -m 644 khyt1331.o $(MODULE_DIR)
	@if [ -n "`/sbin/lsmod | grep khyt1331`" ]; then \
	  echo rmmod khyt1331; \
	  rmmod khyt1331; \
	fi
	insmod khyt1331.o

clean:
	rm -f *.o *~ core camactest esonetest libcamac.a
