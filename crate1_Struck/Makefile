# get OS type from shell
OSTYPE = $(shell uname)
ifeq ($(OSTYPE),Linux)
OSTYPE = linux
ifeq ($(shell uname -m),x86_64)
  OSTYPE = linux64
endif
endif

CC = gcc
CXX = g++
CXXFLAGS = -g -Wall -O3 -I/usr/include 
CXX_LIBS += -lutil -lpthread -lrt

KERNEL_VERSION = $(shell uname -r)
KERNEL_INCLUDE = -I /usr/src/kernels/$(KERNEL_VERSION)/include
CXXFLAGS += $(KERNEL_INCLUDE)

MIDAS_INCLUDE = -I $(MIDASSYS)/include
MIDAS_LIB = $(MIDASSYS)/$(OSTYPE)/lib/mfe.o -L $(MIDASSYS)/$(OSTYPE)/lib -lmidas
CXX_LIBS += $(MIDAS_LIB)

CXXFLAGS += $(MIDAS_INCLUDE) -DPART_OF_MUCAP -DDO_IOPERM
CFLAGS = $(CXXFLAGS)

# Struck 
STRUCK_DRIVER_PATH  := $(DAQdir)/Struck/sis1100-2.13-9

CXXFLAGS += -I$(STRUCK_DRIVER_PATH) -I$(STRUCK_DRIVER_PATH)/sis3100_calls 
CCFLAGS  += -I$(STRUCK_DRIVER_PATH) -I$(STRUCK_DRIVER_PATH)/sis3100_calls 
LDFLAGS  += -L$(STRUCK_DRIVER_PATH)/sis3100_calls -ldl -lstdc++  -lrt -l_sis3100 -Wl,"-rpath=$(STRUCK_DRIVER_PATH)/sis3100_calls"

# SIS3300 digitizer
CXXFLAGS += -I$(DAQdir)/Struck/sis330x

# SIS3350 digitizer
CXXFLAGS += -I$(DAQdir)/Struck/sis3350


MODULES = crate.o rpc_master.o rpc_slave.o diag.o odb_wrapper.o
MODULES += sis3300_module.o sis3100_module.o sis3350_module.o


all: crate

crate: $(MODULES)
	$(CXX) -o $@ $(MODULES) $(CXX_LIBS) $(LDFLAGS)

.c.o: $(DRV)
	$(CXX) $(CXXFLAGS) -c $< 

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f crate *.o 


sis3300_module.o:	sis3300_odb.cpp sis3300_tools.cpp

sis3350_module.o:	sis3350_odb.cpp sis3350_tools.cpp
