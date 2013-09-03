all: rootana

.PHONY: clean print all

# A list of directories with source files.
SOURCE_DIRS = src

# A list of directories with header files. Should be colon-separated. eg:
#   inc:tree_files/inc:extra_stuff/inc
INCLUDE_DIRS = ../src/common:src

# converts the colon-separated list to a list with -I flags. eg:
#   -Iinc/ -Itree_files/inc/ -Iextra_stuff/inc/
INCLUDE_CFLAGS = $(patsubst %,-I%,$(subst :, ,$(INCLUDE_DIRS)))

# Tell GNU where to find header and source files.
vpath %.h : $(INCLUDE_DIRS)
vpath %.cpp : $(SOURCE_DIRS)

ROOT_CFLAGS = $(shell $(ROOTSYS)/bin/root-config --cflags)

# ROOT defines strlcpy(). This prevents a redefinition.
ROOT_CFLAGS += -DHAS_STRLCPY

#The following if statements set the lib path for 64-bit or 32-bit linux
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),32)
  LINUXARCH=linux
else
  LINUXARCH=linux64
endif

# Generate the library flags (-L and -l) for the gcc argument
ROOT_LIBS = $(shell $(ROOTSYS)/bin/root-config --libs) -lThread -lGui -lMinuit
ALL_LIBS =  $(ROOT_LIBS)

OPTFLAGS = -g -O3

CXXFLAGS = $(OPTFLAGS) \
           $(INCLUDE_CFLAGS) $(ROOT_CFLAGS) -fPIC

include src/files.make

# Determine the names of the object files by pattern substitution
# from the source file names.
OBJECTS := $(notdir $(patsubst %.C,%.o,$(SOURCES)))
OBJECTS := $(notdir $(patsubst %.cpp,%.o,$(OBJECTS)))

%.o: %.cpp
	$(CXX) -c -Wall $(CXXFLAGS) $<
	$(CXX) -MM $(CXXFLAGS) $< > $(notdir $(patsubst %.cpp,%.d,$<))

rootana: $(OBJECTS)
	$(CXX) $(ICCFLAGS) $(ROOT_LIBS) -o rootana $(OBJECTS) ../work/user/libDataClasses.so

clean: 
	rm -f *.d *.o *Dict.cpp *Dict.h blank rootana

blank:
	touch blank

include blank $(wildcard *.d)
