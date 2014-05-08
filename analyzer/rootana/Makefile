all:  init libAnalysis.so rootana libAnalysis.rootmap

.PHONY: clean print all

# A list of directories with source files.
SOURCE_DIRS = src

#Where should we store all object and dependency files
OBJ_DIR = obj

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
ROOT_LIBS = $(shell $(ROOTSYS)/bin/root-config --libs) -lThread -lGui -lMinuit -lMinuit2
ALL_LIBS =  $(ROOT_LIBS)

OPTFLAGS = -g -O3 -Wno-write-strings

CXXFLAGS = $(OPTFLAGS) \
           $(INCLUDE_CFLAGS) $(ROOT_CFLAGS) -fPIC

include src/files.make

# Determine the names of the object files by pattern substitution
# from the source file names.
OBJECTS := $(notdir $(patsubst %.C,%.o,$(SOURCES)))
OBJECTS := $(patsubst %.cpp,%.o, $(OBJECTS))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS))

LIB_DATACLASS := $(DAQdir)/analyzer/work/$(USER)/libDataClasses.so
DATALINKDEF := $(DAQdir)/analyzer/src/common/DataLibraryLinkDef.h
${OBJECTS}: ${OBJ_DIR}/%.o: %.cpp
	@echo -e "Making object: '"$@"' from '"$^"'"
	@$(CXX) -c -Wall $(CXXFLAGS) -o $@  $<
	@$(CXX) -MM $(CXXFLAGS) $< > $(patsubst %.o,%.d,$@)

rootana: $(OBJECTS)
	@echo -e "Building Rootana" 
	@$(CXX) $(ICCFLAGS) -Wl,--no-as-needed -o rootana $(OBJECTS) $(ROOT_LIBS) $(LIB_DATACLASS)

libAnalysis.so: $(OBJECTS)
	@echo -e "Making shared library: '"$@"' from '"$^"'"
	@$(CXX) $(ICCLIBS) $(ROOT_LIBS) -shared -o libAnalysis.so \
		$(shell root-config --ldflags) -I$(ROOTSYS)/include $^ -lMinuit -lPhysics \
		$(LIB_DATACLASS)

libAnalysis.rootmap: libAnalysis.so $(DATALINKDEF)
	@rlibmap -f -o libAnalysis.rootmap -l libAnalysis.so -d libCore \
		-c $(DATALINKDEF)

init: | make_obj_dir

make_obj_dir:
	@mkdir -p $(OBJ_DIR) 

clean: clean_libraries clean_rootana clean_obj_dir

clean_libraries:
	@echo Removing libraries
	@rm -f libAnalysis.*

clean_rootana:
	@echo Removing rootana
	@rm -f rootana

clean_obj_dir:
	@echo Removing objects
	@rm -rf $(OBJ_DIR)

include $(wildcard *.d)
