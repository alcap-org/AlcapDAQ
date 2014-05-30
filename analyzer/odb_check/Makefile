NAME = odb_check
INCDIR = include
SRCDIR = src
OBJDIR = $(SRCDIR)/obj

CC = g++
CCFLAGS += -g -Wall
CCFLAGS += -Iinclude
CCFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags)
LDLIBS += $(shell $(ROOTSYS)/bin/root-config --libs)

DEP = DataDir WireMap PulseEstimate ODBCheck
SRC = $(patsubst %,%.cc,$(DEP))
INC = $(SRC:.cc=.hh)
OBJ = $(SRC:.cc=.o)
SRCLOC = $(patsubst %,$(SRCDIR)/%,$(SRC))
INCLOC = $(patsubst %,$(INCDIR)/%,$(INC))
OBJLOC = $(patsubst %,$(OBJDIR)/%,$(OBJ))


all: $(OBJLOC) $(NAME)
	$(CC) $(CCFLAGS) $(OBJLOC) $(NAME).o $(LDLIBS) -o $(NAME)

$(NAME): $(SRCDIR)/$(NAME).cc $(INCLOC)
	$(CC) -c $(CCFLAGS) $< -o $@.o

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CCFLAGS) $< -o $@

clean:
	-rm -f $(OBJDIR)/*.o
	-rm -f $(NAME)
