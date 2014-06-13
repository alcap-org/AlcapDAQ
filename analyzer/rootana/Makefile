all:  init libAnalysis.so rootana libAnalysis.rootmap

.PHONY: clean print all

###################################################################################
# Process the files.make file which lists the directories that contain source code
###################################################################################
include src/files.make

# Root directory for unit test framework
TUT_ROOT = src/unit_test

# Directories that contain source code
EMPTY:=
SPACE:=$(EMPTY) $(EMPTY)
SOURCES:=$(foreach Dir,$(DIRS),$(wildcard $(Dir)/*.cpp))
SOURCE_DIRS:= $(subst $(SPACE),:,$(DIRS))
TUT_SOURCES = $(filter-out $(TUT_ROOT)/tut_main.cpp, $(wildcard $(TUT_ROOT)/*.cpp))

# Classes that need a root dictionary
ROOT_DICT_HEADERS:=$(ROOT_DICT_CLASSES:%=%.h)
ROOT_DICT_SOURCES:=$(ROOT_DICT_CLASSES:%=%.cpp)

#### End of files.make processing #################################################
# Where should we store all object and dependency files
OBJ_DIR = obj

# What should we call the root dictionary
DATA_LIBRARY_NAME:=RootanaDataClasses

# A list of directories with header files. Should be colon-separated. eg:
#   inc:tree_files/inc:extra_stuff/inc
INCLUDE_DIRS:= ../src/common:$(SOURCE_DIRS)
TUT_INCLUDE_DIRS = $(TUT_ROOT)


# converts the colon-separated list to a list with -I flags. eg:
#   -Iinc/ -Itree_files/inc/ -Iextra_stuff/inc/
INCLUDE_CFLAGS := $(patsubst %,-I%,$(subst :, ,$(INCLUDE_DIRS)))

# Tell GNU where to find header and source files.
vpath %.h : $(INCLUDE_DIRS)
vpath %.cpp : $(SOURCE_DIRS)

ROOT_CFLAGS:= $(shell $(ROOTSYS)/bin/root-config --cflags)

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

CXXFLAGS := $(OPTFLAGS) \
           $(INCLUDE_CFLAGS) $(ROOT_CFLAGS) -fPIC

ALCAPANA_DATACLASSES := $(DAQdir)/analyzer/work/$(USER)/libDataClasses.so
ALCAPANA_DATALINKDEF := $(DAQdir)/analyzer/src/common/DataLibraryLinkDef.h
ROOTANA_DATALINKDEF := src/LibraryLinkDef.h
ROOTANA_DATACLASSES :=lib$(DATA_LIBRARY_NAME).so 
DATA_LIBRARY_SOURCE := ${OBJ_DIR}/$(DATA_LIBRARY_NAME).cpp
DATA_LIBRARY_HEADER := ${OBJ_DIR}/$(DATA_LIBRARY_NAME).h

# Determine the names of the object files by pattern substitution
# from the source file names.
OBJECTS := $(notdir $(patsubst %.C,%.o,$(SOURCES)))
OBJECTS := $(patsubst %.cpp,%.o, $(OBJECTS))
OBJECTS := $(addprefix $(OBJ_DIR)/, $(OBJECTS))

TUT_USE_OBJECTS := $(filter-out obj/main.o, $(OBJECTS))
TUT_OBJECTS := $(notdir $(patsubst %.cpp,%.o, $(TUT_SOURCES)))
TUT_OBJECTS := $(addprefix $(TUT_ROOT)/, $(TUT_OBJECTS)) 

####################################################################
# Define the various make targets
####################################################################

${OBJECTS}: ${OBJ_DIR}/%.o: %.cpp
	@echo -e "Making object: '"$@"'"
	@#echo "   from '"$^"'"
	@$(CXX) -c -Wall $(CXXFLAGS) -o $@  $<
	@$(CXX) -MP -MM -MT $@ $(CXXFLAGS) $< > $(patsubst %.o,%.d,$@)

$(DATA_LIBRARY_SOURCE):
	@$(ROOTSYS)/bin/rootcint -f $@ -c $(UIUC_CFLAGS) -p $(INCLUDE_CFLAGS) ${ROOT_DICT_HEADERS} $(ROOTANA_DATALINKDEF)

$(ROOTANA_DATACLASSES):  $(DATA_LIBRARY_SOURCE) $(ROOT_DICT_SOURCES) 
	@g++ -shared -o$@ $(ROOT_LIBS) $(CXXFLAGS) -I$(ROOTSYS)/include $(INCLUDE_CFLAGS) $^

rootana: $(OBJECTS) $(ROOTANA_DATACLASSES)
	@echo -e "Building Rootana" 
	@$(CXX) $(ICCFLAGS) -Wl,--no-as-needed -o rootana $(OBJECTS) $(ROOT_LIBS) -L$(PWD) $(ALCAPANA_DATACLASSES) $(ROOTANA_DATACLASSES)

libAnalysis.so: $(OBJECTS) $(ROOTANA_DATACLASSES)
	@echo -e "Making shared library: '"$@"' from '"$^"'"
	@$(CXX) $(ICCLIBS) $(ROOT_LIBS) -shared -o libAnalysis.so \
		$(shell root-config --ldflags) -I$(ROOTSYS)/include $^ -lMinuit -lPhysics \
		$(ALCAPANA_DATACLASSES) $(PWD)/$(ROOTANA_DATACLASSES)

libAnalysis.rootmap: libAnalysis.so $(ALCAPANA_DATALINKDEF) $(ROOTANA_DATALINKDEF)
	@rlibmap -f -o libAnalysis.rootmap -l libAnalysis.so -d libCore \
		-c $(ALCAPANA_DATALINKDEF) $(ROOTANA_DATALINKDEF)

init: | make_obj_dir

make_obj_dir:
	@mkdir -p $(OBJ_DIR) 

tut: init libAnalysis.so libAnalysis.rootmap unit_test
	@echo -e "Running tests."
	@./unit_test

$(TUT_ROOT)/%.o: $(TUT_ROOT)/%.cpp
	@$(CXX) -c -Wall $(CXXFLAGS) -I$(TUT_INCLUDE_DIRS) -o $@ $^   

unit_test: $(TUT_USE_OBJECTS) $(ROOTANA_DATACLASSES) $(TUT_SOURCES) $(TUT_OBJECTS) $(TUT_ROOT)/tut_main.o
	@echo -e "Building unit_tests"
	@$(CXX) -c -Wall $(CXXFLAGS) -I$(TUT_INCLUDE_DIRS) -o $(TUT_ROOT)/tut_main.o $(TUT_ROOT)/tut_main.cpp
	@$(CXX) $(ICCFLAGS) -Wl,--no-as-needed -o unit_test $(TUT_USE_OBJECTS) $(ROOT_LIBS) -L$(PWD) $(ALCAPANA_DATACLASSES) $(ROOTANA_DATACLASSES) -I$(TUT_INCLUDE_DIRS) $(TUT_OBJECTS) $(TUT_ROOT)/tut_main.o

clean: clean_libraries clean_rootana clean_obj_dir clean_tut

clean_libraries:
	@echo Removing libraries
	rm -f libAnalysis.*
	rm -f $(ROOTANA_DATACLASSES)
	rm -f $(DATA_LIBRARY_SOURCE) $(DATA_LIBRARY_HEADER)

clean_rootana:
	@echo Removing rootana
	rm -f rootana

clean_obj_dir:
	@echo Removing objects
	rm -rf $(OBJ_DIR)

clean_tut:
	@echo Removing unit tests
	rm -f unit_test
	rm -f $(TUT_ROOT)/*.o 

print:
	@echo -e "ALL_LIBS = $(ALL_LIBS)             \n" 
	@echo -e "CXXFLAGS = $(CXXFLAGS)             \n" 
	@echo -e "ROOTANA_DATALINKDEF = $(ROOTANA_DATALINKDEF)       \n" 
	@echo -e "DIRS = $(DIRS)                     \n" 
	@echo -e "INCLUDE_CFLAGS = $(INCLUDE_CFLAGS) \n" 
	@echo -e "INCLUDE_DIRS = $(INCLUDE_DIRS)     \n" 
	@echo -e "LBITS = $(LBITS)                   \n" 
	@echo -e "ALCAPANA_DATACLASSES = $(ALCAPANA_DATACLASSES)   \n" 
	@echo -e "OBJECTS = $(OBJECTS)               \n" 
	@echo -e "OBJ_DIR = $(OBJ_DIR)               \n" 
	@echo -e "OPTFLAGS = $(OPTFLAGS)             \n" 
	@echo -e "ROOT_CFLAGS = $(ROOT_CFLAGS)       \n" 
	@echo -e "ROOT_LIBS = $(ROOT_LIBS)           \n" 
	@echo -e "ROOT_DICT_CLASSES = $(ROOT_DICT_CLASSES)           \n" 
	@echo -e "ROOT_DICT_HEADERS = $(ROOT_DICT_HEADERS)           \n" 
	@echo -e "SOURCES = $(SOURCES)               \n" 
	@echo -e "SOURCE_DIRS = $(SOURCE_DIRS)       \n" 
	@echo -e "TUT_ROOT = $(TUT_ROOT)       \n"
	@echo -e "TUT_SOURCES = $(TUT_SOURCES)       \n"
	@echo -e "TUT_OBJECTS = $(TUT_OBJECTS)       \n"
	@echo -e "TUT_USE_OBJECTS = $(TUT_USE_OBJECTS)       \n"
	@echo -e "TUT_INCLUDE_DIRS = $(TUT_INCLUDE_DIRS)  \n"
	@echo $(wildcard $(OBJ_DIR)/*.d)

include $(wildcard $(OBJ_DIR)/*.d)
