#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     UNIX Makefile for MIDAS slow control frontend
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
#  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#  (Next, I'll commit our local customizations.)
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

# This is for Linux ----------------
LIBS = -lbsd -lm -lutil -lpthread
OSFLAGS = -DOS_LINUX

# This is for OSF1 -----------------
#LIBS = -lbsd
#OSFLAGS = -DOS_OSF1

# This is for Ultrix ---------------
#LIBS =
#OSFLAGS = -DOS_ULTRIX -Dextname

# This is for FreeBSD --------------
#LIBS = -lbsd -lcompat
#OSFLAGS = -DOS_FREEBSD

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
INC_DIR 	= /usr/local/include
LIB_DIR 	= /usr/local/lib
DRV_DIR		= ../../drivers

#-------------------------------------------------------------------
# Drivers needed by the frontend program
#                 
DRIVERS         = camacnul.o hv.o multi.o null.o nulldev.o

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS = -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS =

all: scfe

scfe:  $(LIB) $(LIB_DIR)/mfe.o frontend.o $(DRIVERS)
	$(CC) -o scfe frontend.o $(LIB_DIR)/mfe.o $(DRIVERS) $(LIB) $(LDFLAGS) $(LIBS)

camacnul.o: $(DRV_DIR)/bus/camacnul.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

hv.o: $(DRV_DIR)/class/hv.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

multi.o: $(DRV_DIR)/class/multi.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

nulldev.o: $(DRV_DIR)/device/nulldev.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

null.o: $(DRV_DIR)/bus/null.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*

