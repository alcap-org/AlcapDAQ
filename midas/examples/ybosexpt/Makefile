#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for MIDAS example frontend and analyzer
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
#  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#  (Next, I'll commit our local customizations.)
#
#  Revision 1.1  1999/12/21 19:51:15  pierre
#  - UNIX like makefile for ybos examples
#
#  Revision 1.5  1999/12/21 10:15:29  midas
#  Removed kernel flags
#
#  Revision 1.4  1999/12/20 15:33:39  midas
#  Adjustments for different driver directories
#
#  Revision 1.3  1999/12/10 00:45:48  pierre
#  - Add DRIVER selection and UFE option
#
#####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

#-----------------------------------------
# This is for OSF1
#LIBS = -lbsd
#OSFLAGS = -DOS_OSF1 -Dextname
#FF = f77
#FFLAGS = -nofor_main -D 40000000 -T 20000000

#-----------------------------------------
# This is for Linux
OSFLAGS = -DOS_LINUX 
CFLAGS = -O3

# for f2c, uncomment following:
#LIBS = -lbsd -lm -lutil /usr/lib/libf2c.a
#FF = cc
# for egcs g77, use this
FF = g77
LIBS = -lm -lz -lutil

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
DRV_DIR         = ../../drivers
INC_DIR 	= /usr/local/include
LIB_DIR 	= /usr/local/lib
CERNLIB_DIR	= /cern/pro/lib

#-------------------------------------------------------------------
# Hardware driver can be (camacnul, kcs2926, kcs2927, hyt1331)
#
DRIVER = camacnul

#-------------------------------------------------------------------
# Frontend code name defaulted to frontend in this example.
# comment out the line and run your own frontend as follow:
# gmake UFE=my_frontend
#
UFE = ybos_simfe

####################################################################
# Lines below here should not be edited
####################################################################

# MIDAS library
LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS +=

all: $(UFE) $(DRV_DIR)/bus/esone.c

$(UFE): $(LIB) $(LIB_DIR)/mfe.o $(DRIVER).o $(UFE).o esone.o
	$(CC) $(CFLAGS) $(OSFLAGS) -o $(UFE) $(UFE).c $(DRIVER).o esone.o\
	$(LIB_DIR)/mfe.o $(LIB) $(LDFEFLAGS) $(LIBS)

$(DRIVER).o: $(DRV_DIR)/bus/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $@ $<

esone.o: $(DRV_DIR)/bus/esone.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ \#*




