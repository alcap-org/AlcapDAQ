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

KINOKO_DRV = ../bit3_mvme/kinoko_vmedrv 
CXXFLAGS += -I$(KINOKO_DRV) -I../bit3_mvme/

MIDAS_INCLUDE = -I $(MIDASSYS)/include
MIDAS_LIB = $(MIDASSYS)/$(OSTYPE)/lib/mfe.o -L $(MIDASSYS)/$(OSTYPE)/lib -lmidas
CXX_LIBS += $(MIDAS_LIB)


#CXXFLAGS += $(MIDAS_INCLUDE) -DPART_OF_MUCAP -DDO_IOPERM
CXXFLAGS += $(MIDAS_INCLUDE) -DDO_IOPERM
CFLAGS = $(CXXFLAGS)

#We need this to reboot the desktop digitizers
CXXFLAGS += -I $(CAEN_DIR)/include

MODULES = crate.o rpc_master.o rpc_slave.o diag.o odb_wrapper.o
#MODULES += rpv130_module.o
#VMEDRVS += ../bit3_mvme/rpv130.o ../bit3_mvme/bt617.o
DRVS += parport_module.o caen_desktop_reset.o 

# CAEN
CAEN_DIR = $(DAQdir)/CAEN
LDFLAGS += -L $(CAEN_DIR)/lib -lCAENDigitizer -lCAENComm -lCAENVME

all: crate

crate: $(MODULES) $(DRVS)
	$(CXX) -o $@ $(MODULES) $(CXX_LIBS) $(DRVS)  $(LDFLAGS)

.c.o: $(DRV)
	$(CXX) $(CXXFLAGS) -c $< 

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f crate *.o 

