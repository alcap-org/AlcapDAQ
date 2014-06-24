CPPFLAGS += -I$(shell $(ROOTSYS)/bin/root-config --incdir) 
CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
LDLIBS += $(shell $(ROOTSYS)/bin/root-config --libs)
LDLIBS += -Wall,-rpath,$(shell $(ROOTSYS)/bin/root-config --libdir)

ROOTANALIBS = ../libAnalysis.so ../libRootanaDataClasses.so

CPPFLAGS += -g -O3
SRC = makeSiR2Hists.cpp
OBJ = $(patsubst %.cpp, %.o, $(SRC))
target = r

all: $(target)

$(target): $(OBJ) 
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(ROOTANALIBS)

clean:
	rm -f *.o $(target)

