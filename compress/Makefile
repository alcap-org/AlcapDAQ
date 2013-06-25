# Uncomment next 2 lines to use Intel compiler
#CXX = icpc
#CC = icc

OBJS = caen_compress.o fadc_compress.o n2fadc_ndet_compress.o n2fadc_tpc_compress.o io_buffer.o rle.o comp_compress.o huffman.o overall.o tdc400_compress.o hits_compress.o stck_compress.o

LIB = libmucap_compress.a

CXXFLAGS = -g  -O3 -I$(MIDASSYS)/include

ALL = mucap_compress mucap_expand mucap_optimize
all: $(ALL)

clean:
	rm *.o $(ALL)

$(LIB): $(OBJS)
	rm -f $(LIB)
	ar cr $(LIB) $(OBJS)
	$(CXX) -shared -o libmucap_compress.so $(OBJS)

mucap_compress: mucap_compress.o $(LIB)
	$(CXX)  -o mucap_compress mucap_compress.o -L$(MIDASSYS)/linux/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

mucap_expand: mucap_expand.o $(LIB)
	$(CXX)  -o mucap_expand mucap_expand.o -L$(MIDASSYS)/linux/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

mucap_optimize: mucap_optimize.o $(LIB)
	$(CXX)  -o mucap_optimize mucap_optimize.o -L$(MIDASSYS)/linux/lib -lmidas -L. -lmucap_compress -lz -lpthread -lutil

midas_check:
	$(CXX)  -g -DOPENSSL_NO_KRB5 -I$(MIDASSYS)/include midas_check.cpp -o midas_check -L$(MIDASSYS)/linux64/lib -lmidas -lssl -lz -lpthread -lutil
