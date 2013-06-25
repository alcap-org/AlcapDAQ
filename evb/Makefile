#####################################################################
#
#  Name:         Makefile
#  Created by:   Pierre-Andre Amaudruz
#
#  Contents:     Makefile for mevb MIDAS example 
#
#  $Log: Makefile,v $
#  Revision 1.5  2005/10/31 14:11:37  mucap
#  (Fred)
#  Update to new major version of MIDAS event builder.
#
#  Revision 1.4  2004/05/31 14:26:34  mucap
#  Commented out compression.  (Leftover uncommitted change from run7)
#
#  Revision 1.3  2003/07/17 17:04:23  fegray
#  Added lossless online compression.
#
#  Revision 1.2  2003/05/27 16:26:20  mucap
#  Update to CVS version of MIDAS.
#
#  Revision 1.1  2003/05/13 16:21:16  mucap
#  First commit of MIDAS (Ethernet-based) event-builder.
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
CFLAGS = -O0

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

#-------------------------------------------------------------------
# Hardware driver can be (camacnul, kcs2926, kcs2927, hyt1331)
#
DRIVER = camacnul

####################################################################
# Lines below here should not be edited
####################################################################

# MIDAS library
LIB = $(LIB_DIR)/libmidas.a ../compress/libmucap_compress.a

# compiler
CC = cc
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR) -Wall -I../compress
LDFLAGS +=

all: mevb 

mevb:   $(LIB) mevb.cpp ebuser.o
	$(CXX) $(CFLAGS) $(OSFLAGS) -o mevb mevb.cpp ebuser.o $(LIB) $(LDFEFLAGS) $(LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ mevb \#*


