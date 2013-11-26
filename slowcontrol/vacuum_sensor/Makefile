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

LIBS = -lm -lz -lutil -lnsl -lpthread

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
SRC_DIR         = $(MIDASSYS)
DRV_DIR         = $(MIDASSYS)/drivers
INC_DIR 	= $(MIDASSYS)/include
LIB_DIR 	= $(MIDASSYS)/linux64/lib

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = gcc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)

all: fevacgauge

fevacgauge: $(LIB) $(LIB_DIR)/mfe.o frontend.o
	$(CC) $(OSFLAGS) -o fevacgauge $(LIB_DIR)/mfe.o frontend.o $(LIB) $(LDFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*

