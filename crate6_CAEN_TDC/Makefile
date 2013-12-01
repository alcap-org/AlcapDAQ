MODULES = crate.o caenV767.o sis3600.o dl40x.o lrs2249.o vmic_ttl.o rpc_master.o rpc_slave.o dummy_master.o camac_wrapper.o hyt1331.o vme_universe.o diag.o odb_wrapper.o s500.o

MIDAS_INCLUDE = -I $(MIDAS_HOME)/include
MIDAS_LIB = $(MIDAS_HOME)/linux/lib/mfe.o -L $(MIDAS_HOME)/linux/lib -lmidas -lutil -lpthread

UNIVERSE = ../universe
UNIVERSE_INCLUDE = -I $(UNIVERSE)

CXXFLAGS = -O3 $(UNIVERSE_INCLUDE) $(MIDAS_INCLUDE) -g -DPART_OF_MUCAP -DDO_IOPERM
CFLAGS = $(CXXFLAGS)

all: crate

crate: $(MODULES)
	$(CXX) -o $@ $(MODULES) $(MIDAS_LIB) 

clean:
	rm -f crate *.o hyt1331.c

hyt1331.c:
	cp $(MIDAS_HOME)/drivers/bus/hyt1331.c .
