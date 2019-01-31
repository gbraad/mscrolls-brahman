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

* JINXTER

	include	"equates.i"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"macros.i"
	include "roomequ.i"

      XDEF     V.ASK,V.TELL,REPLY

      XREF     DBS,SP.ASK,VERB,NOUNBUF2,W.IDFY,CHKNPC,P.SAY2,WHOOPS,GETDATA
      XREF     COPOUT,P.SCN,SP.ASK2,P.TN,W.GLARE,P.SUBOB,RND.D1,SCREEN,EXTRA
      XREF	PRTTXT,P.ENDQUOTE
 
V.ASK
 
	TST_NPC				;
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

* ok, so it's not nice- I blame the parents myself...

	CMP.W	#NGUARD,D5
	BEQ.S	15$
	CMP.W	#NSTATION_MASTER,D5
	BEQ.S	15$
	CMP.W	#NGUARDIAN,D5
	BEQ.S	15$

	DO	P.SAY2
15$
	LEA     NOUNBUF2(A4),A1
	MOVEQ	#-1,D7			;keeps last msg#
20$
	MOVE.W  (A1)+,D0            ;ITEM
	BEQ.S   30$                 ;DONE
	TEST_B   WHOOPS(A4)          ;ONLY ONE COPOUT PLEASE!
	BNE.S   30$
	GETINFO
	MOVE.L  A1,-(A7)
	CALL_S  ASKMAIN
	MOVE.L  (A7)+,A1
	BRA.S   20$
30$
	CMP.W	#NGUARD,D5
	BEQ.S	40$
	CMP.W	#NSTATION_MASTER,D5
	BEQ.S	40$
	CMP.W	#NGUARDIAN,D5
	BNE	P.ENDQUOTE
40$
	RET
  
ASKMAIN
 
	LEA     TABLE.ASK(PC),A1    ;GET ADDR. OF TABLE OF OFFSETS
10$
	TEST_W   (A1)
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
	ST      WHOOPS(A4)          ;AVOID LOTS OF COPOUTS
	CLR.L   D2
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

	CALL	Exceptions		;Guardian won't tell unless it's local

	DOGO	P.SCN
10$
	CALL    SP.ASK2             ;COUNT COPOUTS (DEV,PRINCESS)
	ST      WHOOPS(A4)          ;AVOID LOST OF COPOUTS
	BRA     COPOUT
90$
	RET
 

*
*
* Exceptions	-	Guardian will only give reply in table if the
*			noun you are asking about is local.
*
*	Takes:		D5	NPC
*			D1	SCN # of msg to br printed on return
*			D0	Noun being asked about
*

	XREF	ROOM
Exceptions

	CMP.W	#NGUARDIAN,D5		;Is it Wossname?
	BNE.S	90$			;If not, not interested
	ST	WHOOPS(A4)		;No multiples
	CMP.W	#NNICECLOUD,D0		;Waaa, the cloud goes away too soon
	BNE.S	10$
	CMP.W	#RNEND_OF_BEAM,ROOM(A4)	;Always allow in this room only
	BEQ.S	90$
10$
	DO	VALID.NP
	BEQ.S	90$
	MOVE.W	#608,D1			;MSG # for 'can't see one' msg
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

	AskEntry	BAKER,0,0,0,0
	AskEntry	PUBLICAN,0,0,0,0
	AskEntry	PUBCROWD,0,0,0,0
	AskEntry	POSTMISTRESS,0,0,0,0
	AskEntry	STATION_MASTER,229,0,0,0
	AskEntry	GUARD,229,0,0,0
	AskEntry	GUARDIAN,245,246,247,248
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
BAKER.REP
	DC.W	544,NOVEN
	DC.W	545,NINGREDIENTS
	DC.W	586,NPUBLICAN
	DC.W	586,NPUB
	DC.W	587,NPOSTMISTRESS
	DC.W	713,NPUDDING
	DC.W	713,NPUDDING2
	DC.L	0

PUBCROWD.REP
	DC.W	424,NPUBLICAN
	DC.W	425,NPUB
	DC.W	426,NBEER
	DC.W	426,NHANDPUMP
	DC.W	427,NCHARM1		; This seems a leetle tacky??
	DC.W	427,NCHARM2		; 28 words per person who ...
	DC.W	427,NCHARM3		; wants to know about a charm.
	DC.W	427,NCHARM4
	DC.W	427,NCHARM5
	DC.W	588,NBAKER
	DC.W	589,NPOSTMISTRESS
	DC.L	0

PUBLICAN.REP
	DC.W	546,NBAKER
	DC.W	547,NBEER
	DC.W	547,NHANDPUMP
	DC.W	548,NPUB
	DC.W	549,NPUBCROWD
	DC.L	0

POSTMISTRESS.REP
; ask about SCALES,POST OFFICE,POSTMAN??? - (There is no postman!)

	DC.W	574,NSCALES
	DC.W	575,NPOSTOFFICE
	DC.W	577,NPOSTMAN
	DC.W	584,NPUBLICAN
	DC.W	585,NPUB
	DC.L	0

STATION_MASTER.REP
	DC.W	206,NTRAIN_TICKET
	DC.W	207,NLOCOMOTIVE
	DC.W	218,NTRAIN_PASSENGERS
	DC.W	219,NBUSKING
	DC.W	220,NGUARD
	DC.L	0
GUARD.REP
	DC.W	221,NTRAIN_TICKET
	DC.W	222,NLOCOMOTIVE
	DC.W	224,NTRAIN_PASSENGERS
	DC.W	227,NBUSKING
	DC.W	228,NSTATION_MASTER
	DC.L	0

GUARDIAN.REP
* NB all these must be local when you ask about them - else cop-outs
* Should only be used for random guardian appearances.

	DC.W	652,NCANOE
	DC.W	652,NLAKE2
	DC.W	652,NWATER
	DC.W	652,NCANOE_HOLE

	DC.W	656,NMAGPIE
	DC.W	656,NHOLLY_BUSH

	DC.W	660,NMAILBOX1

	DC.W	664,NSLIDINGDOOR
	DC.W	664,NRUNNERS
	DC.W	664,NBOATHOUSE

	DC.W	668,NDIRTMOUND

	DC.W	672,NGIRDER

	DC.W	676,NMODELS			;Weathermen

	DC.W	680,NNICECLOUD

	DC.W	684,NPUB
	DC.W	684,NASH
	DC.W	684,NPUBFIRE
	DC.W	684,NPUBHEARTH

	DC.W	688,NLOCOMOTIVE
	DC.W	688,NSTATION_MASTER
	DC.W	688,NTRAIN_TICKET

	DC.W	692,NPRECIPICE
	DC.W	692,NCASTLE

	DC.W	696,NDUNGEON
	DC.W	696,NINNER_HATCH
	DC.W	696,NSOLID_DOOR
	DC.W	696,NDUNGEON_ROPE

	DC.W	700,NSAFE

	DC.L	0

*----------------------------


V.TELL
	
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	DO	CHKNPC
	DO	P.TN
	TST_PLURAL				;PL
	BNE.S	50$
	DO	ISARE
	MSGGO	NOTINTERESTED
	
50$
	MSG	GLARE
	BRA	P.SUBOB

*--------------------------------

      END

