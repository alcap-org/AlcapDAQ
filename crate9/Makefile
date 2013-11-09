# get OS type from shell
OSTYPE = $(shell uname)
ifeq ($(OSTYPE),Linux)
OSTYPE = linux
ifeq ($(shell uname -p),x86_64)
  OSTYPE = linux64
endif
endif

#MODULES = crate.o new_fadc.o dummy_master.o diag.o odb_wrapper.o

MODULES = crate.o new_fadc.o rpc_master.o rpc_slave.o diag.o odb_wrapper.o

MIDAS = $(MIDASSYS)
MIDAS_INCLUDE = -I $(MIDAS)/include
#MIDAS_LIB = $(MIDAS)/linux/lib/mfe.o -L $(MIDAS)/linux/lib -lmidas -lutil -lpthread
MIDAS_LIB = $(MIDAS)/$(OSTYPE)/lib/mfe.o -L $(MIDAS)/$(OSTYPE)/lib -lmidas -lutil -lpthread

UNIVERSE = ../universe
UNIVERSE_INCLUDE = -I $(UNIVERSE)

CXXFLAGS = -O0 $(UNIVERSE_INCLUDE) $(MIDAS_INCLUDE) -g -DPART_OF_MUCAP -DDO_IOPERM
CFLAGS = $(CXXFLAGS)

all: crate

crate: $(MODULES)
	$(CXX) -o $@ $(MODULES) $(MIDAS_LIB) 

clean:
	rm -f crate *.o hyt1331.c

hyt1331.c:
	cp $(MIDAS_HOME)/drivers/bus/hyt1331.c .
