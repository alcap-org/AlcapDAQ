# Makefile for vmedrv
# Created by Enomoto Sanshiro on 28 November 1999.
# Last updated by Enomoto Sanshiro on 21 January 2007.


MODULE_NAME = vmedrv
MAJOR_ID = 0
TARGET_CARD_NUMBER = 0


SYSTEM = STANDARD
# SYSTEM = FC2
# SYSTEM = FC3

SRC = vmedrv

obj-m = $(MODULE_NAME).o
KERNEL_VERSION = $(shell uname -r)
PWD = $(shell pwd)


### Use CFLAGS for old kernels (<2.6.25 ??) ###
EXTRA_CFLAGS += -DSYSTEM_$(SYSTEM)
#CFLAGS += -DSYSTEM_$(SYSTEM)

### Flags for degugging (Use CFLAGS for old kernels (<2.6.25 ??)) ###
#EXTRA_CFLAGS += -DTRACE_CONFIG
#EXTRA_CFLAGS += -DTRACE_PIO
#EXTRA_CFLAGS += -DTRACE_MAP
#EXTRA_CFLAGS += -DTRACE_INTERRUPT
#EXTRA_CFLAGS += -DTRACE_DMA


default:
	@if [ $(MODULE_NAME) != $(SRC) ]; then \
		ln -f $(SRC).c $(MODULE_NAME).c; \
	fi
	make -C /lib/modules/$(KERNEL_VERSION)/build SUBDIRS=$(PWD) modules
	cp vmedrv.h ..

clean:
	rm -f $(MODULE_NAME).ko $(MODULE_NAME).o $(MODULE_NAME).mod.*


install:
	/sbin/insmod $(MODULE_NAME).ko vmedrv_major=$(MAJOR_ID) vmedrv_name=$(MODULE_NAME) target_card_number=$(TARGET_CARD_NUMBER)
	@./install.sh $(MODULE_NAME)


uninstall:
	@/sbin/rmmod $(MODULE_NAME)
