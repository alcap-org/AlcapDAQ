CPPFLAGS += -I$(shell $(ROOTSYS)/bin/root-config --incdir) 
CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
LDLIBS += $(shell $(ROOTSYS)/bin/root-config --libs)
LDLIBS += -Wall,-rpath,$(shell $(ROOTSYS)/bin/root-config --libdir)

ROOTANALIBS = ../libAnalysis.so ../libRootanaDataClasses.so

CPPFLAGS += -g -O3
target = r

all: $(target)

$(target): makeSiR2Hists.o
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDLIBS) $(ROOTANALIBS)

clean:
	rm -f *.o $(target)

