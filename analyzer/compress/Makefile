# Uncomment next 2 lines to use Intel compiler
#CXX = icpc
#CC = icc

OBJS = caen_compress.o fadc_compress.o nfadc_compress.o n2fadc_ndet_compress.o n2fadc_tpc_compress.o io_buffer.o rle.o comp_compress.o huffman.o overall.o tdc400_compress.o hits_compress.o stck_compress.o

LIB = libmucap_compress.a

CXXFLAGS = -g  -O3 -I$(MIDAS_HOME)/include

#The following if statements set the lib path for 64-bit or 32-bit linux
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),32)
  LINUXVER=linux
else
  LINUXVER=linux64
endif

ALL = mucap_compress mucap_expand mucap_optimize $(LIB)
all: $(ALL)

clean:
	rm *.o $(ALL)

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar cr $(LIB) $(OBJS)

mucap_compress: mucap_compress.o $(LIB)
	$(CXX)  -o mucap_compress mucap_compress.o -L$(MIDAS_HOME)/$(LINUXVER)/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

mucap_expand: mucap_expand.o $(LIB)
	$(CXX)  -o mucap_expand mucap_expand.o -L$(MIDAS_HOME)/$(LINUXVER)/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

mucap_optimize: mucap_optimize.o $(LIB)
	$(CXX)  -o mucap_optimize mucap_optimize.o -L$(MIDAS_HOME)/$(LINUXVER)/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

midas_check:
	$(CXX)  -g -DOPENSSL_NO_KRB5 -I$(MIDAS_HOME)/include midas_check.cpp -o midas_check -L$(MIDAS_HOME)/$(LINUXVER)/lib -lmidas -lssl -lz -lpthread -lutil
