#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     UNIX Makefile for MIDAS slow control frontend
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2004/10/20 18:41:18  admin
#  Trying again to import midas-1.9.5.  There are NT executables, but I'm importing them as well (w/o -kb switch so watch out!)
#
#  Revision 1.3  2003/08/20 21:02:12  pierre
#  add phtread lib
#
#  Revision 1.2  2002/05/29 13:36:42  midas
#  Added nulldev driver
#
#  Revision 1.1  1999/12/21 09:39:00  midas
#  Added source
#
#
#####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Linux
OSFLAGS = -DOS_LINUX -Dextname
CFLAGS = -O0 -g

# for f2c, uncomment following:
#LIBS = -lbsd -lm -lutil /usr/lib/libf2c.a
#FF = cc
# for egcs g77, use this
FF = g77
LIBS = -lm -lz -lutil -lnsl -lpthread

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
SRC_DIR         = $(MIDAS_ROOT)
DRV_DIR         = $(MIDAS_ROOT)/drivers
INC_DIR 	= $(MIDAS_ROOT)/include
LIB_DIR 	= $(MIDAS_ROOT)/linux64/lib

#-------------------------------------------------------------------
# Drivers needed by the frontend program
#                 
DRIVERS         = 

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS +=

all: feseparator

$(DRIVER).o: $(DRV_DIR)/bus/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/bus/$(DRIVER).c

nulldev.o: $(DRV_DIR)/device/nulldev.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o nulldev.o $(DRV_DIR)/device/nulldev.c

null.o: $(DRV_DIR)/bus/null.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o null.o $(DRV_DIR)/bus/null.c

hv.o: $(DRV_DIR)/class/hv.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o hv.o $(DRV_DIR)/class/hv.c

multi.o: $(DRV_DIR)/class/multi.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o multi.o $(DRV_DIR)/class/multi.c

camacnul.o: $(DRV_DIR)/bus/camacnul.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o camacnul.o $(DRV_DIR)/bus/camacnul.c

feseparator:  $(LIB) $(LIB_DIR)/mfe.o frontend.o $(DRIVERS) 
	$(CC) $(FFLAGS) $(OSFLAGS) -o feseparator $(LIB_DIR)/mfe.o \
        frontend.o $(DRIVERS) \
        $(LIB) $(LDFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*

