******************************************************
*
*	%G%
*	%W%
*
******************************************************


*--------------------------------
*
*      SCENDATA.ASM
*
*--------------------------------
   
*--------------------------------
*
* COMMON ROUTINES - *NOT* SAVED
* WITH SAVE/LOAD GAME
*
* NOT TO BE MODIFIED BY PROG!!
*
*--------------------------------
 
  
*--------------------------------
*
* TBL.READ - table of objects which may be read
*

	XDEF	TBL.READ

TBL.READ
	MFT_Noun	BARSIGN
	MFT_Noun	BAKERYNOTICE
	MFT_Noun	DOCUMENT
	MFT_Noun	BUS_SIGN
	MFT_Noun	TICKET
	MFT_Noun	NOTICE
	MFT_Noun	BUSK_SIGN
	MFT_Noun	PARCEL
	MFT_Noun	CMAKERS_SIGN
	
	MFT_Scen	POSTAL_NOTE,59
	MFT_Scen	LETTER,213
	MFT_Scen	ENCYCLOPAEDIAS,451
	MFT_Scen	MAGAZINES,453
	MFT_Scen	BOOK1,618
	MFT_End

*
* TBL.LISTEN Does for 'listen to xyzzy' what TBL.READ does for 'read xyzzy'
*
	XDEF	TBL.LISTEN	  

TBL.LISTEN

	MFT_Scen	FRONTGARDEN1,75
	MFT_Scen	LAKE1,95
	MFT_Scen	CANOE_MOORING,95
	MFT_Scen	WOOD1,89
	MFT_Scen	TREES,89
	MFT_Scen	CORRIDOR1,97
	MFT_Scen	HOLLYBUSH,93
	MFT_Scen	MAGPIE,93
	MFT_Scen	FRIDGE,159
	MFT_Scen	KITCHEN1,159
	MFT_Scen	BULL,167
	MFT_Scen	NUKE_DEVICE,293
	MFT_Scen	INSPECTOR,276
	MFT_Scen	THEATRE,235
* Two indistinct voices can be heard.
	MFT_Scen	VOICES,236
* The voices sound pessimistic as regards your chances of survival.
	MFT_Scen	MACHINE,237
* The machine is ominously silent at the moment.
	MFT_End

*
* TBL.SMELL Does for 'smell xyzzy' what TBL.READ does for 'read xyzzy'
*
	XDEF	TBL.SMELL	  

TBL.SMELL

	MFT_Scen	PUB,350
	MFT_Scen	FRONTGARDEN1,76
	MFT_Scen	FIELD2,88
	MFT_Scen	FISHINGBOAT,94
	MFT_Scen	ONBUS,313
* Stale air and tobacco smoke assail your nostrils.
	MFT_Scen	ROSES,342
* Their delicate perfume is delightful.

	MFT_Scen	BAKERY,347
	MFT_Scen	BAKERY_OVEN,347
* The smell of freshly baked bread fills your nostrils
	MFT_Scen	THEATRE,238
* An antiseptic odour pervades here
	MFT_Scen	SOCK,9
	MFT_Scen	NATURESTUFF,10

	MFT_Scen	CHEESE,753
	MFT_Scen	CHIMNEY,712
	MFT_Scen	CASTLE_KITCHEN,711

	MFT_End

*--------------------------------
*
* INIT.TAB - LIST OF INITIAL STATE
*            DESCRIPTIONS FOR SOME
*            NOUNS
*
* FORMAT :    DC.W    MSG#,NOUN#
*
* MSGS ARE SCENARIO ONES
*
*--------------------------------
 
INIT.TAB
	DC.W	61,NVAT
	DC.W	184,NBAKER	; Baker offers you cakes
* Do we need this one now??? - Paul
	DC.L    0
  
*--------------------------------
*
* MLISTNGO -  LIST OF MESSAGES
*             USED FOR CAN'T GO
*             THAT WAY SITUATIONS
*
* FIRST WORD NEVER USED
*
*--------------------------------
 
MLISTNGO
       
		DC.W	0	;PADDING
*  nway_door
		DC.W	25
*  nway_wall
		DC.W	3
*  nway_bank
		DC.W	24
