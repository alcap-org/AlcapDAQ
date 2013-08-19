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
#  Revision 1.22  2003/12/12 17:40:29  midas
#  Added experim.h dependency
#
#  Revision 1.21  2003/08/19 23:39:48  pierre
#  add pthread lib
#
#  Revision 1.20  2003/07/14 14:53:11  midas
#  Restructured driver directory
#
#  Revision 1.19  2003/05/09 22:52:15  pierre
#  Add ROOTSYS condition on ROOT build
#
#  Revision 1.18  2003/04/28 10:22:47  midas
#  Makefile only compiles ROOT analyzer
#
#  Revision 1.17  2003/04/24 14:09:59  midas
#  Added more ROOT libraries
#
#  Revision 1.16  2003/04/21 04:04:03  olchansk
#  add building "ranalyzer" - the ROOT analyzer
#  replace MANA_LITE with HAVE_HBOOK and HAVE_ROOT
#  compile HBOOK modules into h*.o, ROOT modules into r*.o
#  the HBOOK analyzer is still called "analyzer"
#
#  Revision 1.15  2003/04/14 13:46:30  midas
#  Fixed bug that camacnu.o was not compiled after a 'make clean'
#
#  Revision 1.14  2003/04/08 00:13:07  olchansk
#  add ranalyzer, the ROOT MIDAS analyzer
#
#  Revision 1.13  2002/05/10 05:23:18  pierre
#  add MANA_LITE option
#
#  Revision 1.12  2000/12/01 09:35:18  midas
#  Added fal
#
#  Revision 1.11  2000/10/02 14:04:08  midas
#  Added -lzlib for OSF1
#
#  Revision 1.10  2000/05/18 13:18:33  midas
#  Added -lnsl for linux (problems with CERNLIB 2000)
#
#  Revision 1.9  2000/03/15 12:48:36  midas
#  Added -Dextname for linux
#
#  Revision 1.8  1999/12/22 08:57:27  midas
#  Use ../../drivers directory
#
#  Revision 1.7  1999/12/22 08:53:56  midas
#  Changed /midas/drivers to ../../drivers
#
#  Revision 1.6  1999/12/22 00:05:00  pierre
#  - Set default driver to camacnul
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
# This is for Linux
#
OSFLAGS = -DOS_LINUX -Dextname
CFLAGS = -g -O2 -Wall
LIBS = -lm -lz -lutil -lnsl -lpthread

#-----------------------------------------
# ROOT flags and libs
#
ifdef ROOTSYS
ROOTCFLAGS := $(shell  $(ROOTSYS)/bin/root-config --cflags)
ROOTCFLAGS += -DHAVE_ROOT
ROOTLIBS   := $(shell  $(ROOTSYS)/bin/root-config --libs)
ROOTLIBS   += -lThread
endif
#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 
DRV_DIR         = ../../drivers/bus
INC_DIR 	= ../../include
LIB_DIR 	= ../../linux/lib
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
CC = gcc
CXX = g++
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS +=

all: $(UFE) analyzer

$(UFE): $(LIB) $(LIB_DIR)/mfe.o $(DRIVER).o $(UFE).c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $(UFE) $(UFE).c $(DRIVER).o \
	$(LIB_DIR)/mfe.o $(LIB) $(LDFEFLAGS) $(LIBS)

$(DRIVER).o: $(DRV_DIR)/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/$(DRIVER).c

analyzer: $(LIB) $(LIB_DIR)/rmana.o analyzer.o $(MODULES)
	$(CXX) $(FFLAGS) -o $@ $(LIB_DIR)/rmana.o analyzer.o $(MODULES) \
	$(LIB) $(LDFLAGS) $(ROOTLIBS) $(LIBS)

%.o: %.c experim.h
	$(CXX) $(USERFLAGS) $(ROOTCFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

clean::
	rm -f *.o *~ \#*

#end file
