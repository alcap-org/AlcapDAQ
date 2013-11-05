OBJS=getHist.o mainWindow.o Dictionary.o
CXXFLAGS=`root-config --cflags`
LIBS=`root-config --glibs` -lThread

all: online-display

Dictionary.cpp: 
	rootcint -f $@ -c getHist.h LinkDef.h

online-display: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

clean:
	rm *.o online-display Dictionary.cpp
