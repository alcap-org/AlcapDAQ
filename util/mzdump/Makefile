
#####################################################################
# Lines below should be edited for the appropriate directories
#####################################################################

#The following if statements set the lib path for 64-bit or 32-bit linux
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),32)
  LINUXARCH=linux
else
  LINUXARCH=linux64
endif

#-------------------------------------------------------------------
# this is for MIDAS, STRUCK libraries

#MIDAS_DIR     =  ../../midas-1.9.5/linux
MIDAS_DIR     =  $(MIDASSYS)/$(LINUXARCH)

#-----------------------------------------
# this is for Linux

OSFLAGS  = -DOS_LINUX -Dextname 
CFLAGS   = -g -Wall -O2 -DDEBUG 
#LIBS     = -lm -lz -lutil -lnsl -pg -lpthread
LIBS     = -lm -lz -lutil -lnsl -lpthread


#-------------------------------------------------------------------
# this is for other definitions

UFEMODS = 
SRCS    = *.c
PROGS	= mzdump

####################################################################
# Lines below here should not be edited
####################################################################

# library info

MD5DIR = md5-lib
MD5LIB = $(MD5DIR)/libmd5.a
LIB = $(MIDAS_DIR)/lib/libmidas.a $(MD5LIB)

# compiler info 

CC = gcc 
MEVB = -g -I$(MIDAS_DIR)/../include
CFLAGS  += -g -I$(MIDAS_DIR)/../include -I$(STRUCK_DIR) -I$(STRUCK_DIR)/linux-gnu -Imd5-lib -pg
LDFLAGS += -static



# target rules

all: $(SYNCP) $(PROGS)

$(MD5LIB):
	make -C $(MD5DIR)

mzdump: mzdump.c $(MIDAS_DIR)/lib/ybos.o $(MD5LIB)
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ mzdump.c $(LIB) -lz $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	@ echo "Cleaning All Executables and Binaries ... "
	@ rm -f *.o *~ \#* $(PROGS) $(SYNCP) .depend
	make -C $(MD5DIR) clean

depend: Makefile $(SRCS)
	$(CC) $(CFLAGS) -M $(SRCS)  >./.depend	

###
-include .depend



