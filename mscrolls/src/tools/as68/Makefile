###############################################################################
#
#	%W%
#	%G%
#
#	VAX Makefile
#
###############################################################################

CC = gcc
LEX = flex

FLAGS=	-O -DDEBUG -Wall -m32 #-arch i386
OPTIONS= -DEMU
CFLAGS=	$(FLAGS) $(OPTIONS)
LDFLAGS= 

OFILES=	main.o assemins.o symbols.o output.o errors.o \
	branches.o reloc.o assemble.o macros.o include.o \
	lexan.o parse.o getline.o version.o \
	vfiles.o os.o opcodes.o

HFILES=	alloc.h branches.h directives.h ea.h expr.h hunks.h opcodes.h \
		reloc.h sections.h symbols.h tokens.h macros.h

as68	:	$(OFILES) 
	$(CC) $(CFLAGS) $(OFILES) $(LDFLAGS) -o as68

install: as68
	cp $< ../bin

%.c : %.l
	$(LEX) -L -o $@ $<

# DO NOT DELETE THIS LINE

main.o: inttype.h alloc.h sections.h symbols.h
opcodes.o: inttype.h opcodes.h
assemins.o: inttype.h sections.h expr.h ea.h opcodes.h branches.h
symbols.o: inttype.h alloc.h symbols.h opcodes.h tokens.h directives.h
output.o: inttype.h sections.h reloc.h symbols.h hunks.h
errors.o: inttype.h getline.h
branches.o: inttype.h alloc.h sections.h expr.h branches.h
reloc.o: inttype.h alloc.h reloc.h sections.h symbols.h
assemble.o: inttype.h alloc.h sections.h symbols.h tokens.h expr.h ea.h
assemble.o: directives.h opcodes.h getline.h
macros.o: inttype.h alloc.h macros.h directives.h tokens.h symbols.h getline.h
include.o: inttype.h alloc.h
parse.o: inttype.h symbols.h expr.h ea.h tokens.h getline.h
getline.o: inttype.h symbols.h macros.h opcodes.h
branches.h:sections.h sections.h expr.h
	touch -f branches.h
ea.h:modes.h
	touch -f ea.h
expr.h:symbols.h
	touch -f expr.h
symbols.h:sections.h
	touch -f symbols.h
tokens.h:symbols.h expr.h
	touch -f tokens.h

.PRECIOUS: lexan.c

clean:
	rm -f *.o *~ *.obj *.pdb *ilk *.exe lexan.c
