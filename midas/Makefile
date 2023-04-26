#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for MIDAS binaries and examples under unix
#
#  $Log: Makefile,v $
#  Revision 1.2  2005/06/20 23:43:31  mucap
#  (Fred)
#  Added logic to detect the x86-64 architecture and build in the linux64
#  subdirectory.
#
#  Revision 1.65  2004/10/06 19:00:41  olchansk
#  Fix "make install": delete old optional items (libmidas.so, hmana.o, rmana.o) if we do not install newer versions
#
#  Revision 1.64  2004/10/04 04:52:27  olchansk
#  fix missing dependance: $(PROGS): $(SHLIB)
#
#  Revision 1.63  2004/09/29 17:57:33  midas
#  Added large file (>2GB) support for linux
#
#  Revision 1.62  2004/09/24 23:19:59  midas
#  Use 'install', don't install rmana.o if no ROOT present
#
#  Revision 1.61  2004/09/23 20:23:25  midas
#  Use -DUSE_ROOT instead of -DHAVE_ROOT in mana.c
#
#  Revision 1.60  2004/09/21 21:50:12  midas
#  Added optional mySQL linking for mlogger
#
#  Revision 1.59  2004/09/18 04:33:26  olchansk
#  Add support for linking with the static libRoot.a: use "make NEED_LIBROOTA=1"
#  This is experimental and untested. Compilation and linking tested on RHL9 with ROOT v4.00.08.
#
#  Revision 1.58  2004/09/18 04:10:12  olchansk
#  Per discussion with Stefan, make the default to build and use static libmidas.a.
#  To build midas with the dynamic libmidas.so, use "make NEED_SHLIB=1"
#
#  Revision 1.57  2004/09/18 04:06:00  olchansk
#  Add -Wuninitialized to CFLAGS
#  Add removal of library files to "make clean"
#
#  Revision 1.56  2004/08/08 23:28:08  pierre
#  test commit
#
#  Revision 1.55  2004/06/28 15:44:25  olchansk
#  Do not try to specify "-rpath" on Darwin
#
#  Revision 1.54  2004/06/07 02:41:59  olchansk
#  set ROOT and libmidas.a RPATH on executables
#
#  Revision 1.53  2004/04/04 01:41:20  olchansk
#  dio does not need to link with midas libraries
#
#  Revision 1.52  2004/01/22 06:33:10  pierre
#  fix location of midas.so
#
#  Revision 1.51  2004/01/18 10:15:08  olchansk
#  add support for macosx aka darwin
#  make building shared library dependant on NEED_SHLIB, enabled by default
#
#  Revision 1.50  2004/01/16 05:31:02  pierre
#  mcleanup again
#
#  Revision 1.49  2004/01/16 00:27:42  pierre
#  change mcleanup path
#
#  Revision 1.48  2004/01/07 08:20:27  midas
#  Added -l90 to indent
#
#  Revision 1.47  2004/01/06 13:25:26  midas
#  Added 'indent' target
#
#  Revision 1.46  2003/12/12 13:14:43  midas
#  Fixed compiler warning via -Wno-unused-function
#
#  Revision 1.45  2003/06/25 18:22:53  pierre
#  Added pthread support for UNIX version - DBM
#
#  Revision 1.44  2003/05/13 11:08:28  midas
#  Build rmana.o only if ROOTSYS defined
#
#  Revision 1.43  2003/05/09 22:44:42  pierre
#  add ROOTSYS path to root-config
#
#  Revision 1.42  2003/04/28 11:12:55  midas
#  Adjusted ROOT flags
#
#  Revision 1.41  2003/04/20 02:59:13  olchansk
#  merge ROOT code into mana.c
#  remove MANA_LITE, replaced with HAVE_HBOOK, HAVE_ROOT
#  ROOT histogramming support almost complete,
#  ROOT TTree filling is missing
#  all ROOT code is untested, but compiles with RH-8.0, GCC-3.2, ROOT v3.05.03
#
#  Revision 1.40  2003/04/18 01:49:49  olchansk
#  Add "rmidas"
#
#  Revision 1.39  2003/04/16 23:28:56  olchansk
#  change cflags to -g -O2 -Wall -Wunitialized for maximum warnings (-Wuninitialized does not work with -O2)
#  if ROOTSYS is set, build mlogger with ROOT support
#  if ROOTSYS is set, build rmana.o with ROOT support
#
#  Revision 1.38  2003/04/08 00:05:16  olchansk
#  add rmana.o (the ROOT MIDAS analyzer)
#
#  Revision 1.37  2002/06/04 07:32:00  midas
#  Added 'melog'
#
#  Revision 1.36  2002/05/10 05:20:54  pierre
#  add MANA_LITE option on mana & fal
#
#  Revision 1.35  2002/01/24 22:26:48  pierre
#  rm elog, add cc for webpaw,odbhist
#
#  Revision 1.34  2002/01/23 11:27:32  midas
#  Removed elogd (now separate package)
#
#  Revision 1.33  2001/10/05 22:32:38  pierre
#  - added mvmestd in install include.
#  - change ybos.c to ybos.o in mdump build rule.
#
#  Revision 1.32  2001/08/07 11:04:03  midas
#  Added -lc flag for libmidas.so because of missing stat()
#
#  Revision 1.31  2001/08/07 10:40:46  midas
#  Added -w flag for HBOOK files to supress warnings cause by cfortran.h
#
#  Revision 1.30  2001/08/06 12:02:56  midas
#  Fixed typo
#
#  Revision 1.29  2001/05/23 13:17:19  midas
#  Added elogd
#
#  Revision 1.28  2001/02/19 12:42:25  midas
#  Fixed bug with missing "-Dextname" under Linux
#
#  Revision 1.27  2000/12/01 09:26:44  midas
#  Added fal.o (again?)
#
#  Revision 1.26  2000/11/20 13:42:39  midas
#  Install mcleanup
#
#  Revision 1.25  2000/11/20 13:13:06  midas
#  Fixed little bug
#
#  Revision 1.24  2000/11/20 11:40:36  midas
#  Added pmana.o for parallelized analyzer
#
#  Revision 1.23  2000/09/29 21:00:35  pierre
#  - Add SPECIFIC_OS_PRG in PROGS
#  - Define SPECIFIC_OS_PRG or each OS
#  - Add -lc in LIBS for OSF1
#  - Add $(LIBS) in SHLIB for OSF1
#
#  Revision 1.22  2000/07/21 18:30:11  pierre
#  - Added MIDAS_PREF_FLAGS for custom build
#
#  Revision 1.21  2000/05/11 14:21:34  midas
#  Added webpaw
#
#  Revision 1.20  2000/04/27 14:48:27  midas
#  Added mgd.c in mhttpd
#
#  Revision 1.19  2000/04/20 23:26:12  pierre
#  - Correct stripchart.tcl installtion
#
#  Revision 1.18  2000/04/18 20:35:05  pierre
#  - Added mchart and stripchart.tcl installation
#
#  Revision 1.17  2000/02/29 20:19:53  midas
#  Removed -lbsd for Linux
#
#  Revision 1.16  2000/02/24 19:39:22  midas
#  Add esone.h in installation
#
#  Revision 1.15  1999/12/20 14:24:17  midas
#  Adjusted for different driver direcotries
#
#  Revision 1.14  1999/09/17 11:55:08  midas
#  Remove OS_UNIX (now defined in midas.h)
#
#  Revision 1.13  1999/09/14 15:32:09  midas
#  Added elog
#
#  Revision 1.12  1999/08/24 13:44:25  midas
#  Added odbhist
#
#  Revision 1.11  1999/08/08 19:20:21  midas
#  Added chmod +s to mhttpd
#
#  Revision 1.10  1999/08/03 13:14:33  midas
#  Removed -DPIC, added dio to normal utils compilation
#
#  Revision 1.9  1999/07/22 19:42:58  pierre
#  - Added USERFLAGS to allow other options (-DUSERFLAGS=-static)
#
#  Revision 1.8  1999/06/23 10:01:20  midas
#  Additions (shared library etc.) added by Glenn
#
#  Revision 1.3  1999/01/19 09:11:24  midas
#  Added -DNO_PTY for Ultrix
#
#  Revision 1.2  1998/11/22 20:13:26  midas
#  Added fal.o to standard installation
#
#  Revision 1.1  1998/11/22 20:10:33  midas
#  Changed makefile to Makefile
#
#  Revision 1.4  1998/10/28 10:53:20  midas
#  Added -lutil for linux
#
#  Revision 1.3  1998/10/19 17:54:36  pierre
#  - Add lazylogger task
#
#  Revision 1.2  1998/10/12 12:18:55  midas
#  Added Log tag in header
#
#
#  Previous revision history
#  ------------------------------------------------------------------
#  date         by    modification
#  ---------    ---   -----------------------------------------------
#  11-NOV-96    SR    created
#  29-JAN-97    SR    added mtape utility
#  06-MAY-97    SR    included ybos in libmidas.a
#  05-SEP-97    SR    reorganized examples subtree
#  28-Oct-97   PA+SR  reorganization for multiple OS.
#  08-Jan-98    SR    Added creation of OS_DIR
#
#####################################################################
#
# Usage:
#        gmake             To compile the MIDAS library and binaries
#        gmake install     To install the library and binaries
#        gmake examples    To compile low-level examples (not necessary)
#        gmake static      To make static executables for debugging
#
#
# This makefile contains conditional code to determine the operating
# system it is running under by evaluating the OSTYPE environment
# variable. In case this does not work or if the GNU make is not
# available, the conditional code has to be evaluated manually.
# Remove all ifeq - endif blocks except the one belonging to the
# current operating system. From this block remove only the first
# and the last line (the one with the ifeq and endif statement).
#
# "gmake install" will copy MIDAS binaries, the midas library and
# the midas include files to specific directories for each version.
# You may change these directories to match your preferences.
#####################################################################

