******************************************************
*
*	@(#)equates.head	1.5
*	8/25/87
*
******************************************************


*--------------------------------
*
* SRC.ADV.EQUATES
*
*--------------------------------


*******************************
*
*
* THIS WAS IN THE ASM FILE ON THE APPLE...
*
*

TRUE     EQU     1
FALSE    EQU     0

ABSOLUTELY_THE_LAST_LINK EQU FALSE      ;+/- 1 (or 2)
MAGIC_NORM     EQU   $4FE0C2B5

MAGIC_FRIG1    EQU   $4FE0C2B5
MAGIC_FRIG2    EQU   $4FE0C2B6
MAGIC_FRIG3    EQU   $4FE0C2B5
MAGIC_FRIG4    EQU   04
MAGIC_FRIG5    EQU   05
MAGIC_FRIG6    EQU   06
MAGIC_FRIG7    EQU   07
MAGIC_FRIG8    EQU   08
MAGIC_FRIG9    EQU   09


QL128   	SET      FALSE
ST520   	SET      FALSE 
AMIGA512	SET      FALSE
AMIGA256	SET      FALSE           ;hohohoho
C64     	SET      FALSE 
C128    	SET      FALSE
APPLE2  	SET      FALSE
BBC_SWR 	SET      FALSE           ;SWR=sideways ram
CPC6128 	SET      FALSE       
MAC512  	SET      FALSE
IBM	  	SET      FALSE           ;Hmmmmmmmmmmmmm
VAX		SET	 FALSE
ATARI   	SET      FALSE
PCW8256		SET	 FALSE
SPECTRUM	SET	 FALSE
GENERIC		SET	 FALSE
IIGS		SET	 FALSE
* gamedef.i - included by equates.asm

THE_PAWN	EQU	FALSE
VAX		SET	TRUE
PROD		EQU	TRUE
PROD_TEST	EQU	FALSE
YCHEAT		EQU	TRUE	
YCHEATMORE	EQU	FALSE
TimeCode	EQU	FALSE
NAMEDPICS       EQU     TRUE

MAXWGHT		EQU	$40		;max weight u can carry

TITLEPIC        SET     FALSE
Remastered      EQU     TRUE


******************************************************
*
*	%W%
*	%G%
*
******************************************************


set1	SET	(QL128!ST520!AMIGA512!AMIGA256!C64!C128)
set2	SET	(BBC_SWR!CPC6128!APPLE2!MAC512!IBM!VAX!ATARI)

	IFEQ	(set1!set2)

	 FAIL	'No machines defined!!!!'

	ENDC


*

  
      IFNE     BBC_SWR!C64!C128!APPLE2!VAX!ATARI
 
MachineDefined	EQU	TRUE
BookProtection EQU   FALSE 
SpoolLen       EQU    FALSE         ;no printer buffer
RECLIM         EQU    6             ;max relphrase depth
WIDTH.DEF      EQU    39            ;chars per line (as used in left just)
BUFFLEN        EQU    $80           ;size of input buffer
FunctionKeys   EQU    FALSE         ;no function keys
M68000         EQU    FALSE
DosPresent     EQU    TRUE
Graphics       EQU    TRUE
RestartRAM     EQU    FALSE
DummyText      EQU    FALSE         ;!!!! atomic bomb !! drugs !!!!
ExternText     EQU    TRUE          ; Text is external to this one!
MemoryScarce   EQU    FALSE         ;Last resort, lose stuff like COLOR=COLOUR
TimeInfo       EQU    FALSE         ;so as we can see how long various stuff is
ESC_edit       EQU    FALSE         ;re-edit last line with ESC
Wimp           EQU    FALSE
Mono_Possible  EQU    FALSE         ;Can we possibly run this in Monochrome?
FastBits       EQU    TRUE          ; Yes, 'cos we emulate search, etc.
SpeechAvail    EQU    FALSE          ;THE most useless feature of all time...
INOUTLINK	EQU	FALSE

      ENDC

***************************************************************
*
*	%W%
*	%G%
*
****************************************************************

	IFND	MachineDefined

	FAIL	'No definitions made for this machine!!!!'

	ENDC

*--------------------------------
*
* direction numbers
* 
 
DIR_N	EQU	0
DIR_NE	EQU	1
DIR_E	EQU	2
DIR_SE	EQU	3
DIR_S	EQU	4
DIR_SW	EQU	5
DIR_W	EQU	6
DIR_NW	EQU	7
DIR_U	EQU	8
DIR_D	EQU	9			

	IFEQ	THE_PAWN

DIR_IN		EQU	10		;in/out not reported by P.EXITS
DIR_OUT		EQU	11

Sizeof_NET	EQU	12		;bytes per room in network

	ENDC

	IFNE	THE_PAWN

Sizeof_NET	EQU	10

	ENDC

*--------------------------------
*
* misc 'conj' section word numbers
*
	
WITH		EQU	0
TO		EQU	1
FROM		EQU	2
ON		EQU	3
INTO		EQU	4
IN		EQU	5
FOR		EQU	6
BY		EQU	7
OVER		EQU	8
UNDER		EQU	9
ACROSS		EQU	10
AT		EQU	11
AROUND		EQU	12
WITHIN		EQU	13
THROUGH		EQU	14
OFF		EQU	15
ONTO		EQU	16
AGAINST		EQU	17
ABOUT		EQU	18
TOGETHER	EQU	19
UNTIL		EQU	20
BEHIND		EQU	21
AFTER		EQU	22
UPSIDEDOWN	EQU	23
 
*--------------------------------
*
*  section numbers, NB cannot simply re-order, eg code assumes
*				nouns follow verbs and adjectives
*
 
SECT.QUANT	EQU	0			;quantifiers - a,the,some,an,etc
SECT.ADV	EQU	1			;real adverbs - carefully,quietly,etc
SECT.CONJ	EQU	2
SECT.SPEC	EQU	3
SECT.VERB	EQU	4
SECT.ADJ	EQU	5
SECT.NOUN	EQU	6
SECT.DIR	EQU	7
SECT.CONT	EQU	8
SECT.QUEST	EQU	9
SECT.TOBE	EQU	10
SECT.SYN	EQU	11
SECT.YOU	EQU	12
SECT.WANT	EQU	13
SECT.THINK	EQU	14
SECT.NOT	EQU	15
SECT.INSULT	EQU	16
SECT.HELLO	EQU	17
SECT.MULT	EQU	18
SECT.MODEL	EQU	19
SECT.VAL	EQU	20
SECT.YESNO	EQU	21 
SECT.REL	EQU	22	
SECT.ONE	EQU	23 
SECT.PPRN	EQU	24
SECT.WBITS	EQU	25
SECT.STATE	EQU	26
SECT.NUM	EQU	27
SECT.SCEN	EQU	28		;scenary nouns 

*--------------------------------
 
CONT.MASK	EQU	$20000000	;bits used in location info
REL.MASK	EQU	$01000000
HIDN.MASK	EQU	$02000000
WORN.MASK	EQU	$04000000
INSP.MASK	EQU	$10000000

*--------------------------------


*
* Texture equates
*

TEXTURE.VOID		EQU	0
TEXTURE.SMOOTH		EQU	1
TEXTURE.ROUGH		EQU	2
TEXTURE.HOT		EQU	3
TEXTURE.COLD		EQU	4
TEXTURE.SLIPPERY	EQU	5
TEXTURE.FURRY		EQU	6
TEXTURE.CLAMMY		EQU	7
TEXTURE.SOFT		EQU	8
TEXTURE.HARD		EQU	9
TEXTURE.WARM		EQU	10
TEXTURE.NPC		EQU	11
TEXTURE.THORNY		EQU	12
TEXTURE.STICKY		EQU	13

*--------------------------------
 


*--------------------------------
 

	IFNE	THE_PAWN
MAXWGHT		EQU	$18		;max weight u can carry
	ENDC	;THE_PAWN

HI		EQU	$8000		;used to set hi bits
BIT14		EQU	$4000		;used for hi but-one bits
SIGCHAR		EQU	7		;no of chars search looks at
BIGWRD		EQU	26		;size of left just buffer
SLIM		EQU	3		;no. of chars nec. for plural checks


MAXROOM		EQU	200

	IFEQ	THE_PAWN

RNINVENTORY	EQU	0
NINVENTORY	EQU	0

	ENDC

	IFEQ	M68000

	include	"opsnot68k.i"

	ENDC

	IFNE	M68000

	include	"ops68k.i"

	ENDC

