###############################################################################
#
#	%W%
#	%G%
#
#	VAX Makefile
#
###############################################################################

CFLAGS	= -O -Wall #-m32 #-arch i386
LINK = link
CFLAGS_MSC	= -Zi
LDFLAGS_MSC = //DEBUG
LDFLAGS =

CPPFLAGS = $(CFLAGS)
CPPFLAGS_MSC = $(CFLAGS_MSC)
CC = gcc
CPP = g++

TARGET = lnk
OBJDIR = obj

CPPSOURCES =

CSOURCES = main.c linkit.c errors.c memory.c symbols.c output.c \
	   patch.c version.o

OBJS = $(patsubst %.cpp, $(OBJDIR)/%.obj, $(notdir $(CPPSOURCES))) \
       $(patsubst %.c, $(OBJDIR)/%.obj, $(notdir $(CSOURCES)))

OS =   $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(CPPSOURCES))) \
       $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(CSOURCES)))

all: $(TARGET)
exe: $(TARGET).exe

$(OBJDIR)/%.o: %.cpp 
	$(CPP) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.obj: %.cpp
	cl $(CPPFLAGS_MSC) -c -Fo$@ $<

$(OBJDIR)/%.obj: %.c
	cl $(CFLAGS_MSC) -c -Fo$@ $<


$(TARGET).exe: $(OBJS)
	$(LINK) //OUT:$@ $(LDFLAGS_MSC) $(OBJS)

$(TARGET): $(OS)
	$(CPP) $(CFLAGS) $(OS) $(LDFLAGS) -o $@

LFOFILES=	listform.o
MCOFILES=	mctoaout.o

lf	: $(LFOFILES)
	$(CC) $(CFLAGS) $(LFOFILES) $(LDFLAGS) -o lf

mcaout	: $(MCOFILES)
	$(CC) $(CFLAGS) $(MCOFILES) $(LDFLAGS) -o mcaout


install: lnk lf mcaout
	cp $^ ../bin

.PHONY: clean tags 

clean :
	rm -f $(OBJS) $(OS) *.exe *.ilk *.pdb *.o *~

tags:
	ctags -e *.c *.h