*  nway_gate
		DC.W	2
*  nway_oven
		DC.W	26
*  nway_fire
		DC.W	28
*  nway_noway
		DC.W	419
*  nway_trapdoor
		DC.W	430
* nway_hatch
		DC.W	450
* nway_train
		DC.W	475
* nway_cantmove
		DC.W	448
* nway_beam
		DC.W	591
* nway_dirt
		DC.W	100
* nway_passengers
		DC.W	733
* nway_flowers
		DC.W	734
* nway_hedgerows
		DC.W	740
* nway_edge
		DC.W	741
* nway_holly
		DC.W	742
* nway_current
		DC.W	743
* nway_steep
		DC.W	744
* nway_trees
		DC.W	745
* nway_wreckage
		DC.W	746
* nway_toorisky
		DC.W	747
* nway_carriage
		DC.W	748
* nway_railings
		DC.W	749
* nway_deadend
		DC.W	750
* nway_tobar
		DC.W	705
* nway_darktower
		DC.W	706
 
*--------------------------------
*
* MOVE.TAB - UNCONDITIONAL
*            'TELEPORTS' TO A ROOM
*            IF 'GO XYZZY'
* 
* format of table:
*
* 		DC.W	noun#
*		DC.B	direction taken(!?),target room#
*
*--------------------------------
MOVE.TAB
		

	DC.W	NLOCOMOTIVE
	DC.B	DIR_E,RNON_TRAIN

	DC.W	NLAKE2
	DC.B	DIR_N,RNLAKE1

	DC.L    0

*--------------------------------
*
* PP.OP.CL   -   LIST OF NOUNS
*                TO GET OPENED
*                OR CLOSED IF
*                PUSHED OR PULLED
*
* (PUSH/PULL.OPEN/CLOSE)
*
*  NO TERMINATOR - DOORS TABLE
*  IS PART OF IT 
*
*  FORMAT -   DC.W    NOUN#,-1
*
*  BEWARE OF THE PADDING!!
*
*--------------------------------
  
PP.OP.CL
 
  
*--------------------------------
*
*  DOORS - DIRECTIONS TO PATCH
*          INTO NETWORK WHEN DOOR
*          OPENED
*
*          ALSO AS ABOVE BUT 
*          CONDITIONAL & BIDIRECTIONAL
*          (DOOR MUST BE OPEN)
* 
*
*  FORMAT:
*              /NOUN# OF DOOR,        IE.  DC.W  NOUNNO
*              #DIR IN,#DIR OUT            DC.B  DIR_IN,DIR_OUT
*
*--------------------------------

DoorEntry	MACRO	* noun#,dir_in,dir_out
		DC.W	N\1
		DC.B	DIR_\2,DIR_\3
		ENDM
 
DOORS
	
	DoorEntry	GARDENGATE,S,N
	DoorEntry	GARDENGATE,OUT,IN
	DoorEntry	FRONT_DOOR,N,S
	DoorEntry	FRONT_DOOR,IN,OUT
	DoorEntry	GREEN_DOOR,N,S
	DoorEntry	GREEN_DOOR,OUT,IN
	DoorEntry	KITCHEN_DOOR,NE,SW
	DoorEntry	KITCHEN_DOOR,OUT,IN
	DoorEntry	WOODEN_DOOR,SW,NE
	DoorEntry	WOODEN_DOOR,OUT,IN
	DoorEntry	RED_DOOR,E,W
	DoorEntry	RED_DOOR,IN,OUT
	DoorEntry	TRAPDOOR1,D,U
	DoorEntry	TRAPDOOR1,IN,OUT
	DoorEntry	SLIDINGDOOR,S,N
	DoorEntry	AUTODOORS,N,S
	DoorEntry	AIRLOCK_DOOR,E,W
	DoorEntry	AIRLOCK_DOOR,OUT,IN
	DoorEntry	HATCH,D,U
	DoorEntry	PODOOR,SW,NE
	DoorEntry	PODOOR,OUT,NE		; Ken sez this should work.
	DoorEntry	PUBDOOR,SE,NW
	DoorEntry	PUBDOOR,SE,OUT		; Ken sez this should work.
	DoorEntry	BATHROOMDOOR,SW,NE
	DoorEntry	KITCHENDOOR,S,N
	DoorEntry	LEATHERDOOR,N,S
	DoorEntry	TELEGRAPH_DOOR,W,E
	DoorEntry	TELEGRAPH_DOOR,IN,OUT
