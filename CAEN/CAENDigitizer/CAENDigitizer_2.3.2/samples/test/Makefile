########################################################################
#
##              --- CAEN SpA - Computing Division ---
#
##   CAENDigitizer Software Project
#
##   Created  :  October    2009      (Rel. 1.0)
#
##   Auth: A. Lucchesi
#
#########################################################################
ARCH	=	`uname -m`

OUTDIR  =    	./bin/$(ARCH)/Release/
OUTNAME =    	ReadoutTest_Digitizer.bin
OUT     =    	$(OUTDIR)/$(OUTNAME)

CC	=	gcc

COPTS	=	-fPIC -DLINUX -O2

#FLAGS	=	-soname -s
#FLAGS	=       -Wall,-soname -s
#FLAGS	=	-Wall,-soname -nostartfiles -s
#FLAGS	=	-Wall,-soname

DEPLIBS	=	-lCAENDigitizer

LIBS	=	-L.. -L/home/daq/AlcapDAQ/CAEN/lib -lCAENComm

INCLUDEDIR =	-I./include -I/home/daq/AlcapDAQ/CAEN/CAENDigitizer/CAENDigitizer_2.3.2/include -I/home/daq/AlcapDAQ/CAEN/CAENComm/include  -I/home/daq/AlcapDAQ/CAEN/CAENVMELib/CAENVMELib-2.30.2/include

OBJS	=	src/ReadoutTest_Digitizer.o src/keyb.o 

INCLUDES =	./include/*

#########################################################################

all	:	$(OUT)

clean	:
		/bin/rm -f $(OBJS) $(OUT)

$(OUT)	:	$(OBJS)
		/bin/rm -f $(OUT)
		if [ ! -d $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
		$(CC) $(FLAGS) -o $(OUT) $(OBJS) $(LIBS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

