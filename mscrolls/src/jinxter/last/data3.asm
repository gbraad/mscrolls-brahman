******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

* jinxster

*--------------------------------
*
*  SRC.ADV.DATA3
*
*--------------------------------


	XDEF	ERR1,ERR2,ERR3,ERR4,GLOBAL.F
	XDEF	REP1

;	XDEF	SCAN.SUB,SCANVERB,SCAN.OBJ,SCANMIXN,IUFLAG
;	XDEF	KYWRD,ISARETYP,NPC.LBF1,NPC.LBF2,NPC.TOBE,NPCQUEST
;	XDEF	NPCMODEL,NPCABOUT,NPC.WRD,PERSON,TENSE


	
*--------------------------------
*
* TIME MAPS
*
*
*
*
*  00 00 00 MEANS STOP		
*		
*  00 AB CD MEANS GOTO ABCD IN TABLE
*
*
*--------------------------------
*
* FORMAT :
*
*
* CURRLOC		DC.W	0   CURRENT ROOM#
*
* OFFSET		DC.W	0   OFFSET INTO
*				TIME MAP TABLE
*
*
*	HEX   
*
*	DC.B	$00,$00,$00,$00
		
	
	
*--------------------------------
*
* THIS IS A WHOLE BILE OF
* JUNK TAKEN OUT OF VARIOUS
* BITS OF CODE TO AVOID LOCAL
* DATA AREAS
*
*--------------------------------
  
*--------------------------------
*
*  NPC.SUB2 LOCAL BITS
*
* REALLY QUITE GLOBAL SINCE
* THESE ARE THE ERROR SUPRESSION
* FLAGS
*
* YUK!
*
*--------------------------------
 
ERR1		DC.W	0
ERR2		DC.W	0
ERR3		DC.W	1
ERR4		DC.W	0	;FLAGS NO WHICH (ETC)  
GLOBAL.F	DC.W	0
 
*--------------------------------
*
*  NPC.SUB LOCAL BITS
*
*--------------------------------
  
REP1		DC.W	0
;SCAN.SUB	DC.W	0
;SCANVERB	DC.W	0
;SCAN.OBJ	DC.W	0
;SCANMIXN	DC.W	0
;IUFLAG		DC.W	0
;KYWRD		DC.W	0
;ISARETYP	DC.W	0
	
*--------------------------------
*
*  PARSER.NPC LOCAL BITS
*
*--------------------------------
 
	IFNE	0

NPC.LBF1
	
*LOCAL BUFFER FOR NPC PARSE 
	
	DS.B	$20
	
NPC.LBF2
	
	DS.B	$10
	
   
*RESULT FORMAT:
	
NPC.TOBE	DC.W	-1
NPCQUEST	DC.W	-1
NPCMODEL	DC.W	-1
NPCABOUT	DC.W	0
NPC.WRD		DC.L	0
TENSE		DC.W	0

* 0 = PRES
* 1 = PRES PART
	
	ENDC	;0

	XDEF	PERSON

PERSON		DC.W	2
	
	
*--------------------------------

	END