# get OS type from shell
OSTYPE = $(shell uname)

#
# System direcotries for installation, modify if needed
#

ifndef PREFIX
PREFIX     = /usr/local
endif

SYSBIN_DIR = $(PREFIX)/bin
SYSLIB_DIR = $(PREFIX)/lib
SYSINC_DIR = $(PREFIX)/include

#
#  Midas preference flags
#  -DYBOS_VERSION_3_3  for YBOS up to version 3.3 
MIDAS_PREF_FLAGS  =

#
# Option to build the midas shared library
#
# To link midas with the static libmidas.a, say "make ... NEED_SHLIB="
# To link midas with the shared libmidas.so, say "make ... NEED_SHLIB=1"
#
NEED_SHLIB=1

#
# Option to set the shared library path on MIDAS executables
#
NEED_RPATH=1

#
# Option to use the static ROOT library libRoot.a
#
# To link midas with the static ROOT library, say "make ... NEED_LIBROOTA=1"
#
NEED_LIBROOTA=

#
# Optional libmysqlclient library for mlogger
#
# To add mySQL support to the logger, say "make ... NEED_MYSQL=1"
NEED_MYSQL=1
#
# 
# Updated by VT for compatibility with 32-bit machines
#MYSQL_LIBS=/usr/lib64/mysql/libmysqlclient.so
ifeq ($(shell uname -m),x86_64)
    MYSQL_LIBS=/usr/lib64/mysql/libmysqlclient.so
