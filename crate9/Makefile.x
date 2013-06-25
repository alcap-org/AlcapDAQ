MODULES = crate.o new_fadc.o rpc_master.o rpc_slave.o diag.o odb_wrapper.o

MIDAS = $(MIDAS_ROOT)
MIDAS_INCLUDE = -I $(MIDAS)/include
MIDAS_LIB = $(MIDAS)/linux64/lib/mfe.o -L $(MIDAS)/linux64/lib -lmidas -lutil -lpthread

UNIVERSE = ../universe
UNIVERSE_INCLUDE = -I $(UNIVERSE)

CXXFLAGS = -O3 $(UNIVERSE_INCLUDE) $(MIDAS_INCLUDE) -g -DPART_OF_MUCAP -DDO_IOPERM
CFLAGS = $(CXXFLAGS)

all: crateX

crateX: $(MODULES)
	$(CXX) -o $@ $(MODULES) $(MIDAS_LIB) 

clean:
	rm -f crate *.o hyt1331.c

hyt1331.c:
	cp $(MIDAS_HOME)/drivers/bus/hyt1331.c .
