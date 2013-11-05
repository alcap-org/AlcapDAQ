OBJS=getHist.o mainWindow.o Dictionary.o TOnlineFrame.o
CXXFLAGS=`$(ROOTSYS)/bin/root-config --cflags`
LIBS= `$(ROOTSYS)/bin/root-config --glibs` -lThread 

all: online-display

Dictionary.cpp: 
	rootcint -f $@ -c getHist.h LinkDef.h

%.o: %.cpp
	$(CXX) -c -Wall $(CXXFLAGS) $<

online-display: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f online-display *.o Dictionary.h Dictionary.cpp

