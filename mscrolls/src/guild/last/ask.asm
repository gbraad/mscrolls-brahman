******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.ASK
*
*--------------------------------

* Guild of Thieves !!!!!!!!!!!!!!!!!!!!

	include	"equates.asm"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"macros.asm"

      XDEF     V.ASK,V.TELL,REPLY

      XREF     DBS,SP.ASK,VERB,NOUNBUF2,W.IDFY,CHKNPC,P.SAY2,WHOOPS,GETDATA
      XREF     COPOUT,P.SCN,SP.ASK2,P.TN,W.GLARE,P.SUBOB,RND.D1,SCREEN,EXTRA
      XREF	PRTTXT,QuoteFlag
 
V.ASK
 
	BTST    #6,4(A0)
	BEQ     DBS
	CALL    SP.ASK
	CMP.W   #VNTELL,VERB(A4)
	BEQ     V.TELL
	TEST_W  NOUNBUF2(A4)        ;INCASE NOTHING SPEC 
	BNE.S   10$
	MOVE.W	#-1,NOUNBUF2(A4)	;let ask produce the cop-out.
 
10$
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	DO	CHKNPC
	LEA     NOUNBUF2(A4),A1
	MOVEQ	#-1,D7			;keeps last msg#
20$
       	TEST_B  WHOOPS(A4)          ;ONLY ONE COPOUT PLEASE!
	BNE.S   30$
	MOVE.W  (A1)+,D0            ;ITEM
	BEQ.S   30$                 ;DONE
        BMI.S   21$                 ;dont getinfo -1
	GETINFO
21$        
	MOVE.L  A1,-(A7)
	CALL_S  ASKMAIN
	MOVE.L  (A7)+,A1
	BCLR	#0,QuoteFlag(A4)
	BEQ.S	25$
	MSG	QUOTE
25$
	MSG	LINE
	BRA.S   20$
30$
	RET
  
ASKMAIN
 
	LEA     TABLE.ASK(PC),A1    ;GET ADDR. OF TABLE OF OFFSETS
10$
	TEST_W  (A1)
	BEQ.S   30$                 ;NOT IN TABLE
	CMP.W   (A1)+,D5            ;CORRECT NOUN#?
	BEQ.S   20$                 ;YES!
	ADDQ.L  #6,A1               ;NOPE
	BRA.S   10$                 ;NEXT PLEASE..
 
20$
	MOVE.L  (A1)+,D2            ;COPOUT LIST
	ADD.W   (A1),A1             ;REPLY LIST ADDR 
	BRA.S   REPLY
30$
	CLR.L   D2

NearCopout

	CALL    SP.ASK2             ;COUNT COPOUTS (DEV,PRINCESS)
	ST      WHOOPS(A4)          ;AVOID LOST OF COPOUTS
	MOVE.W	D5,D0			;get npc back
	DO	P.SAY2
	BRA     COPOUT
 
*--------------------------------
  
REPLY
 
	MOVE.L  (A1)+,D1
	BEQ.S   10$
	CMP.W   D1,D0
	BNE.S   REPLY
	SWAP    D1
	CMP.W	D1,D7			;same as last message?
	BEQ.S	90$			;yes - don't repeat!
	MOVE.W	D1,D7			;save for next time
	MOVE.W	D5,D0			;get npc back
	DO	P.SAY2
	DOGO	P.SCN
10$
	BRA     NearCopout
90$
	RET
 
*--------------------------------
*
* TABLE.ASK - INFO ON EACH NPC'S
*             REPLIES & COPOUTS
*
* FORMAT      NOUN NO. (WORD)
*             COPOUTS  (LONG WRD)
*             NPC.REP-* (WORD)   
* 
* ZERO WORD TERMS LIST
*
*--------------------------------

AskEntry	MACRO	* noun#,copout1,copout2,copout3,copout4
		DC.W	N\1
		DC.B	\2,\3,\4,\5
		DC.W	\1.REP-*
		ENDM
		
 
