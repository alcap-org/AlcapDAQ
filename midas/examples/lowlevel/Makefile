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
#  Revision 1.3  2003/08/20 21:01:39  pierre
#  add phtread lib
#
#  Revision 1.2  1999/10/26 12:08:31  midas
#  Switched to shared midas library
#
#  Revision 1.1  1999/10/26 12:03:07  midas
#  Added file
#
#
#####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Linux ----------------
LIBS = -lutil -lpthread
OSFLAGS = -DOS_LINUX -fPIC

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

####################################################################
# Lines below here should not be edited
####################################################################

LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS = -g -I$(INC_DIR) -L$(LIB_DIR)

PROGS = produce consume rpc_test rpc_clnt rpc_srvr
all: $(PROGS)

$(PROGS): %: %.c $(LIB)
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $< -lmidas $(LIBS)

clean:
	rm -f $(PROGS) *~ \#*

