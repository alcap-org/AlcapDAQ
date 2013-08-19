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
#  Revision 1.4  2003/08/20 21:02:00  pierre
#  add phtread lib
#
#  Revision 1.3  2001/12/17 18:39:34  pierre
#  remove analyzer modules
#
#  Revision 1.2  2001/12/17 18:35:16  pierre
#  update for newer version
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
# This is for OSF1
#LIBS = -lbsd -lzlib
#OSFLAGS = -DOS_OSF1 -Dextname
#FF = f77
#FFLAGS = -nofor_main -D 40000000 -T 20000000

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
DRV_DIR         = ../../drivers
INC_DIR 	= /usr/local/include
LIB_DIR 	= /usr/local/lib
CERNLIB_DIR	= /cern/pro/lib

#-------------------------------------------------------------------
# List of analyzer modules
#                 
MODULES 	= 

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

all: $(UFE) 

$(UFE): $(LIB) $(LIB_DIR)/mfe.o $(DRV_DIR)/bus/$(DRIVER).o $(UFE).c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $(UFE) $(UFE).c $(DRIVER).o \
	$(LIB_DIR)/mfe.o $(LIB) $(LDFEFLAGS) $(LIBS)

$(DRIVER).o: $(DRV_DIR)/bus/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/bus/$(DRIVER).c

analyzer: $(LIB) $(LIB_DIR)/mana.o analyzer.o $(MODULES)
	$(FF) $(FFLAGS) -o analyzer $(LIB_DIR)/mana.o analyzer.o \
	$(MODULES) $(CERNLIB_DIR)/libpacklib.a \
	$(LIB) $(LDFLAGS) $(LIBS)

fal: $(LIB) $(LIB_DIR)/fal.o $(DRV_DIR)/bus/$(DRIVER).o $(UFE).c $(MODULES) $(UFE).o analyzer.o
	$(FF) $(FFLAGS) -o fal $(LIB_DIR)/fal.o $(UFE).o $(DRIVER).o \
	analyzer.o $(MODULES) $(CERNLIB_DIR)/libpacklib.a \
	$(LIB) $(LDFEFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*
