all: alcapana libDataClasses.so libDataClasses.rootmap

.PHONY: clean print all

# A list of directories with source files.
SOURCE_DIRS = src

# A list of directories with header files. Should be colon-separated. eg:
#   inc:tree_files/inc:extra_stuff/inc
INCLUDE_DIRS = ../common

# MuCap compress library
COMPRESS = ../../compress

# Path to md5-lib
MD5 = ../../md5-lib

# converts the colon-separated list to a list with -I flags. eg:
#   -Iinc/ -Itree_files/inc/ -Iextra_stuff/inc/
INCLUDE_CFLAGS = $(patsubst %,-I%,$(subst :, ,$(INCLUDE_DIRS)))

ROOT_CFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)

# ROOT defines strlcpy(). This prevents a redefinition.
ROOT_CFLAGS += -DHAS_STRLCPY

MIDAS_CFLAGS = -I$(MIDASSYS)/include -L../md5-lib/ -lmd5 -I./

#The following if statements set the lib path for 64-bit or 32-bit linux
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),32)
  LINUXARCH=linux
else
  LINUXARCH=linux64
endif

# Generate the library flags (-L and -l) for the gcc argument
MIDAS_LIBS = $(MIDASSYS)/$(LINUXARCH)/lib/rmana.o \
             -L $(MIDASSYS)/$(LINUXARCH)/lib \
             -lmidas -lutil
ROOT_LIBS = $(shell $(ROOTSYS)/bin/root-config --libs) -lThread -lGui -lMinuit
UNCOMPRESS_LIBS = -L$(COMPRESS) -L$(MD5) -lmd5
ALL_LIBS =  $(MIDAS_LIBS) $(ROOT_LIBS) $(UNCOMPRESS_LIBS)

OPTFLAGS = -g -O3

CXXFLAGS = $(OPTFLAGS) \
           $(INCLUDE_CFLAGS) $(MIDAS_CFLAGS) $(ROOT_CFLAGS) \
           -I$(COMPRESS) -DHAVE_ROOT -fPIC


# Makefile.MODULES is generated from the MODULE file. It defines
# the variables MODULE_OBJS, which are 
# lists of the MIDAS analyzer modules to be compiled and run.
Makefile.MODULES: MODULES
	../../generate-Makefile.MODULES

# Defines MODULE_OBJS = ...
include Makefile.MODULES

# Tell GNU where to find header and source files.
vpath %.h : $(INCLUDE_DIRS)
vpath %.cpp : $(SOURCE_DIRS)


# These classes are made into libDataClasses.so, a library that contains
# all the stand-alone data classes needed for the analysis.
# This includes things like TElectron, but not the TElectronAnalysis module.
DATA_LIBRARY_CLASSES = $(shell cat ../../src/common/DataLibraryClasses.list)
DATA_LIBRARY_SRCS = $(DATA_LIBRARY_CLASSES:%=%.cpp)
DATA_LIBRARY_HEADERS = $(DATA_LIBRARY_CLASSES:%=%.h)
DATA_LIBRARY_OBJS = $(DATA_LIBRARY_CLASSES:%=%.o)

# These classes are used in the alcapana binary, but do not require
# dictionary generation. For instance, module objects.
ANA_CLASSES = $(shell cat ../../src/common/AnaClasses.list)
ANA_SRCS = $(ANA_CLASSES:%=%.cpp)
ANA_HEADERS = $(ANA_CLASSES:%=%.h)
ANA_OBJS = $(ANA_CLASSES:%=%.o)

base_objs = analyzer.o

objs = $(base_objs) $(MODULE_OBJS)

analyzer.o: MODULES.h
MODULES.h: MODULES
	../../generate-MODULES.h ./ 
#$(INCLUDE_DIRS) 

M4FLAGS = -I. 

%.mql: %.MQL
	m4 $(M4FLAGS) $< > $@

%.cpp: %.mql
	../../mql/mqlcompile $< $@

%.o: %.cpp
	$(CXX) -c -Wall $(CXXFLAGS) $<
	$(CXX) -MM $(CXXFLAGS) $< > $(notdir $(patsubst %.cpp,%.d,$<))

alcapana: $(objs) $(ANA_OBJS) libDataClasses.so
	$(CXX) $(ICCFLAGS) -o $@ $^ $(ALL_LIBS) -lmucap_compress -lm -lz  -Wl,-rpath,$(shell pwd)

DataLibraryDict.cpp: $(DATA_LIBRARY_HEADERS) ../../src/common/DataLibraryLinkDef.h
	$(ROOTSYS)/bin/rootcint -f $@ -c $(UIUC_CFLAGS) -p $^

libDataClasses.so: $(DATA_LIBRARY_OBJS) DataLibraryDict.o
	$(CXX) $(ICCLIBS) $(ROOT_LIBS) -shared -o libDataClasses.so $(shell root-config --ldflags) -I$(ROOTSYS)/include $^ -lMinuit -lPhysics

libDataClasses.rootmap: libDataClasses.so ../../src/common/DataLibraryLinkDef.h
	rlibmap -f -o libDataClasses.rootmap -l libDataClasses.so -d libCore -c ../../src/common/DataLibraryLinkDef.h

#$(INCLUDE_DIRS)/DataLibraryLinkDef.h


decompressor.o: decompressor.c
	$(CC) -c $(MIDAS_CFLAGS) -I/usr/include/sasl  -UUSE_ROOT  $<

clean: 
	rm -f *.d *.o *Dict.cpp *Dict.h blank MODULES.h Makefile.MODULES \
	alcapana libDataClasses.so libDataClasses.rootmap

blank:
	touch blank

print: 
	@echo "DATA_CLASSES = $(DATA_CLASSES)"; echo
	@echo "DATA_CLASSES = $(DATA_CLASSES)"; echo
	@echo "DATA_LIBRARY_HEADERS = $(DATA_LIBRARY_HEADERS)"; echo
	@echo "DATA_LIBRARY_OBJS = $(DATA_LIBRARY_OBJS)"; echo
	@echo "DATA_LIBRARY_SRCS = $(DATA_LIBRARY_SRCS)"; echo
	@echo "base_objs = $(base_objs)"; echo
	@echo "MODULE_OBJS = $(MODULE_OBJS)"; echo

include blank $(wildcard *.d)
