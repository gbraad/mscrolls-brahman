CC	= 	gcc
OPT	= 	-O -Wall -Wno-parentheses -m32  #-arch i386

CFLAGS	=	$(OPT)
LDFLAGS	=	
HDRS	=	defs.h data.h vfiles.h
OBJ	=	fred23jr.o utils.o errors.o change.o in_word.o othertypes.o vfiles.o os.o

fred23jr :	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o fred23jr

$(OBJ) :	$(HDRS)

fred23jr.c :	sections.str itemtypes.str itemprops.str \
		npcprops.str roomprops.str

DEFOUT = itemtypes.h itemprops.h covertypes.h textures.h npcprops.h \
	 roomprops.h sections.h

defs.h : $(DEFOUT)
	touch defs.h

makedefs :	makedefs.o
	$(CC) $(CFLAGS) makedefs.o -o makedefs

covertypes.h covertypes.str : covertypes makedefs
	./makedefs covertypes
itemtypes.h itemtypes.str : itemtypes  makedefs
	./makedefs itemtypes
roomprops.h roomprops.str : roomprops  makedefs
	./makedefs roomprops
itemprops.h itemprops.str : itemprops  makedefs
	./makedefs itemprops
npcprops.h npcprops.str : npcprops  makedefs
	./makedefs npcprops
sections.h sections.str : sections  makedefs
	./makedefs sections
textures.h textures.str: textures  makedefs
	./makedefs -p=TX textures

install: fred23jr
	cp $< ../bin

clean:
	rm -f *.o *.obj *.exe *~ makedefs fred23jr *.str *.ilk *.pdb $(DEFOUT)

