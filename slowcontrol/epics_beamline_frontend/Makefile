#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for MIDAS slow control example under NT
#
#  Revision history
#  ------------------------------------------------------------------
#  date         by    modification
#  ---------    ---   -----------------------------------------------
#  13-NOV-97    SR    created
#
#####################################################################


#-------------------------------------------------------------------
# The following lines define directories. Adjust if necessary
#                 
#INC_DIR 	= $(MIDASSYS)/include
#MIDAS_DIR       = /afs/psi.ch/user/r/raselli/midas/musr-1.9.5
INC_DIR 	= $(MIDAS_HOME)/include
#LIB_DIR        = $(MIDAS_HOME)/linux/$(HOSTNAME)/lib
LIB_DIR         = $(MIDAS_HOME)/linux64/lib
#BIN_DIR        = $(MIDAS_HOME)/linux/$(HOSTNAME)/bin
BIN_DIR         = $(MIDAS_HOME)/linux64/bin
DRV_DIR		= $(MIDAS_HOME)/drivers
#EPICS_DIR      = /usr/local/epics/base
EPICS_DIR       = $(HOME)/installation//base-3.15.0.1

MAKEFILE = Makefile

####################################################################
# Lines below here should not be edited
####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Linux ----------------
LIBS = -lbsd -lm -lutil -lpthread 
OSFLAGS = -DOS_LINUX -fPIC #-static $(MIDAS_PREF_FLAGS) $(USERFLAGS)

# This is for OSF1 -----------------
#LIBS = -lbsd
#OSFLAGS = -DOS_OSF1

# This is for Ultrix ---------------
#LIBS =
#OSFLAGS = -DOS_ULTRIX -Dextname

# This is for FreeBSD --------------
#LIBS = -lbsd -lcompat
#OSFLAGS = -DOS_FREEBSD


LIBRARY = $(LIB_DIR)/libmidas.a
#LIB = $(LIBRARY) -Wl,-rpath,$(EPICS_DIR)/lib/SL3-x86 -lca -lCom
#LIB = $(LIBRARY) -Wl,-rpath,$(EPICS_DIR)/lib/SL6-x86 -lca -lCom
LIB = $(LIBRARY) -Wl,-rpath,$(EPICS_DIR)/lib/linux-x86_64 -lca -lCom

# compiler
CC = cc
CFLAGS = -g -I$(INC_DIR) -I$(DRV_DIR)
#LDFLAGS = -L$(LIB_DIR) -L$(EPICS_DIR)/lib/SL3-x86 
#LDFLAGS = -L$(LIB_DIR) -L$(EPICS_DIR)/lib/SL6-x86
LDFLAGS = -L$(LIB_DIR) -L$(EPICS_DIR)/lib/linux-x86_64 

MIDAS_H    = $(INC_DIR)/midas.h
GENERIC_H  = $(DRV_DIR)/class/generic.h
SPLINE_H   = $(DVM_DIR)/spline.h $(MIDAS_H)
NULL_H     = $(DRV_DIR)/bus/null.h
LANGPIB_H  = $(DRV_DIR)/bus/langpib.h
TCPIP_H    = $(DRV_DIR)/bus/tcpip.h
#EPICSCA_H  = $(DRV_DIR)/device/epics_ca.h $(DRV_DIR)/device/epics_ca_private.h
EPICSCA_H  = ./epics_ca.h ./epics_ca_private.h

all: EPICSGFA_eblpie1_scfe 

#
# -- EPICSGFA epics secondary beamlines or beamline components
#
EPICSGFA_eblpie1.o: EPICSGFA_eblpie1.c $(MIDAS_H) $(GENERIC_H) $(NULL_H) $(EPICSCA_H) \
                   $(MAKEFILE)
	$(CC) $(CFLAGS) -I$(DRV_DIR)/class -I$(DRV_DIR)/device -I$(DRV_DIR)/bus \
         $(OSFLAGS) -c -o $@ $<

epics_ca.o: ./epics_ca.c $(MIDAS_H) $(EPICSCA_H) $(MAKEFILE)
	$(CC) $(CFLAGS) -I$(DRV_DIR)/class -I$(DRV_DIR)/device -I$(EPICS_DIR)/include \
         -I$(EPICS_DIR)/include/os/Linux $(OSFLAGS) -c -o $@ $<

null.o: $(DRV_DIR)/bus/null.c $(MIDAS_H) $(NULL_H) $(MAKEFILE)
	$(CC) $(CFLAGS) -I$(DRV_DIR)/bus $(OSFLAGS) -c -o $@ $<

$(LIB_DIR)/generic.o: $(DRV_DIR)/class/generic.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

EBLPIE1_OBJS = $(LIB_DIR)/mfe.o EPICSGFA_eblpie1.o $(LIB_DIR)/generic.o \
              epics_ca.o $(LIB_DIR)/ybos.o null.o

EPICSGFA_eblpie1_scfe: $(EBLPIE1_OBJS) $(MAKEFILE)
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(EBLPIE1_OBJS) $(LDFLAGS) $(LIB) $(LIBS) -lm

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*