else
    MYSQL_LIBS=/usr/lib/mysql/libmysqlclient.so
endif

# update for Ubuntu 14.04 64 bit
ifeq ($(shell lsb_release -si), Ubuntu)
ifeq ($(shell lsb_release -sr), 14.04)
ifeq ($(shell uname -m),x86_64)
	MYSQL_LIBS=/usr/lib/x86_64-linux-gnu/libmysqlclient.so
endif
endif
endif


#####################################################################
# Nothing needs to be modified after this line 
#####################################################################

#-----------------------
# Common flags
#
CFLAGS = -g -O0 -Wall -Wuninitialized -I$(INC_DIR) -I$(MXML_DIR) -I$(DRV_DIR)\
				 -L$(LIB_DIR) -DINCLUDE_FTPLIB $(MIDAS_PREF_FLAGS) $(USERFLAGS)

# >2GB file support
CFLAGS += -D_LARGEFILE64_SOURCE

#-----------------------
# OSF/1 (DEC UNIX)
#
ifeq ($(HOSTTYPE),alpha)
OSTYPE = osf1
endif

ifeq ($(OSTYPE),osf1)
OS_DIR = osf1
OSFLAGS = -DOS_OSF1
FFLAGS = -nofor_main -D 40000000 -T 20000000
LIBS = -lc -lbsd
SPECIFIC_OS_PRG = 
endif

