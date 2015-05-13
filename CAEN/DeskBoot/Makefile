########################################################################
#                                                                      
#              --- CAEN SpA - Computing Division ---                   
#                                                                      
#   DeskBoot Software Project                                        
#                                                                      
#   Created  :  June 2013      (Rel. 1.0)                                             
#                                                                                                                       
#                                                                      
########################################################################

EXE	=	DeskBoot

CC	=	gcc

COPTS	=	-fPIC -DLINUX -Wall 

FLAGS	=	-s

DEPLIBS	=       -lc -lm

LIBS	=	

INCLUDEDIR =	-I.

OBJS	=	DeskBoot.o

INCLUDES =	./

#########################################################################

all	:	$(EXE)

clean	:
		/bin/rm -f $(OBJS) $(EXE)

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXE)
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