*	DoorEntry	DOOR,N,S		;heavy wooden door
	DoorEntry	DOOR1,E,W		;tiny door to library
	DoorEntry	DOOR1,IN,OUT		;tiny door to library
	DoorEntry	WITCH_DOOR,N,S		;into witch chamber
	DoorEntry	WITCH_DOOR,IN,OUT	;into witch chamber
	DoorEntry	WITCH_DOOR1,N,S		;into witch chamber
	DoorEntry	WITCH_DOOR1,IN,OUT	;into witch chamber
	DoorEntry	SOLID_DOOR,N,S		;into the dungeon
	DoorEntry	SOLID_DOOR,IN,OUT	;into the dungeon
	DoorEntry	INNER_HATCH,N,S
	DoorEntry	OUTER_HATCH,N,S
	DoorEntry	BRIDGE,SE,N
	DoorEntry	HOLLY_BUSH,S,N		; go holly bush
	DoorEntry	BAKERYSTEPS,D,U		; go steps in bakery
	DoorEntry	CMAKERS_DOOR,SW,NE
	DoorEntry	CMAKERS_DOOR,IN,OUT
	DoorEntry	STAIRCASE3,U,D
	DC.L    0

*--------------------------------
*
*  TBL.DOOR    (TBL.DOORS)
*
*  FORMAT: /DOORN#,/KEYN#
*             ..     ..
*          /0
*
* REALLY TABLE OF THINGS THAT 
* CAN BE UNLOCKED
*
*--------------------------------
 
TBL.DOOR
	DC.W	NMAILBOX,NKEY1
	DC.W	NMAILBOX1,NKEY4
	DC.W	NGREEN_DOOR,NKEY2
	DC.W	NFRONT_DOOR,NKEY2
	DC.W	NKITCHEN_DOOR,NKEY3
	DC.L     0

 
*--------------------------------
*
* SP.JVTBL      - TABLE OF MSG 
*              IN CASE OF DEATH
*              BY JUMPING
*
* FORMAT:      /ROOM#,/SCN.MSG#
*
*
*--------------------------------
 
  
SP.JVTBL
              DC.W    0,0
 
JVTBLEND
 
**************************************************************
*
* ListNPCAllow
*
* list of verbs which a given NPC will allow on things on his person
*
* format:
*
* 	DC.W	VERB#,NPC's noun#
*
* 	DC.L	0		;long word zero terminator
*

	XDEF	ListNPCAllow

ListNPCAllow

	DC.L	0

********************************************************

	XDEF	MULTIPLE_LIST

MULTIPLE_LIST

* format	DC.W	codenumber,noun#
* The codenumber serves as an identifier, marking objects tobe in the
* same set.

	DC.W	1,NMATCH
	DC.W	1,NMATCH2
	DC.W	1,NMATCH3
	DC.W	1,NMATCH4
	DC.W	2,NBEER			; really dodgy .w
	DC.W	2,NHANDPUMP
	DC.L	0



********************************************************
	include	"nway.i"
	XDEF	NwayandDoor
* This is a simple table - every time you close an (object) it will
* search through this table and if there is a corresponding (object) it
* will fod in the nway for THAT object rather than the norm which is
* nway_door

NwayandDoor:
	DC.W	NGARDENGATE,nway_gate
	DC.W	NTRAPDOOR1,nway_trapdoor
	DC.W	NHATCH,nway_hatch
	DC.W	NINNER_HATCH,nway_hatch
	DC.W	NOUTER_HATCH,nway_hatch
	DC.W	-1
* always terminated
	XDEF	FutureEvents
* Possible flyzzable futures.
FutureEvents
	DC.W	0
	DC.W	NCHAIRS
	DC.W	NPAINTING1
	DC.W	NBANQUET_TABLE1
	DC.W	NFIREPLACE1
* no END this file is INCLUDE'd by COMMON.ASM