#-----------------------
# Ultrix
#
ifeq ($(HOSTTYPE),mips)
OSTYPE = ultrix
endif

ifeq ($(OSTYPE),ultrix)
OS_DIR = ultrix
OSFLAGS = -DOS_ULTRIX -DNO_PTY
LIBS =
SPECIFIC_OS_PRG = 
endif

#-----------------------
# FreeBSD
#
ifeq ($(OSTYPE), FreeBSD)
OS_DIR = freeBSD
OSFLAGS = -DOS_FREEBSD
LIBS = -lbsd -lcompat
SPECIFIC_OS_PRG = 
endif

#-----------------------
# MacOSX/Darwin is just a funny Linux
#
ifeq ($(OSTYPE),Darwin)
OSTYPE = darwin
endif

ifeq ($(OSTYPE),darwin)
OS_DIR = darwin
OSFLAGS = -DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY -fPIC -Wno-unused-function
LIBS = -lpthread
SPECIFIC_OS_PRG = $(BIN_DIR)/mlxspeaker
NEED_RANLIB=1
NEED_SHLIB=
NEED_RPATH=
endif

#-----------------------
# This is for Linux
#
ifeq ($(OSTYPE),Linux)
OSTYPE = linux
ifeq ($(shell uname -m),x86_64)
  OSTYPE = linux64
endif
endif

ifeq ($(OSTYPE),linux)
OS_DIR = linux
OSFLAGS = -DOS_LINUX -fPIC -Wno-unused-function
LIBS = -lutil -lpthread
SPECIFIC_OS_PRG = $(BIN_DIR)/mlxspeaker $(BIN_DIR)/dio
endif

ifeq ($(OSTYPE),linux64)
OS_DIR = linux64
OSFLAGS = -DOS_LINUX -fPIC -Wno-unused-function
LIBS = -lutil -lpthread
SPECIFIC_OS_PRG = $(BIN_DIR)/mlxspeaker $(BIN_DIR)/dio
endif

#-----------------------
# This is for Solaris
#
ifeq ($(OSTYPE),solaris)
CC = gcc
OS_DIR = solaris
OSFLAGS = -DOS_SOLARIS
LIBS = -lsocket -lnsl
SPECIFIC_OS_PRG = 
endif

#####################################################################
# end of conditional code
#####################################################################

#
# Midas directories
#
INC_DIR  = include 
SRC_DIR  = src
UTL_DIR  = utils
DRV_DIR  = drivers
EXAM_DIR = examples
ZLIB_DIR = ./zlib-1.0.4
MXML_DIR = ./mxml

#
# Midas operating system dependent directories
#
LIB_DIR  = $(OS_DIR)/lib
BIN_DIR  = $(OS_DIR)/bin

#
# targets
#
EXAMPLES = $(BIN_DIR)/consume $(BIN_DIR)/produce \
	$(BIN_DIR)/rpc_test $(BIN_DIR)/msgdump $(BIN_DIR)/minife \
	$(BIN_DIR)/minirc $(BIN_DIR)/odb_test

PROGS = $(BIN_DIR)/mserver $(BIN_DIR)/mhttpd \
	$(BIN_DIR)/mlogger $(BIN_DIR)/odbedit \
	$(BIN_DIR)/mtape $(BIN_DIR)/mhist \
	$(BIN_DIR)/mstat $(BIN_DIR)/mcnaf \
	$(BIN_DIR)/mdump $(BIN_DIR)/lazylogger \
	$(BIN_DIR)/mchart $(BIN_DIR)/stripchart.tcl \
	$(BIN_DIR)/webpaw $(BIN_DIR)/odbhist \
	$(BIN_DIR)/melog \
	$(SPECIFIC_OS_PRG)

