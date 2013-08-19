#####################################################################
#
#  Name:         Makefile
#  Created by:   Pierre-Andre Amaudruz
#
#  Contents:     Makefile for mevb MIDAS example 
#
#  $Log: Makefile,v $
#  Revision 1.1.1.1  2005/06/20 23:37:10  mucap
#  Importing release 1.9.5 of the MIDAS source code as distributed by PSI.
#  (Next, I'll commit our local customizations.)
#
#  Revision 1.4  2004/10/07 20:08:34  pierre
#  1.9.5
#
#  Revision 1.3  2004/09/28 16:16:39  pierre
#  Add MIDASSYS
#
#  Revision 1.2  2003/08/19 23:25:29  pierre
#  add pthread lib
#
#  Revision 1.1.1.2  2002/01/17 21:37:44  pierre
#  Initial Version
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
CFLAGS = 

# for f2c, uncomment following:
#LIBS = -lbsd -lm -lutil /usr/lib/libf2c.a
#FF = cc
# for egcs g77, use this
FF = g77
LIBS = -lm -lz -lutil -lnsl -lpthread

#-------------------------------------------------------------------
# The following lines define direcories. Adjust it if necessary.
#                 
DRV_DIR         = $(MIDASSYS)/drivers
INC_DIR         = $(MIDASSYS)/include
LIB_DIR         = $(MIDASSYS)/linux/lib
CERNLIB_DIR     = /cern/pro/lib

#-------------------------------------------------------------------
# Hardware driver can be (camacnul, kcs2926, kcs2927, hyt1331)
#
DRIVER = camacnul

####################################################################
# Lines below here should not be edited
####################################################################

# MIDAS library
LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR)
LDFLAGS +=

all: mevb ebfe

ebfe: $(LIB) $(LIB_DIR)/mfe.o $(DRV_DIR)/bus/$(DRIVER).o ebfe.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o ebfe ebfe.c $(DRIVER).o \
	$(LIB_DIR)/mfe.o $(LIB) $(LDFEFLAGS) $(LIBS)

$(DRIVER).o: $(DRV_DIR)/bus/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/bus/$(DRIVER).c

mevb:   $(LIB) mevb.c ebuser.o
	$(CC) $(CFLAGS) $(OSFLAGS) -o mevb mevb.c ebuser.o $(LIB) $(LDFEFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ mevb \#*


