# to make 68k simulator


CFLAGS	= -O -Wall -Wno-unused-value -m32 #-arch i386
LINK = link
CFLAGS_MSC	= -Zi
LDFLAGS_MSC = //DEBUG
LDFLAGS =

CPPFLAGS = $(CFLAGS)
CPPFLAGS_MSC = $(CFLAGS_MSC)
CC = gcc
CPP = g++

TARGET = sim
OBJDIR = obj

CPPSOURCES =

CSOURCES = dis1.c dis2.c dis3.c read_sym.c kem.c get_ea.c do.c \
	aline.c display.c symbols.c

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

$(TARGET).exe: dir $(OBJS)
	$(LINK) //OUT:$@ $(LDFLAGS_MSC) $(OBJS)

$(TARGET): dir $(OS)
	$(CPP) $(CFLAGS) $(OS) $(LDFLAGS) -o $@

install: $(TARGET)
	cp $^ ../bin

dir:
	mkdir -p $(OBJDIR)

.PHONY: clean tags 

clean :
	rm -f $(OBJS) $(OS) *.exe *.ilk *.pdb *~

tags:
	ctags -e *.c *.h

kem.o : kem.h dis.h
do.o : kem.h dis.h
get_ea.o : kem.h dis.h
display.o : kem.h dis.h
dis1.o : kem.h dis.h sym_form.h hunks.h
dis2.o : dis.h aline.h kem.h
dis3.o : dis.h kem.h
read_sym.o : dis.h sym_form.h hunks.h
aline.o : dis.h aline.h



