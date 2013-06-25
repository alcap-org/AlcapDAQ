# Makefile for vmedrv libraries
# Created by Enomoto Sanshiro on 8 December 1999.  
# Last updated by Enomoto Sanshiro on 20 December 1999.

KINOKO_DRV = ./kinoko_vmedrv
CC = gcc
CFLAGS = -g -Wall -I/usr/include -I$(KINOKO_DRV)
CXX = g++
CXXFLAGS = -g -Wall -I/usr/include -I$(KINOKO_DRV)

MIDAS_INC = $(MIDASSYS)/include
CFLAGS += -I$(MIDAS_INC)
CXXFLAGS += -I$(MIDAS_INC)

ROOTLIBDIR := $(shell $(ROOTSYS)/bin/root-config --libdir)
ROOTLIBS   := -L$(ROOTLIBDIR) $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lXMLIO -lThread
ROOTGLIBS  := -L$(ROOTLIBDIR) $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lXMLIO -lThread
ROOTCFLAGS := $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS += -DHAVE_ROOT $(ROOTCFLAGS)

#TARGETS = r
DRV = bt617.o rpv130.o

all: $(DRV)

.c.o:
	$(CXX) $(CXXFLAGS) -c $< 

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f *.o
	rm -f $(TARGETS)

