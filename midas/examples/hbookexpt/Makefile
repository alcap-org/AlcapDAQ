#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for MIDAS example frontend and analyzer
#                using HBOOK functionality
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2005/06/20 23:37:11  mucap
#  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#  (Next, I'll commit our local customizations.)
#
#  Revision 1.6  2003/12/12 17:40:29  midas
#  Added experim.h dependency
#
#  Revision 1.5  2003/08/20 21:00:46  pierre
#  add phtread lib
#
#  Revision 1.4  2003/07/14 14:51:45  midas
#  Restructured driver directory
#
#  Revision 1.3  2003/05/09 09:58:23  midas
#  Removed ranalyzer from Makefile
#
#  Revision 1.2  2003/04/25 13:27:13  midas
#  Link against hmana.o
#
#  Revision 1.1  2003/04/25 13:20:07  midas
#  Initial revision
#
#
#####################################################################
#

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

#-----------------------------------------
# This is for Linux
OSFLAGS = -DOS_LINUX -Dextname
CFLAGS = -O3

# for f2c, uncomment following:
#LIBS = -lbsd -lm -lutil /usr/lib/libf2c.a
#FF = cc
# for egcs g77, use this
FF = g77
LIBS = -lm -lz -lutil -lnsl -lpthread

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
DRV_DIR         = ../../drivers/bus
INC_DIR 	= /usr/local/include
LIB_DIR 	= /usr/local/lib
#
# Link with or without CERNLIB
ifndef USERFLAGS
CERNLIB_PACK	= /cern/pro/lib/libpacklib.a
endif
#
#-------------------------------------------------------------------
# List of analyzer modules
#                 
MODULES 	= adccalib.o adcsum.o scaler.o

#-------------------------------------------------------------------
# Hardware driver can be (camacnul, kcs2926, kcs2927, hyt1331)
#
DRIVER = camacnul

#-------------------------------------------------------------------
# Frontend code name defaulted to frontend in this example.
# comment out the line and run your own frontend as follow:
# gmake UFE=my_frontend
#
UFE = frontend

####################################################################
# Lines below here should not be edited
####################################################################

# MIDAS library
LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS +=

all: $(UFE) fal analyzer

$(UFE): $(LIB) $(LIB_DIR)/mfe.o $(DRIVER).o $(UFE).c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $(UFE) $(UFE).c $(DRIVER).o \
	$(LIB_DIR)/mfe.o $(LIB) $(LDFEFLAGS) $(LIBS)

$(DRIVER).o: $(DRV_DIR)/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/$(DRIVER).c

analyzer: $(LIB) $(LIB_DIR)/hmana.o analyzer.o $(MODULES)
	$(FF) $(FFLAGS) -o analyzer $(LIB_DIR)/hmana.o analyzer.o \
	$(MODULES) $(CERNLIB_PACK) \
	$(LIB) $(LDFLAGS) $(LIBS)

fal: $(LIB) $(LIB_DIR)/fal.o $(DRIVER).o $(UFE).c $(MODULES) $(UFE).o analyzer.o
	$(FF) $(FFLAGS) -o fal $(LIB_DIR)/fal.o $(UFE).o $(DRIVER).o \
	analyzer.o $(MODULES) $(CERNLIB_PACK) \
	$(LIB) $(LDFEFLAGS) $(LIBS)

.c.o: experim.h
	$(CC) $(USERFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

clean:
	rm -f *.o *~ \#*