ANALYZER = $(LIB_DIR)/mana.o

ifdef CERNLIB
ANALYZER += $(LIB_DIR)/hmana.o
endif

ifdef ROOTSYS
PROGS += $(BIN_DIR)/rmidas
ANALYZER += $(LIB_DIR)/rmana.o
endif

OBJS =  $(LIB_DIR)/midas.o $(LIB_DIR)/system.o $(LIB_DIR)/mrpc.o \
	$(LIB_DIR)/odb.o $(LIB_DIR)/ybos.o $(LIB_DIR)/ftplib.o $(LIB_DIR)/strlcpy.o

LIBNAME=$(LIB_DIR)/libmidas.a
LIB    =$(LIBNAME)
ifdef NEED_SHLIB
SHLIB = $(LIB_DIR)/libmidas.so
LIB   = -lmidas -Wl,-rpath,$(SYSLIB_DIR)
endif
VPATH = $(LIB_DIR):$(INC_DIR)

all:    $(OS_DIR) $(LIB_DIR) $(BIN_DIR) \
	$(LIBNAME) $(SHLIB) \
	$(ANALYZER) \
	$(LIB_DIR)/mfe.o \
	$(LIB_DIR)/fal.o $(PROGS)

examples: $(EXAMPLES)

#####################################################################

#
# create library and binary directories
#

$(LIB_DIR)/strlcpy.o: $(MXML_DIR)/strlcpy.c
	$(CC) -c $(CFLAGS) $(OSFLAGS) -o $@ $<

$(OS_DIR):
	@if [ ! -d  $(OS_DIR) ] ; then \
           echo "Making directory $(OS_DIR)" ; \
           mkdir $(OS_DIR); \
        fi;

$(LIB_DIR):
	@if [ ! -d  $(LIB_DIR) ] ; then \
           echo "Making directory $(LIB_DIR)" ; \
           mkdir $(LIB_DIR); \
        fi;

$(BIN_DIR):
	@if [ ! -d  $(BIN_DIR) ] ; then \
           echo "Making directory $(BIN_DIR)" ; \
           mkdir $(BIN_DIR); \
        fi;

#
# main binaries
#

ifdef NEED_MYSQL
CFLAGS     += -DHAVE_MYSQL
LIBS       += $(MYSQL_LIBS) -lz
endif

ifdef ROOTSYS
ROOTLIBS    := $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS   := $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS  := $(shell $(ROOTSYS)/bin/root-config --cflags)
CFLAGS     += -DHAVE_ROOT $(ROOTCFLAGS)

ifdef NEED_RPATH
ROOTLIBS   += -Wl,-rpath,$(ROOTSYS)/lib
ROOTGLIBS  += -Wl,-rpath,$(ROOTSYS)/lib
endif

ifdef NEED_LIBROOTA
ROOTLIBS    := $(ROOTSYS)/lib/libRoot.a -lssl -ldl -lcrypt
ROOTGLIBS   := $(ROOTLIBS) -lfreetype
endif

$(BIN_DIR)/rmidas: $(BIN_DIR)/%: $(SRC_DIR)/%.c
	$(CXX) $(CFLAGS) $(OSFLAGS) $(ROOTCFLAGS) -o $@ $< $(LIB) $(ROOTGLIBS) $(LIBS)
endif # ROOTSYS

$(BIN_DIR)/mlogger: $(BIN_DIR)/%: $(SRC_DIR)/%.c
	$(CXX) $(CFLAGS) $(OSFLAGS) -o $@ $< $(LIB) $(ROOTLIBS) $(LIBS)