TABLE.ASK
 
	AskEntry	GATEKEEPER,213,214,0,0
	AskEntry	THIEF,215,216,0,0
	AskEntry	GRAVEDIGGER,217,218,0,0
	AskEntry	MACAW,224,225,226,0
	AskEntry	MILLER,227,228,0,0
	AskEntry	GUEST,0,0,0,0

	DC.L	0
 
*--------------------------------
* 
* reply tables
*
* format:
*
* NPC#.REP
*	DC.W	msg#,noun#
*	....
*	DC.L	0		;LONG word terminator
*
*
*--------------------------------
	

GATEKEEPER.REP

	DC.W	193,NCASTLE
	DC.W	194,NBEAR
	DC.W	194,NBANQUET
	DC.W	195,NRATS
	DC.W	195,NRAT1
	DC.W	195,NRAT2
	DC.W	195,NRAT3
	DC.W	195,NRAT4
	DC.W	196,NCELLAR
	DC.W	196,NWINE1		;maybe all the wines need to be here?
	DC.W	198,NGALLERY
	DC.W	198,NPAINTING1		;ditto
	DC.W	401,NRACE		;rat (ok rodent) race
	DC.W	512,NKBANK		;bank of kerovnia
	DC.W	512,NBANK		;treat sandy bank as bank of kerovnia
	DC.W	512,NMANAGER		;treat manager as bank of kerovnia

	DC.L	0

*--------------------------------
	

THIEF.REP

*	DC.W	199,NKEROVNIA
*	DC.W	199,NOFTITCH		;hmmm
	DC.W	200,NGUILD
	DC.W	201,NCASTLE
	DC.W	201,NBARON
	DC.W	397,NGUEST
	DC.W	397,NBANQUET
	DC.W	515,NKBANK
	DC.W	515,NBANK		;treat sandy bank as bank of kerovnia
	DC.W	515,NMANAGER		;treat manager as bank of kerovnia

	DC.L	0


*--------------------------------

GRAVEDIGGER.REP

	DC.W	202,NBEAR		;goldilocks(?)+3 bears!!
	DC.W	202,NGRAVE1
	DC.W	202,NGRAVE2
	DC.W	202,NGRAVE3
	DC.W	202,NHEADSTONE1
	DC.W	202,NHEADSTONE2
	DC.W	202,NHEADSTONE3
	DC.W	203,NSHOP1
	DC.W	511,NKBANK
	DC.W	511,NBANK		;treat sandy bank as bank of kerovnia
	DC.W	511,NMANAGER		;treat manager as bank of kerovnia

	DC.L	0
	
*--------------------------------

MACAW.REP

	DC.W	208,NBIRD3		;mynah bird

	DC.L	0

	
*--------------------------------
	
MILLER.REP

	DC.W	209,NLUTE
	DC.W	210,NMILL1
	DC.W	210,NMILL2
	DC.W	210,NMILL3
	DC.W	210,NWHEATFIELD
	DC.W	210,NPWHEAT
	DC.W	211,NCASTLE
	DC.W	211,NBARON
	DC.W	212,NTEMPLE
	DC.W	484,NGUM
	DC.W	513,NKBANK
	DC.W	513,NBANK		;treat sandy bank as bank of kerovnia
	DC.W	513,NMANAGER		;treat manager as bank of kerovnia

	DC.L	0
	
*--------------------------------

GUEST.REP

	DC.W	392,NBARON
	DC.W	396,NSASH
	DC.W	448,NTRUNK
	DC.W	447,NBANQUET
	DC.W	514,NKBANK
	DC.W	514,NBANK		;treat sandy bank as bank of kerovnia
	DC.W	514,NMANAGER		;treat manager as bank of kerovnia
	DC.W	181,NCASTLE

	DC.L	0


*--------------------------------

V.TELL
	
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	DO	CHKNPC
	DO	P.TN
	BTST    #0,(A0)            ;PL
	BNE.S   50$
	MOVEQ   #2,D1
	RND_D1
	ADD.W   #74,D1		; boredom/not interested responses
	DOGO	P.SCN
	
50$
	MSG	GLARE
	BRA     P.SUBOB

*--------------------------------

      END

