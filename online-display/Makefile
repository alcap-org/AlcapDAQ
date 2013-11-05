OBJS=getHist.o mainWindow.o Dictionary.o
CXXFLAGS=`$(ROOTSYS)/bin/root-config --cflags`
LIBS= `$(ROOTSYS)/bin/root-config --glibs` -lThread

all: online-display

Dictionary.cpp:
		rootcint -f $@ -c getHist.h LinkDef.h

online-display: $(OBJS)
		g++ -o $@ $(OBJS) $(LIBS)

clean:
		rm online-display *.o Dictionary.h Dictionary.cpp
