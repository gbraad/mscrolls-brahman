**************************************************************
*
*	%W%
*	%G%
*
**************************************************************

* Guild of Thieves protection list (may live UNDER the stack!)

* Ken 26/4/87

	SECTION	"ADV",code

	include	"equates.asm"
	include	"nounequ1.i"
	include	"verbequ.i"
	include	"adjequ.i"


Position	MACRO	* word(as per text),page#,col#,line#,word#
* \1
		DC.W	(\3<<14)+(\2<<9)+(\4<<4)+\5

		ENDM

WordTable	MACRO	* section,word	(as per matchbuf)
	
		DC.W	(\1<<10)+\2

		ENDM

	XDEF	ProtList,ProtSize,ProtEnd


	DS.B	32				;spare stack

ProtList


	Position	THIEF,12,1,3,1
	WordTable	SECT.NOUN,NTHIEF-1
	Position	CUT,12,1,5,3
	WordTable	SECT.VERB,VNCUT
	Position	TAKE,12,1,9,5
	WordTable	SECT.VERB,VNTAKE
	Position	NOTE,12,1,9,6
	WordTable	SECT.NOUN,NNOTE-1
	Position	LARGE,12,1,25,3
	WordTable	SECT.ADJ,ADJ_large-4
	Position	CLOUDS,12,1,25,4
	WordTable	SECT.NOUN,NWCLOUD-1
	Position	STOP,12,2,4,7
	WordTable	SECT.VERB,VNSTOP
	Position	PICK,12,2,9,2
	WordTable	SECT.VERB,VNPICK
	Position	NOTICE,12,2,14,2
	WordTable	SECT.ADJ,ADJ_notice-4
	Position	STAND,12,2,15,3
	WordTable	SECT.VERB,VNSTAND
	Position	DOWN,12,2,16,7
	WordTable	SECT.DIR,DIR_D
	Position	STOOL,13,1,3,6
	WordTable	SECT.NOUN,NSTOOL-1
	Position	UP,13,1,5,2
	WordTable	SECT.DIR,DIR_U
	Position	WISH,13,1,11,5
	WordTable	SECT.WANT,$0001
	Position	EXITS,13,1,23,1
	WordTable	SECT.VERB,VNEXITS
	Position	OUT,13,1,24,1
	WordTable	SECT.VERB,VNEXIT
	Position	EXIT,13,1,24,3
	WordTable	SECT.VERB,VNEXIT
	Position	DRINK,13,2,7,1
	WordTable	SECT.VERB,VNDRINK
	Position	SOME,13,2,13,1
	WordTable	SECT.QUANT,$0002
	Position	BOTTLE,13,2,13,5
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	LARGE,13,2,18,4
	WordTable	SECT.ADJ,ADJ_large-4
	Position	ALL,14,1,12,2
	WordTable	SECT.SPEC,$0001
	Position	DROP,14,1,13,5
	WordTable	SECT.VERB,VNDROP
	Position	GET,14,1,15,2
	WordTable	SECT.VERB,VNGET
	Position	ALL,14,1,15,3
	WordTable	SECT.SPEC,$0001
	Position	ALL,14,1,15,7
	WordTable	SECT.SPEC,$0001
	Position	ROOM,14,1,16,5
	WordTable	SECT.NOUN,NROOM1-1
	Position	ROOM,14,1,19,2
	WordTable	SECT.NOUN,NROOM1-1
	Position	GLASS,14,2,4,2
	WordTable	SECT.NOUN,NWINDOW-1
	Position	GLASS,14,2,6,5
	WordTable	SECT.NOUN,NWINDOW-1
	Position	GLASS,14,2,7,2
	WordTable	SECT.NOUN,NWINDOW-1
	Position	GLASS,14,2,7,5
	WordTable	SECT.NOUN,NWINDOW-1
	Position	GLASS,14,2,8,3
	WordTable	SECT.NOUN,NWINDOW-1
	Position	POINT,14,2,9,3
	WordTable	SECT.VERB,VNPOINT
	Position	GLASS,15,1,4,3
	WordTable	SECT.NOUN,NWINDOW-1
	Position	STOP,15,1,12,3
	WordTable	SECT.VERB,VNSTOP
	Position	DRINK,15,1,14,2
	WordTable	SECT.VERB,VNDRINK
	Position	CASE,15,1,21,3
	WordTable	SECT.NOUN,NCASE-1
	Position	PUT,15,2,2,4
	WordTable	SECT.VERB,VNPUT
	Position	COIN,15,2,2,6
	WordTable	SECT.NOUN,NPIECE-1
	Position	ONTO,15,2,2,7
	WordTable	SECT.CONJ,ONTO
	Position	COIN,15,2,3,6
	WordTable	SECT.NOUN,NPIECE-1
	Position	ON,15,2,4,3
	WordTable	SECT.CONJ,ON
	Position	INTO,15,2,4,6
	WordTable	SECT.CONJ,INTO
	Position	COIN,15,2,5,7
	WordTable	SECT.NOUN,NPIECE-1
	Position	COIN,15,2,8,6
	WordTable	SECT.NOUN,NPIECE-1
	Position	ASK,15,2,9,6
	WordTable	SECT.VERB,VNASK
	Position	YOU,15,2,10,1
	WordTable	SECT.YOU,$0000
	Position	POINT,15,2,12,3
	WordTable	SECT.VERB,VNPOINT
	Position	ONE,15,2,13,6
	WordTable	SECT.ONE,$0000
	Position	SMALL,16,1,1,2
	WordTable	SECT.ADJ,ADJ_small-4
	Position	CHEST,16,1,1,3
	WordTable	SECT.NOUN,NCHEST-1
	Position	GREEN,16,1,3,2
	WordTable	SECT.ADJ,ADJ_green-4
	Position	LARGE,16,1,4,1
	WordTable	SECT.ADJ,ADJ_large-4
	Position	CHEST,16,1,4,2
	WordTable	SECT.NOUN,NCHEST-1
	Position	BROKEN,16,1,20,5
	WordTable	SECT.ADJ,ADJ_broken-4
	Position	DOWN,16,1,21,1
	WordTable	SECT.DIR,DIR_D
	Position	FIVE,16,1,21,3
	WordTable	SECT.ADJ,ADJ_five-4
	Position	GET,16,1,26,3
	WordTable	SECT.VERB,VNGET
	Position	ALL,16,1,26,4
	WordTable	SECT.SPEC,$0001
	Position	GREEN,16,2,1,7
	WordTable	SECT.ADJ,ADJ_green-4
	Position	BOTTLE,16,2,2,1
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	GREEN,16,2,2,4
	WordTable	SECT.ADJ,ADJ_green-4
	Position	BOTTLE,16,2,3,1
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	BOTTLE,16,2,4,4
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	BOTTLE,16,2,5,5
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	MAN,16,2,7,1
	WordTable	SECT.NOUN,NGUEST-1
	Position	OPEN,16,2,10,3
	WordTable	SECT.VERB,VNOPEN
	Position	BOTTLE,16,2,10,5
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	BOTTLE,16,2,11,4
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	HUGE,16,2,17,3
	WordTable	SECT.ADJ,ADJ_huge-4
	Position	BOTTLE,16,2,22,5
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	DOOR,17,1,8,1
	WordTable	SECT.NOUN,NDOOR1-1
	Position	SPOT,17,1,11,5
	WordTable	SECT.ADJ,ADJ_spot-4
	Position	SPOT,17,1,14,4
	WordTable	SECT.ADJ,ADJ_spot-4
	Position	BITE,17,1,16,5
	WordTable	SECT.VERB,VNBITE
	Position	WHAT,2,1,1,1
	WordTable	SECT.QUEST,$0002
	Position	GUILD,2,1,3,2
	WordTable	SECT.NOUN,NGUILD-1
	Position	RHINO,2,1,11,5
	WordTable	SECT.NOUN,NRHINO-1
	Position	MASTER,2,1,13,2
	WordTable	SECT.ADJ,ADJ_master-4
	Position	MANY,2,1,20,3
	WordTable	SECT.VAL,$0001
	Position	FIND,2,2,2,4
	WordTable	SECT.VERB,VNFIND
	Position	NORTH,2,2,14,1
	WordTable	SECT.DIR,DIR_N
	Position	STOP,2,3,10,1
	WordTable	SECT.VERB,VNSTOP
	Position	SHOW,2,3,11,2
	WordTable	SECT.VERB,VNSHOW
	Position	WERE,2,3,26,4
	WordTable	SECT.TOBE,$0003
	Position	CHEST,3,1,6,1
	WordTable	SECT.NOUN,NCHEST-1
	Position	FOOL,3,1,18,2
	WordTable	SECT.INSULT,$000A
	Position	FACE,3,2,1,4
	WordTable	SECT.NOUN,NFACE-1
	Position	MUST,3,2,9,2
	WordTable	SECT.MODEL,$0000
	Position	PEOPLE,3,2,30,5
	WordTable	SECT.NOUN,NPEOPLE-1
	Position	WORK,3,3,10,2
	WordTable	SECT.THINK,$0007
	Position	RHINO,3,3,11,2
	WordTable	SECT.NOUN,NRHINO-1
	Position	ABOUT,3,3,20,1
	WordTable	SECT.CONJ,ABOUT
	Position	BOTTLE,4,1,3,1
	WordTable	SECT.NOUN,NBOTTLE1-1
	Position	PLACE,4,1,18,5
	WordTable	SECT.VERB,VNPLACE
	Position	DRINK,4,1,24,1
	WordTable	SECT.VERB,VNDRINK
	Position	GLASS,4,3,19,5
	WordTable	SECT.NOUN,NWINDOW-1
	Position	DOOR,4,3,30,2
	WordTable	SECT.NOUN,NDOOR1-1
	Position	THINGS,5,1,11,3
	WordTable	SECT.SPEC,$0001
	Position	EYE,5,1,18,3
	WordTable	SECT.NOUN,NEYE-1
	Position	ENTRANCE,5,1,25,1
	WordTable	SECT.ADJ,ADJ_entrance-4
	Position	FIELD,5,2,14,4
	WordTable	SECT.NOUN,NFIELD-1
	Position	MUST,5,2,31,5
	WordTable	SECT.MODEL,$0000
	Position	ROOM,5,3,12,3
	WordTable	SECT.NOUN,NROOM1-1
	Position	SWAG,5,3,15,2
	WordTable	SECT.ADJ,ADJ_swag-4
	Position	THIRD,6,1,14,1
	WordTable	SECT.ADJ,ADJ_third-4
	Position	FIRST,6,2,4,4
	WordTable	SECT.ADJ,ADJ_first-4
	Position	LOOKING,6,2,9,1
	WordTable	SECT.ADJ,ADJ_looking-4
	Position	GIVE,6,3,1,4
	WordTable	SECT.VERB,VNGIVE
	Position	PLACE,7,1,1,2
	WordTable	SECT.VERB,VNPLACE
	Position	SKELETON,7,1,11,1
	WordTable	SECT.NOUN,NSKELETON-1
	Position	SMEAR,7,1,16,5
	WordTable	SECT.VERB,VNRUB
	Position	WINDOW,7,1,27,3
	WordTable	SECT.NOUN,NWINDOW-1
	Position	PAPER,7,1,30,2
	WordTable	SECT.ADJ,ADJ_paper-4
	Position	CHAIR,7,2,4,1
	WordTable	SECT.NOUN,NCHAIR1-1
	Position	KNOW,7,2,10,5
	WordTable	SECT.THINK,$0002
	Position	SOME,7,3,12,2
	WordTable	SECT.QUANT,$0002
	Position	FOOL,7,3,15,2
	WordTable	SECT.INSULT,$000A
	Position	CASE,8,2,12,4
	WordTable	SECT.NOUN,NCASE-1
	Position	THIEF,8,2,20,3
	WordTable	SECT.NOUN,NTHIEF-1
	Position	THIEVES,8,3,16,3
	WordTable	SECT.NOUN,NGUILD-1
	Position	LAUGH,8,3,24,1
	WordTable	SECT.VERB,VNLAUGH
	Position	NIGHT,8,3,31,2
	WordTable	SECT.ADJ,ADJ_night-4
	Position	YELLOW,9,1,14,2
	WordTable	SECT.ADJ,ADJ_yellow-4
	Position	SMELL,9,1,15,4
	WordTable	SECT.VERB,VNSMELL
	Position	WHERE,9,2,2,4
	WordTable	SECT.QUEST,$0000
	Position	MOST,9,3,18,2
	WordTable	SECT.QUANT,$0005
	Position	BLOW,10,1,4,3
	WordTable	SECT.VERB,VNBLOW
	Position	OFF,10,1,18,2
	WordTable	SECT.CONJ,OFF
	Position	PEOPLE,10,1,21,2
	WordTable	SECT.NOUN,NPEOPLE-1
	Position	OVER,10,2,3,1
	WordTable	SECT.CONJ,OVER
	Position	RATS,10,3,7,2
	WordTable	SECT.NOUN,NRATS-1
	Position	DIE,10,3,20,3
	WordTable	SECT.NOUN,NDIE1-1
	Position	PLACE,10,3,31,2
	WordTable	SECT.VERB,VNPLACE

ProtSize	EQU	(*-ProtList)/4

	DS.B		8			;enough for 2 ret addr.

ProtEnd

	END
