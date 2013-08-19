#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     UNIX Makefile for MIDAS EPICS slow control frontend
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
#  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#  (Next, I'll commit our local customizations.)
#
#  Revision 1.3  2000/03/03 01:11:22  pierre
#  - Added EPICS_DIR for Epics library
#
#  Revision 1.2  1999/12/21 09:37:38  midas
#  Adjusted for new driver names
#
#  Revision 1.1  1999/09/22 12:24:02  midas
#  *** empty log message ***
#
#
#####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Linux ----------------
LIBS = -lbsd -lm -lutil
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
EPICS_DIR       = /usr/local/lib
#-------------------------------------------------------------------
# Drivers needed by the frontend program
#                 
DRIVERS         = camacnul.o generic.o epics_ca.o

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a $(EPICS_DIR)/libca.a $(EPICS_DIR)/libCom.a

# compiler
CC = cc
CFLAGS = -g -I. -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS =

all: fe_epics

fe_epics:  $(LIB) $(LIB_DIR)/mfe.o frontend.o $(DRIVERS)
	$(CC) -o fe_epics frontend.o $(LIB_DIR)/mfe.o $(DRIVERS) $(LIB) $(LDFLAGS) $(LIBS)

camacnul.o: $(DRV_DIR)/bus/camacnul.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

generic.o: $(DRV_DIR)/class/generic.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

epics_ca.o: $(DRV_DIR)/device/epics_ca.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*
