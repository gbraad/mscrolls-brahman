******************************************************
*
*	%G%
*	%W%
*
******************************************************

		SECTION		"ADV",CODE

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
 
  
*
* TBL.LISTEN Does for 'listen to xyzzy' what TBL.READ does for 'read xyzzy'
*
	XDEF	TBL.LISTEN	  

TBL.LISTEN
	MFT_Scen	HEART,417
	MFT_Scen	ORGAN,394
	MFT_Scen	PIANO,157
* scorpions?

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

	DC.W	154,NBOAT
	DC.W	161,NMILL1	;in the mill
	DC.W	172,NCEMTR	;cemetry - gravedigger
	DC.W	170,NAVRE	; Macaw message!
*	DC.W	37,NJAILER

* next 4 pairs are in same loc.


	DC.W	176,NPODIUM1
	DC.W	177,NDIE1	;red die
	DC.W	176,NPODIUM2
	DC.W	178,NDIE2	;green die
	DC.W	176,NPODIUM3
	DC.W	179,NDIE3	;blue die
	DC.W	176,NPODIUM4
	DC.W	180,NDIE4	;yellow die

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
		DC.W	241
*  nway_wall
		DC.W	184
*  nway_riverbank
		DC.W	185
*  nway_trees
		DC.W	186
*  nway_castlemoat
		DC.W	187
*  nway_vanes
		DC.W	188
*  nway_toll
		DC.W	189
*  nway_shop
		DC.W	190
*  nway_slope
		DC.W	191
*  nway_death
		DC.W	192
*  nway_river
		DC.W	238
*  nway_lake
		DC.W	239
*  nway_railings
		DC.W	240
*  nway_pigeon
		DC.W	242
*  nway_rockface
		DC.W	243
*  nway_fridge
		DC.W	244
*  nway_ironbars
		DC.W	245
*  nway_misc1
		DC.W	246
*  nway_misc2
		DC.W	247
*  nway_misc3
		DC.W	248
*  nway_misc4
		DC.W	249
*  nway_misc5
		DC.W	250
*  nway_misc6
		DC.W	0
 
 
 
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
		
	DC.W	NBOAT2
	DC.B	DIR_E,RNBOAT

	DC.W	NJETTY2
	DC.B	DIR_W,RNJETTY

	DC.W	NMILL2
	DC.B	DIR_S,RNMILL1

	DC.W	NCOALS
	DC.B	DIR_W,RNSTPS

	DC.W	NKBANK
	DC.B	DIR_S,RNINBANK
 
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
	DoorEntry	DOOR1,N,S
*	DoorEntry	DOOR3,S,N
	DoorEntry	DOOR4,W,E
	DoorEntry	DOOR5,SW,NE
	DoorEntry	IGATE,SE,NW
	DoorEntry	DOOR6,SE,NW
	DoorEntry	DRAWBRIDGE,N,S
	DoorEntry	DOOR7,S,N
	DoorEntry	STAIRCASE,U,D
	DoorEntry	ROPE,U,D
	DoorEntry	LADDER,U,D
	DoorEntry	OAKDOOR,S,N
	DoorEntry	MANDOOR,S,N
	DoorEntry	PRIVDOOR,E,W

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

	DC.W	NCAGE3,NKEY1		;key to bear's cage
	DC.W	NDOOR4,NKEY2
	DC.W	NDOOR6,NCKEY		;door into cage (zoo)
	DC.W	NPRIVDOOR,NIVORYKEY	;door to zoo office

 	DC.W	NSARCOPHAGUS,NFINGER	; finger bone for sarcophagus

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

	DC.W	VNREAD,NGUEST		;so u can read the sash
	DC.W	VNPUSH,NGUEST		;so u can push trunk ?
	DC.W	VNPULL,NGUEST		;so u can pull trunk ?
	DC.W	VNGET,NGUEST		;so trunk is too heavy to lift?
	DC.W	VNBET,NGATEKEEPER	;so u can bet on rats
					; (possibly with gatekeepers coin!)
	DC.L	0


* no END this file is INCLUDE'd by COMMON.ASM