$(BIN_DIR)/%:$(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $< $(LIB) $(LIBS)

$(BIN_DIR)/odbedit: $(SRC_DIR)/odbedit.c $(SRC_DIR)/cmdedit.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(SRC_DIR)/odbedit.c $(SRC_DIR)/cmdedit.c $(LIB) $(LIBS)

$(BIN_DIR)/mhttpd: $(SRC_DIR)/mhttpd.c $(SRC_DIR)/mgd.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(SRC_DIR)/mhttpd.c $(SRC_DIR)/mgd.c $(LIB) $(LIBS) -lm

$(PROGS): $(LIBNAME) $(SHLIB) 

#
# examples
#

$(BIN_DIR)/%:$(EXAM_DIR)/lowlevel/%.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $< $(LIB) $(LIBS)

$(BIN_DIR)/%:$(EXAM_DIR)/basic/%.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $< $(LIB) $(LIBS)

$(EXAMPLES): $(LIBNAME)

#
# midas library
#

$(LIBNAME): $(OBJS)
	rm -f $@
	ar -crv $@ $^
ifdef NEED_RANLIB
	ranlib $@
endif

ifdef NEED_SHLIB
$(SHLIB): $(OBJS)
	rm -f $@
	ld -shared -o $@ $^ $(LIBS) -lc
endif

#
# frontend and backend framework
#

$(LIB_DIR)/mfe.o: msystem.h midas.h midasinc.h mrpc.h
$(LIB_DIR)/fal.o: $(SRC_DIR)/fal.c msystem.h midas.h midasinc.h mrpc.h

$(LIB_DIR)/fal.o: $(SRC_DIR)/fal.c msystem.h midas.h midasinc.h mrpc.h
	$(CC) -Dextname -c $(CFLAGS) $(OSFLAGS) -o $@ $<
$(LIB_DIR)/mana.o: $(SRC_DIR)/mana.cpp msystem.h midas.h midasinc.h mrpc.h
	$(CXX) -c $(CFLAGS) $(OSFLAGS) -o $@ $<
$(LIB_DIR)/hmana.o: $(SRC_DIR)/mana.cpp msystem.h midas.h midasinc.h mrpc.h
	$(CXX) -Dextname -DHAVE_HBOOK -c $(CFLAGS) $(OSFLAGS) -o $@ $<
ifdef ROOTSYS
$(LIB_DIR)/rmana.o: $(SRC_DIR)/mana.cpp msystem.h midas.h midasinc.h mrpc.h
	$(CXX) -DUSE_ROOT -c $(CFLAGS) $(OSFLAGS) $(ROOTCFLAGS) -o $@ $<
endif

#
# library objects
#

$(LIB_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $(OSFLAGS) -o $@ $<

$(LIB_DIR)/midas.o: msystem.h midas.h midasinc.h mrpc.h
$(LIB_DIR)/system.o: msystem.h midas.h midasinc.h mrpc.h
$(LIB_DIR)/mrpc.o: msystem.h midas.h mrpc.h
$(LIB_DIR)/odb.o: msystem.h midas.h midasinc.h mrpc.h
$(LIB_DIR)/ybos.o: msystem.h midas.h midasinc.h mrpc.h
$(LIB_DIR)/ftplib.o: msystem.h midas.h midasinc.h

#
# utilities
#
$(BIN_DIR)/%:$(UTL_DIR)/%.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $< $(LIB) $(LIBS)


$(BIN_DIR)/mcnaf: $(UTL_DIR)/mcnaf.c $(DRV_DIR)/bus/camacrpc.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(UTL_DIR)/mcnaf.c $(DRV_DIR)/bus/camacrpc.c $(LIB) $(LIBS)

$(BIN_DIR)/mdump: $(UTL_DIR)/mdump.c $(LIB_DIR)/ybos.o
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(UTL_DIR)/mdump.c $(LIB) -lz $(LIBS)

$(BIN_DIR)/lazylogger: $(SRC_DIR)/lazylogger.c $(LIB_DIR)/ybos.o
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(SRC_DIR)/lazylogger.c $(LIB) -lz $(LIBS)

$(BIN_DIR)/dio: $(UTL_DIR)/dio.c
	$(CC) $(CFLAGS) $(OSFLAGS) -o $@ $(UTL_DIR)/dio.c

$(BIN_DIR)/stripchart.tcl: $(UTL_DIR)/stripchart.tcl
	cp -f $(UTL_DIR)/stripchart.tcl $(BIN_DIR)/. 


#####################################################################

static:
	rm -f $(PROGS)
	make USERFLAGS=-static

#####################################################################

install:
# system programs
	@echo "... "
	@echo "... Installing programs to $(SYSBIN_DIR)"
	@echo "... "

	@if [ ! -d  $(SYSBIN_DIR) ] ; then \
          echo "Making directory $(SYSBIN_DIR)" ; \
          mkdir -p $(SYSBIN_DIR); \
        fi;

	@for i in mserver mhttpd odbedit mlogger dio ; \
	  do \
	  install -v -m 755 $(BIN_DIR)/$$i $(SYSBIN_DIR) ; \
	  done

	install -v -m 755 $(UTL_DIR)/mcleanup $(SYSBIN_DIR)
	chmod +s $(SYSBIN_DIR)/dio
	chmod +s $(SYSBIN_DIR)/mhttpd

# utilities
	@echo "... "
	@echo "... Installing utilities to $(SYSBIN_DIR)"
	@echo "... "
	@for i in mhist melog odbhist mtape mstat lazylogger mdump mcnaf mlxspeaker mchart stripchart.tcl webpaw; \
	  do \
	  install -v -m 755 $(BIN_DIR)/$$i $(SYSBIN_DIR) ; \
	  done
	ln -fs $(SYSBIN_DIR)/stripchart.tcl $(SYSBIN_DIR)/stripchart
	chmod +s $(SYSBIN_DIR)/webpaw

# include
	@echo "... "
	@echo "... Installing include files to $(SYSINC_DIR)"
	@echo "... "

	@if [ ! -d  $(SYSINC_DIR) ] ; then \
          echo "Making directory $(SYSINC_DIR)" ; \
          mkdir -p $(SYSINC_DIR); \
        fi;

	@for i in midas msystem midasinc mrpc ybos cfortran hbook hardware mcstd mvmestd esone mfbstd ; \
	  do \
	  install -v -m 644 $(INC_DIR)/$$i.h $(SYSINC_DIR) ; \
	  done

# library + objects
	@echo "... "
	@echo "... Installing library and objects to $(SYSLIB_DIR)"
	@echo "... "

	@if [ ! -d  $(SYSLIB_DIR) ] ; then \
          echo "Making directory $(SYSLIB_DIR)" ; \
          mkdir -p $(SYSLIB_DIR); \
        fi;

	@for i in libmidas.a mana.o mfe.o fal.o ; \
	  do \
	  install -v -m 644 $(LIB_DIR)/$$i $(SYSLIB_DIR) ; \
	  done

ifdef CERNLIB
	install -v -m 644 $(LIB_DIR)/hmana.o $(SYSLIB_DIR)/hmana.o
else
	rm -fv $(SYSLIB_DIR)/hmana.o
endif
ifdef ROOTSYS
	install -v -m 644 $(LIB_DIR)/rmana.o $(SYSLIB_DIR)/rmana.o
else
	rm -fv $(SYSLIB_DIR)/rmana.o
endif
ifdef NEED_SHLIB
	install -v -m 644 $(LIB_DIR)/libmidas.so $(SYSLIB_DIR)
else
	rm -fv $(SYSLIB_DIR)/libmidas.so
endif

	@if [ -d  $(ZLIB_DIR) ] ; then \
	  install -v -m 644 $(ZLIB_DIR)/zlib.h $(SYSINC_DIR) ;\
	  install -v -m 644 $(ZLIB_DIR)/zconf.h $(SYSINC_DIR) ;\
	  install -v -m 644 $(ZLIB_DIR)/libz.a $(SYSLIB_DIR) ;\
	fi;


indent:
	find . -name "*.[hc]" -exec indent -kr -nut -i3 -l90 {} \;

clean:
	rm -f $(LIB_DIR)/*.o $(LIB_DIR)/*.a $(LIB_DIR)/*.so *~ \#*

mrproper : clean
	rm -rf $(OS_DIR)
	rm -rf vxworks/68kobj vxworks/ppcobj


