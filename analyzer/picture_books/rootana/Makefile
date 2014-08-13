# Define some variables for ROOT
export ROOTCFLAGS   := $(shell root-config --cflags)
XROOTLIBS := $(shell root-config --libs)
export ROOTLIBS := $(subst -lMathCore,-lMathCore -lMathMore,$(XROOTLIBS))
export ROOTGLIBS    := $(shell root-config --glibs)
export ROOTINC=${ROOTSYS}/include

all:
	g++ ${ROOTLIBS} ${ROOTCFLAGS} -o picture_book.exe main.cpp CommandLine.cpp PictureBook.cpp

clean: 
	rm *.o *.exe
