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

MODULES = crate.o rpc_master.o rpc_slave.o diag.o odb_wrapper.o
MODULES += dt5720_module.o

# CAEN
CAEN_DIR = $(DAQdir)/CAEN
LDFLAGS += -L $(CAEN_DIR)/lib -lCAENDigitizer -lCAENComm -lCAENVME
# CAEN_DIGITIZER library
#CXXFLAGS += -I $(CAEN_DIR)/CAENDigitizer/CAENDigitizer_2.3.2/include
#LDFLAGS += -L $(CAEN_DIR)/CAENDigitizer/CAENDigitizer_2.3.2/lib/i686 -lCAENDigitizer
CXXFLAGS += -I $(CAEN_DIR)/CAENDigitizer/CAENDigitizer_2.6.5/include
# CAEN Comm library
#CXXFLAGS += -I $(CAEN_DIR)/CAENComm/include
CXXFLAGS += -I $(CAEN_DIR)/CAENComm-1.2/include
#LDFLAGS += -L $(CAEN_DIR)/CAENComm/lib -lCAENComm
# CAEN VME library
#CXXFLAGS += -I $(CAEN_DIR)/CAENVMELib/CAENVMELib-2.30.2/include
CXXFLAGS += -I $(CAEN_DIR)/CAENVMELib/CAENVMELib-2.41/include
#LDFLAGS += -L $(CAEN_DIR)/CAENVMELib/CAENVMELib-2.30.2/lib/X86 -lCAENVME




all: crate

crate: $(MODULES)
	$(CXX) -o $@ $(MODULES) $(CXX_LIBS) $(LDFLAGS)

.c.o: $(DRV)
	$(CXX) $(CXXFLAGS) -c $< 

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f crate *.o 

