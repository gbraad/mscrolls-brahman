#############################################################################
#
#	%W%
#	%G%
#
#	Makefile for EMU
#	This version uses a back end written in C, so should
#	be portable across machines, if not operating systems.
#
#############################################################################

CC	=	gcc

OPT	=	-O2 -DNDEBUG
#CFLAGS	=	-g -O -DBSD42 -DC -finline-functions -DSTANDALONE -DGDBREMOTE
CFLAGS	=	-g -O2 -Wall -DC
YFLAGS	=	-d
# front end object files
OfFILES	=	main.o load.o charinout.o \
		dis1.o dis2.o dis3.o display.o get_text.o symbols.o \
		errors.o printnum.o dump.o breakpoints.o \
		commands.o load_bits.o registers.o cmd.o lexan.o \
		date.o version.o
# back end object files
ObFILES	=	emulate.o decode.o decodetable.o ea.o aline.o utils.o
OFILES	=	$(OfFILES) $(ObFILES)
CFILES	=	main.c load.c charinout.c \
		dis1.c dis2.c dis3.c display.c get_text.c symbols.c \
		errors.c printnum.c dump.c breakpoints.c \
		commands.c line.c printnum.c search.c load_bits.c registers.c \
		emulate.c decode.c decodetable.c ea.c
HFILES	=	aline.h bits.h commands.h dis.h ea.h emu.h errors.h \
		hunks.h symbols.h

OGDBREMOTE =	gdbremote.o unixio.o unixback.o load.o load_bits.o \
		decode.o decodetable.o ea.o \
		utils.o registers.o trap1.o

OSTANDALONE =	decode.o decodetable.o ea.o aline.o utils.o \
		registers.o charinout.o \
		standalone.o load.o load_bits.o vmtext.o decom.o

.l.c	:
	lex $*.l
	mv lex.yy.c $@

.y.c	:
	yacc -d $*.y
	mv y.tab.c $@

.PRECIOUS: lexan.c


emu	:	$(OFILES)
	$(CC) $(CFLAGS) $(OFILES) -o emu

gdbremote :	$(OGDBREMOTE)
	$(CC) $(CFLAGS) $(OGDBREMOTE) -o gdbremote

standalone :	$(OSTANDALONE)
	$(CC) $(CFLAGS) $(OSTANDALONE) -o standalone

install	:	emu
	cp emu ../bin

clean:
	rm -f *.o *~ *.exe


depend:
	grep '^#include' ${CFILES} | grep -v '<' | \
	sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    -e 's/\.c/.o/' >xtmp
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
			else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep <xtmp
	rm -f xtmp
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile OMakefile
	ed - Makefile < eddep
	rm eddep makedep

# DO NOT DELETE THIS LINE

load.o: emu.h symbols.h hunks.h
main.o: emu.h symbols.h errors.h
search.o: emu.h
dis1.o: emu.h dis.h symbols.h bits.h disopts.h
dis2.o: dis.h aline.h emu.h bits.h
dis3.o: dis.h emu.h bits.h
display.o: dis.h emu.h symbols.h disopts.h
symbols.o: symbols.h hunks.h errors.h
dump.o: emu.h
breakpoints.o: emu.h symbols.h
commands.o: commands.h cmd.c
search.o: emu.h
