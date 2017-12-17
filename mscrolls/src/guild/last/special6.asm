******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SPECIALS6
*
*--------------------------------

* Guild of Thieves !!!!!!!!!!!!!


	include	"equates.asm"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"roomequ.i"
	include	"macros.asm"
	include	"scoreref.asm"
	include "msgequ1.asm"
	include "msgequ2.asm"
	include "msgequ3.asm"



	XDEF     SP.BREAK,SP.PULL,SP.CAST,SP.LISTE,SP.SLIDE
	XDEF     SP.LIGHT,SP.PRESS,SP.DRINK,SP.TURN,SP.UNLOC,SP.SCORE,SP.RNOUN
	XDEF     SP.EAT,SP.SMOKE,SP.PUT,SP.PLANT,SP.EMPTY,SP.BUY_Before
	XDEF     SP.DROP,SP.VLIGHT,SP.UNLIGHT,SP.PUT2,SP.MEND,SP.PUTON

	XDEF	SP_MakeGlue,SP.PutBefore,SmearFish,BlackHoles,SP.BUY,SP.LEAVE

	XREF     P.SCN,SETOUT,DEAD,R.NOUN,GETDATA,OPEN,CLOSE,GETNOUNS,SETPRON
	XREF     VALID,P.TN,W.CLICK,W.NHAP,W.VANI,ISARE,YCANTW,P.DESC,STOPRON
	XREF     W.NOW,W.UNLOCK,W.IDFY,W.HAPPY,V.TASTE,V.LIGHT
	XREF     DOSCORE2,CARRIED,CHKPAUSE,YCANT,WHOOPS,SP.BRKD0
	XREF     ROOM,VERB,CONJ,LIGHT,CHEAT,CHEAT2,CHEAT.LI,PSUDAT
	XREF     NPCINRM,CLOCK,DBUG.ON,HAVEWON,ADDNOUN,SUBJECT
	XREF	GETRELN,GET_CONT,DBS,P.TNSTOP

	XREF     SNOWLOC,DO.SCORE,DO.SCN,PRTTXT,VALID.NP,P.BTNI,W.LIT
	XREF	W.OFF,P.TNIN,BSFULLSTOP,CONNECT3,SCRPNT,SEATED,P.MHAV,P.SUB

	XREF	SP.Beam,MoorFlag,RubFeet
	XREF	FlagBedup,FlagPiconwall,ScoreList
	XREF	TIMER8,FGT_Die1,FGT_Bars,DoHelpMan
	XREF	GetValue,TIMERF,TIMER7

*****************************************
*
* SP.BREAK - fake 'iron bars'

	XREF	ODT_DOOR7,Brokemirror,Lucky

SP.BREAK
	
	CMP.W	#NBUCKET,D0		;going for the bucket?
	BNE.S	01$
	CMP.W	#VNKICK,VERB(A4)	;kicking it?
	BNE.S	01$
	SCN	182			;ok,if you really want to
	POPSP
	DOGO	DEAD
01$
	CMP.W	#NHEART,D0		;Break heart?
	BNE.S	02$
	BSET	#2,(A0)			;Done before?
	BNE	90$			;=> Yup
	SCN	135
	BRA	80$
02$
	CMP.W	#NLUTE,D0
	BNE.S	05$
	MOVE.W	#264,FGT_Lute(A4)		;tweek description
	BSET	#0,FlagLute(A4)
	SCN	44				;breaks beyond hope
	BRA	80$
05$
	CMP.W	#NBARS,D0
	BEQ	DoBarsSpecial
10$
	CMP.W	#NDOOR7,D0
	BNE.S	15$
	MOVE.W	#NWINDOW,D0
15$
	CMP.W	#NWINDOW,D0		; Shop door?
	BNE.S	20$
	BCLR	#7,ODT_DOOR7(A4)	;new description on look.
	CMP.W	#VNTHROW,VERB(A4)
	BNE.S	17$
	SCN	343
* The window shatters into a vast number of glass shards.^
	BRA.S	18$
17$
	SCN	265
18$
	DO	SETOUT			; remove the window...
	MOVE.W	#NSHARDS,D0
	GETINFO
	BCLR	#7,6(A0)		;set the shards into the room.
	CALL	SETPRON
	
	Connect	SHOP1,SHOP2,S

*	MOVE.W	#RNSHOP1,D0		;from outside shop
*	MOVEQ	#DIR_S,D1		;going south
*	MOVE.W	#RNSHOP2,D2		;to inside shop (THROUGH door!)
*	CALL	CONNECT2

	Connect	SHOP1,SHOP2,IN

*	MOVE.W	#RNSHOP1,D0		;from outside shop
*	MOVEQ	#DIR_IN,D1		;going in
*	MOVE.W	#RNSHOP2,D2		;to inside shop (THROUGH door!)
*	CALL	CONNECT2

	Connect	SHOP2,SHOP1,N

*	MOVE.W	#RNSHOP2,D0
*	MOVEQ	#DIR_N,D1
*	MOVE.W	#RNSHOP1,D2
*	CALL	CONNECT2

	BRA.S	80$

20$
	CMP.W	#NCOAL,D0
	BNE.S	30$
	MOVE.L	6(A0),-(A7)		;save loc. of coal
	DO	SETOUT			;fod it from game
	MOVE.W	#NFOSSIL,D0		;+install fossil
	GETINFO
	MOVE.L	(A7)+,6(A0)		;where the coal was
	SCN	71			;message about coal
	CALL	SETPRON

	SCORE	BREAKCOAL

	BRA.S	80$
30$
	CMP.W	#NMIRROR,D0		; Break mirror?
	BNE.S	90$
	ST	Brokemirror(A4)		;So rub shoe knows
	CLR.B	Lucky(A4)		;Lose any luck we had - for good
	BSET	#2,(A0)			; Already broken?
	BNE.S	90$			; Yes!
	SCN	415
80$
	POPSP
90$
	RET


	XDEF	DoBarsSpecial

DoBarsSpecial

	SCORE	BREAKBARS

	CLR.W	FGT_Bars+2(A4)		; Remove desc. of bars from bend
	SCN	107
	DO	SETOUT

	Connect	RRM,ROOM9,NE

*	MOVE.W	#RNRRM,D0		;from echo chamber
*	MOVEQ	#DIR_NE,D1		;going ne
*	MOVE.W	#RNROOM9,D2		;to junction chamber
*	CALL	CONNECT2

*
* and back... (NB connect is likely to kill D0/D2 so a simple EXG would die)
*
	POPSP

	Connect	ROOM9,RRM,SW
	RET

*	MOVE.W	#RNROOM9,D0
*	MOVEQ	#DIR_SW,D1
*	MOVE.W	#RNRRM,D2
*	BRA	CONNECT2

*****************************************


SP.LISTE
SP.SLIDE
SP.UNLOC
SP.SCORE.NOUN
SP.SMOKE
SP.PLANT
SP.DROP
SP.MEND
SP.CAST
  	RET

*********************************
*
* SP.PULL
*
	XREF	PushPullLever,SP.MOVE.STATUE,WrenchPipe

SP.PULL
	
	CMP.W	#NPIPE,D0
	BEQ	WrenchPipe
	CMP.W	#NBOAT2,D0
	BEQ.S	10$
	CMP.W	#NMOORING,D0		;pulling boat to jetty?
	BNE.S	30$
10$
	POPSP
	BSET	#0,MoorFlag(A4)		;check where boat is/move it
	BNE.S	20$
	SCN	344
* As you pull on the rope the boat glides gently up to the jetty.
	BRA.S	90$
20$
	SCN	345
* The boat is far too heavy to be pulled out of the water by the rope.
	BRA.S	90$
30$
	CMP.W	#NLEVER,D0
	BEQ	PushPullLever
	CMP.W	#NBARS,D0
	BEQ	DoBarsSpecial

	CMP.W	#NSTATUE,D0		;pulling the statue?
	BNE.S	40$
	CMP.W	#RNTEMPLE,ROOM(A4)	;are we in the temple?
	CALL	SP.MOVE.STATUE
	BRA.S	80$
40$
	BRA.S	90$
	CMP.W	#NTRUNK,D0	; Pulling the trunk for the guest?
	BNE.S	90$
	CALL	DoHelpMan
80$:
	POPSP
90$
	RET

*--------------------------------
*
* SP.VLIGHT - special for light VERB (rather than sussing if room is dark)
*

IncenseLife	EQU	20		; How long incense burns for

SP.VLIGHT

	CMP.W	#NLAMP,D0
	BNE.S	10$
	MOVE.W	#ON,CONJ(A4)
	BRA	SP.TURN
10$
	RET

*--------------------------------
*
* SP.UNLIGHT
*

SP.UNLIGHT

	CMP.W	#NLAMP,D0
	BNE.S	10$
	MOVE.W	#OFF,CONJ(A4)
	BRA	SP.TURN
10$
	RET

*--------------------------------

List.Light

	DC.W	NLAMP
	DC.W	NWHITE		; the white makes a come back
	DC.W	0		;terminator
 
SP.LIGHT 
 
	DO	R.NOUN             ;ROOM NOUN# = D0
	GETINFO				;ROOM INFO = A0
	BTST    #2,5(A0)
	SNE     LIGHT(A4)
	BNE.S   90$
 
	LEA	List.Light(PC),A1
10$
	MOVE.W	(A1)+,D0
	BEQ.S	90$
	DO	VALID.NP
	BNE.S	10$
	GETINFO
	BTST	#7,(A0)
	BEQ.S	10$
	ST	LIGHT(A4)
	BRA.S	10$
90$
	IFNE     YCHEAT
	MOVE.W  CHEAT(A4),D0       ;VARIOUS CHEAT WAYS TO GET LIGHT
	OR.W    CHEAT2(A4),D0
	OR.W    CHEAT.LI(A4),D0
	OR.B    D0,LIGHT(A4)
	
	ENDC
 
	RET
  

*--------------------------------
* BEDDOWN moves the bed down.
BEDDOWN
	CLR.B	FlagBedup(A4)
	MOVE.W	#NBED2,D0
	GETINFO
	BCLR	#7,6(A0)	; Bed now IN
	TEST_B	FlagPiconwall(A4)
	BEQ.S	10$		; Move abstract in ?
	MOVE.W	#NABSTRACT,D0	; Yes, so do it!
	GETINFO
	BCLR	#7,6(A0)
10$:
	MOVE.W	#NBED2UP,D0
	DOGO	SETOUT

*--------------------------------
* BEDUP moves the bed up.
BEDUP
	ST	FlagBedup(A4)
	MOVE.W	#NBED2UP,D0
	GETINFO
	BCLR	#7,6(A0)	; Bed now IN
	MOVE.W	#NBED2,D0	; Hide the real bed
	DO	SETOUT
	TEST_B	FlagPiconwall(A4)
	BEQ.S	10$
	MOVE.W	#NABSTRACT,D0
	DO	SETOUT		; ... and hide the hole.
10$:
	RET
 
* Move all on bed to the lab.
CATAPULT:

	MOVE.W	#NBED2,D2	; Get all related to bed
	DO	GETRELN
10$
	MOVE.W	(A1)+,D0
	BEQ.S	99$		; End of list?
	GETINFO
	CMP.B	#1,6(A0)	; *Really* on the bed?
	BNE.S	10$
	MOVE.L	#RNLAB,6(A0)	; Throw into the lab!
	BRA.S	10$
99$
	RET

; CHKWITH Validates the 'with' part of 'press button with widget'
; Returns EQ if item is OK
CHKWITH
	CMP.W	#WITH,CONJ(A4)		; 'With' something?
	BNE.S	25$

	MOVE.W	D5,D0
	DO	CARRIED			; Must be carrying the tool
	BNE.S	32$
	GETINFO
	MOVE.B	1(A0),D1
	LSR.B	#4,D1
	CMP.B	#4,D1			;hardness 3 or less?
	BLT.S	10$                     ;too soft
	CMP.B	#12,D1
	BGE.S	10$                     ;>=12 too hard
	SETEQ
	RET
10$
	DO	YCANTW
	BRA.S	90$
25$:
	SCN	365			; Cant quite reach
	BRA.S	90$
32$:
	CALL	P.MHAV			; You must have the xyzzy
90$:
	SETNE
	RET	

*--------------------------------
* SP.PRESS Copes with pressing the up and down buttons for the folding bed.
* When sitting on the bed, you must push buttons with an object.
* 

SP.PRESS
	CMP.W	#NDOWNBUTTON,D0		; Press the 'Down button'
	BNE.S	20$
	CMP.W	#NBED2,SEATED(A4)	; Whilst sitting on the bed?
	BNE.S	10$
	CALL	CHKWITH
	BNE.S	190$
10$:
	TEST_B	FlagBedup(A4)		; Is bed UP ?
	BNE.S	15$
12$:
	MSG	CLICK			; No, so nuffink happens
	BRA.S	190$
15$:
	SCN	366			; The bed descends...
	CALL	BEDDOWN

190$:
	BRA.S	90$

* Now look after the 'up' button
20$:
	CMP.W	#NUPBUTTON,D0		; Up button??
	BNE.S	SP.PRESS2	
	CMP.W	#NBED2,SEATED(A4)	; Sitting comfortably?
	BNE.S	40$			; Nope, so easy!

	CALL	CHKWITH			; With something suitable?
	BNE.S	90$
	SCN	367			; Bed flies upwards...
	TEST_B	FlagPiconwall(A4)	; Picture on wall?
	BNE.S	37$			; IF so, kill him.
	SCN	369			; Hurtle into lab.
	CALL	CATAPULT		;do now else thing's are not local
	MOVE.W	#RNLAB,ROOM(A4)		; Move player to lab.
	CLR.W	SEATED(A4)
	MOVE.W	#1,SCRPNT(A4)		;give a description first time
	BRA.S	85$			; move bed, etc

* Smashes into painting, so kill him
37$:
	SCN	368
	POPSP
	DOGO	DEAD
	
* Not sitting on bed, so rather different.
40$:
	TEST_B	FlagBedup(A4)		; Is bed up?
	BNE.S	12$			; Click, and that's it!
	SCN	372			; Bed flies upwards!
	TEST_B	FlagPiconwall(A4)	; Painting on wall?
	BNE.S	85$			; yes, so don't catapult. 	
	CALL	CATAPULT
85$
	CALL	BEDUP			; Move the correct bed in, etc.
90$
	POPSP				; Show we handled the special
	RET

	XDEF	SP.PRESS2

SP.PRESS2

	CMPI.W	#NBUTTON,D0		; Opening till?
	BNE	90$
	MOVE.W	#NTILL,D0
	GETINFO
	BCLR	#5,(A0)			; Open the till...
	BNE.S	70$			; Unless it already was!
	MSG	CLICK
	BRA.S	80$
70$
	BSET	#4,(A0)			; Till now open
	SCN	160
80$
	POPSP				; That's all!
90$
	RET 
*--------------------------------
 
 
SP.DRINK
	CMP.W	#NRATWATER,D0		; Drink water with rats in it?
	BNE.S	90$
	SCN	440			; kill the player!
	POPSP
	DOGO	DEAD
90$:
 	RET

*--------------------------------
 
SP.TURN
;* Make turn stopcock (on/off) work
	CMP.W	#NSTOPCOCK,D0		; Twist/turn stopcock?
	BNE.S	SP.TURN2
	POPSP
	CMP.W	#ON,CONJ(A4)
	BEQ.S	10$			; Open stopcock...
	CMP.W	#OFF,CONJ(A4)
	BEQ	CLOSE
	BTST	#4,(A0)			; Is it open
	BNE	CLOSE			; Yes, so close it!
10$:
	BRA	OPEN			; Open it!

SP.TURN2	
	CMP.W	#NLAMP,D0		;our light source for today?
	BNE.S	20$

	CMP.W	#ON,CONJ(A4)		;turn on?
	BNE.S	10$			;nope
	POPSP				;fod return address
	PEA	BSFULLSTOP(PC)		;replace it with routine to put '.'
					;on end of msg
	TEST_W	LIGHT(A4)		;was room lit already?
	BNE.S	05$			;ne=> yes
	MOVE.W	#1,SCRPNT(A4)		;so screen refreshes room name etc
05$

	BeSure	M.LIT

	MOVE.W	#M.LIT,D1		;'lit' message (as P.BTNI wants D1)
	BSET	#7,(A0)			;set lit bit+test if it was already
	BNE	P.BTNI			;ne=> is already alight
	DOGO	P.TNIN			; 'the xyzzy is now lit'
10$
	CMP.W	#OFF,CONJ(A4)		;turn off?
	BNE.S	20$			;nope
	POPSP				;fod return address

	BeSure	M.OFF

	MOVE.W	#M.OFF,D1		;'lit' message (as P.BTNI wants D1)
	BCLR	#7,(A0)			;fod lit bit+test if it was already
	BEQ	P.BTNI			;eq=> is already off
	DOGO	P.TNIN			;'the xyzzy is now off'
	
20$
	CMP.W	#NMIRROR,D0		;trying to turn the mirror?
	BNE.S	99$
	CMP.B	#1,FlagBeam(A4)		;is the mirror in the beam?
	BNE.S	99$
	SCN	361			;little dots of light go round cage
	BRA	SP.Beam			;finally resting on the wax...
80$
	POPSP
99$
	RET
	
 
 
 
 
 
 
 
*--------------------------------
*
* SP.SCORE - ADD TO SCORE  IF
*               WE GET TO A SECIAL
*            PLACE  - IE PAST ROCKS
*
*--------------------------------
 
 
SP.SCORE
	
	LEA     ScoreList_ROOMS(PC),A1
	MOVE.W  ROOM(A4),D1
 
10$
	MOVE.L  (A1)+,D0
	BEQ.S   20$
	CMP.W   D1,D0           ;LOW WORD IS ROOM
	BNE.S   10$
	SWAP    D0              ;HI WORD IS SCORE  ADDR
	LEA	ScoreList(A4),A2
	ADD.W	D0,A2
	CALL    DOSCORE2       ;OK IF IN ONE ROOM CAN'T BE ANYWHERE ELSE
 
20$
	RET

* this would check ALL nouns that can score, but eats CPU

*	LEA     ScoreList_NOUNS(PC),A1
*30$
*	MOVE.L  (A1)+,D0
*	BEQ.S   40$
*	DO	CARRIED
*	BNE.S   30$              ;NOPE
*	SWAP    D0
*	LEA	-4(A1,D0.W),A2
*	CALL    DOSCORE2
*	BRA.S   30$              ;NEXT
*40$
*	RET

* get any score going for noun in D0
	
	XDEF	SP.SCORE.NOUN,SP.SCORE.TREASURE

SP.SCORE.TREASURE

	PUSH_L	D1/A1
	LEA     ScoreList_NOUNS(PC),A1
10$
	MOVE.L  (A1)+,D1
	BEQ.S   20$
	CMP.W	D1,D0		;right noun?	
	BNE.S	10$
*	DO	CARRIED
*	BNE.S   20$              ;NOPE
	SWAP    D1
	LEA	ScoreList(A4),A2
	ADD.W	D1,A2
	CALL    DOSCORE2
20$
	PULL_L	D1/A1
	RET
 
	XDEF	IsTreasure,IsTreasureNotCastle

* IsTreasure - returns NE if a given object is scoreing obj

IsTreasure

	PUSH_L	D1/A1
	LEA     ScoreList_NOUNS(PC),A1
	CALL	IsTreasure_sub
	PULL_L	D1/A1
	RET

* IsTreasureNotCastle  - returns NE if a given object is scoreing obj
*                        but stolen from OUTSIDE!

IsTreasureNotCastle

	PUSH_L	D1/A1
	LEA     ScoreList_NotCastle(PC),A1
	CALL	IsTreasure_sub
	PULL_L	D1/A1
	RET

IsTreasure_sub

10$
	MOVE.L  (A1)+,D1
	BEQ.S   20$
	CMP.W	D1,D0		;right noun?	
	BNE.S	10$
	SETNE
20$
	RET


* list of nouns you get points for - DC.W SC.fred,Nfred
	XDEF	T_Carried
	XREF	QCARRIED
T_Carried:
* returns in D1 the number of treasures player is carrying...
	PUSH_L	A1/D0
	LEA	ScoreList_NOUNS(PC),A1	
	CLR.W	D1
10$
	MOVE.L	(A1)+,D0		; the next treasure
	BEQ.S	90$			; finished
	CALL	QCARRIED		; NE => not carried
	BNE.S	10$
	ADDQ	#1,D1
	BRA.S	10$
90$
	PULL_L	A1/D0
99$	
	RET

 
ScoreList_NOUNS

SL_NOUN	MACRO	* noun
	DC.W	SC.\1,N\1
	ENDM

	SL_NOUN	CHALICE		;nouns stolen from castle
	SL_NOUN	FOSSIL
	SL_NOUN	PAINTING1
	SL_NOUN	RUBY
	SL_NOUN	POT
	SL_NOUN	DRESS
	SL_NOUN	RING
	SL_NOUN	BAG2	

ScoreList_NotCastle

	SL_NOUN RHINO
	SL_NOUN	PLECTRUM	;nouns stolen from elsewhere
	SL_NOUN	BURNER
	SL_NOUN	STATUETTE
	SL_NOUN	CHIPS
	SL_NOUN	GEM
	SL_NOUN	CUBE2		; The ivory cube is a treasure for thief.ajs
	SL_NOUN	BROOCH
	DC.L    0
	
* list of rooms you get points for
 
ScoreList_ROOMS

SL_ROOM	MACRO	* room
	DC.W	SC.\1,RN\1
	ENDM

	SL_ROOM	MILL1
	SL_ROOM	CELLAR
	SL_ROOM	SUMP
	SL_ROOM LAB
	SL_ROOM	SHOP2
	SL_ROOM	WHITESQUARE
	SL_ROOM	LIBRARY2
	SL_ROOM	STPS
	SL_ROOM	INBANK
	SL_ROOM	MANOFFIS
	SL_ROOM	TILL1

	DC.L    0
 
SP.RNOUN
  
	EXG     D0,D1
	DO	R.NOUN
	EXG     D0,D1
	RET
 
SP.EAT

      IFNE	YCHEAT
	CMP.W	#NGUILD,D0
	BNE.S	05$
	NOT.B	DBUG.ON(A4)
        BRA.S   80$
      ENDC
05$
	CMP.W	#NCOCONUT,D0
        BNE.S   10$
   	BTST	#2,(A0)		;is it broken?
	BNE.S	90$
	SCN	383
	BRA	80$
10$

        BRA.S   90$
80$
	POPSP
90$
	RET
 
 
 

*******************************************************
* PUTON happens when doing PUT x on y, b4 any verb stuff
*
SP.PUTON
 
	CMP.W	#ON,CONJ(A4)
	BEQ.S	05$
	CMP.W	#ONTO,CONJ(A4)
	BNE.S	90$
05$:
	CMP.W	#RNFOURROOM,ROOM(A4)	; In the four room?
	BEQ	DoMachine		; yup
	CMP.W	#NPOISON,D0
	BNE.S	10$
	CMP.W	#NDFISH,D5
	BNE.S	10$
	CALL	SmearFish
	BRA.S	80$
	
10$:
	CMP.W	#NSUCCULENTS,D0
	BNE.S	30$
	CMP.W	#NFEET,D5
	BNE.S	30$
	CALL	RubFeet
	BRA.S	80$

30$
	CMP.W	#NFIRE1,D5	; Putting things on fire
	BNE.S	40$
************************************************************
* This bit about burning things on fires is *DODGY*
;	AND.B	#$F0,3(A0)	; Clear texture
;	ORI.B	#3,3(A0)	; Make it hot.
;	BCLR	#5,3(A0)	; .. and not wet?
;	BRA.S	90$
;	MOVE.W	#WITH,CONJ(A4)	; fake up light x with y
;
;	CALL	V.LIGHT
;	BTST	#7,6(A0)	; Did it burn ?
;	BNE.S	80$
;	MOVE.W	#$100,6(A0)	; Related.
;	MOVE.W	D5,8(A0)	; Put it on fire
************************************************************
	BRA.S	90$
	BRA.S	80$

40$
	CMP.W	#NMAGGOT,D0		;putting the maggot on the needle?
	BNE.S	90$
	CMP.W	#NNEEDLE,D5
	BNE.S	90$

	DO	FIXCONT
	MOVE.L	#$01000000+NNEEDLE,6(A0)
	DO	P.TN
	DO	ISARE
	MSG	NOW
	MSG	ON
	EXG	D0,D5
	DO	P.TNSTOP

80$:
	POPSP
	RET
90$:
	BRA	SP.PutBefore


*
* Called by throw x at y, put x at y, to fod objects put in water,etc 
*

BlackHoles

	PUSH_L	D1/A0

* stop 'put player in moat' & 'put feet in moat' fuckups
	
	BTST	#4,3(A0)		;moveable?
	BEQ.S	99$

	MOVE.B	6(A0),D1		;get locinfo
	AND.B	#$18,D1			;intrinsic or insep?
	BEQ.S	04$
	SETEQ				;exit - we didn't do anything
	BRA.S	99$
04$
	MOVEQ	#118,D1			; Set up 'Glop' message
	CMP.W	#NPOOL,D5		;trying to put something in the pool
	BEQ.S	10$
	LEA	BlackHoleList(PC),A0
05$
	MOVE.W	(A0)+,D1
	BEQ.S	99$			;not in our list exit EQ
	CMP.W	D1,D5
	BNE.S	05$

	MOVE.W	#487,D1			; Ordinary boring message
10$:	
	DO	CARRIED			; check we have the object!
	BEQ.S	15$
	CALL	P.MHAV
	BRA.S	20$
15$
	DO	P.TN
	DO	P.SCN
	DO	SETOUT			;get rid of object
20$:
	SETNE
99$:
	PULL_L	D1/A0
	RET

BlackHoleList

	DC.W	NMOAT			; for moat..
	DC.W	NRIVER			; river..
	DC.W	NLAKE			; lake..
	DC.W	NHOLE			; hole in bedroom
	DC.W	0

* this special gets called before it does anything with the verb routine

	XREF	Weigh_Flag,SP.GET
        XREF    HoldMirror

SP.PutBefore
	CMP.W	#NFIRE1,D5
	BNE.S	03$
	SCN	482			;now look here...
	BRA.S	04$
03$
	CMP.W	#NDEADFLIES,D0		;put flies?
	BNE.S	05$
	CMP.W	#NNEEDLE,D5		;On needle?
	BNE.S	05$
	SCN	242			; really - grow up!
04$
	POPSP
	RET
05$

	CMP.W	#NGUM,D0		;sticking the gum anywhere?
	BEQ	SP.PutGum

	PEA	07$(PC)			;to come back if pop'ed by sp.get
	CALL	SP.GET
	POPSP

	CALL	NeedleMoat		; putting the needle in the moat?
	CALL	BlackHoles		; See if we ditch object?
	BNE	80$
	CMP.W	#RNFOURROOM,ROOM(A4)	; putting things in pocket?
	BNE.S	10$
	CMP.W	#NPOCKET1,D5		
	BNE.S	10$
	SCN	78			; no time
	ST	WHOOPS(A4)		; no more please chaps.
07$
	POPSP
	RET
10$

        CALL    HoldMirror              ; put mirror in beam
        BEQ.S   80$
20$
	CMP.W	#NBIRD3,D0
	BNE.S	30$
	CMP.W	#IN,CONJ(A4)
	BEQ.S	25$
	CMP.W	#INTO,CONJ(A4)
	BNE.S	30$
25$:
	CMP.W	#NCAGE4,D5		; into bird cage is OK!
	BEQ.S	30$
	MOVE.W	D5,STOPRON(A4)		; Set up for message
	PUSH_L	D0-D1/A0
	MOVE.W	D5,D0
	GETINFO
	MOVE.B	10(A0),D1
	AND.B	#7,D1			; put into container ?
	PULL_L	D0-D1/A0		; restore regs.
	BEQ.S	30$			; nope, so let verb routine handle it

	SCN	421			; bird refuses to let you

80$:
	POPSP
30$
90$:	
	RET
 
 
*--------------------------------
*
* SP.PUT - CALLED AT THE *END* 
*          OF PUT - ONCE ACTION
*          HAS HAPPENED *MUST*
*          RETURN TO VERB ROUTINE
*
*--------------------------------

SP.PUT

	CALL	SP.SCORE.NOUN		;score points if obj is now carried
	RET


	XREF	FlagTollGate,FlagBeam

SPOTMSG5	EQU	128	; Five spots per side
 
SP.PUT2

	EXG	D0,D5			;'cos of wierd posn in V.PUT
	CMP.W	#NCAULDRON,D5		;mixing glue potion?

	BNE.S	10$
	CALL	SP_MakeGlue
	BRA	90$
10$
	CMP.W	#NSLOT1,D5		;into the toll gate's slot?
	BNE.S	20$
	DO	SETOUT			;whatever it is you won't get it back!
	CMP.W	#NPIECE,D0		;the 'two freg coin'?
	BNE.S	90$			;nope - pity...
	SCN	109			;barrier now up (mechanism breaks)
	ST	FlagTollGate(A4)	;ne=. been pain
	BRA.S	90$
20$
	CMP.W	#NSLOT2,D5		; Put things in dice room slots
	BLT.S	90$
	CMP.W	#NSLOT5,D5
	BGT.S	90$

	PUSH_L	D0-D2/A0-A1
	MOVE.W	#NDIE1,D0		; initial die #
	MOVE.L	#CONT.MASK+NSLOT2,D2	; slot #
	LEA	FGT_Die1(A4),A1	 ; first glance for die
35$:
	GETINFO
	CMP.L	6(A0),D2		; contained in correct slot
	BNE.S	85$
	CMP.W	#HI+SPOTMSG5,(A1)	 ; Check spots on each die 
	BNE.S	85$
	ADDQ.L	#4,A1			; Increment first glance number
	
	ADDQ.W	#1,D2			;Increment slot number
	ADDQ.W	#1,D0			;Increment die number
	CMP.W	#NDIE4,D0		; Done last die?
	BLE.S	35$
	MOVE.W	#NCASE,D0		; Case already unlocked??
	GETINFO
	BCLR	#6,(A0)			; Already unlocked ? 
	BEQ.S	85$
*	BCLR	#5,(A0)			; Now unclose it!
*	BSET	#4,(A0)			; Now open it!
	SCN	131			; Case pops open
	SCORE	GETDIE			; let us score some points for it..
*	MOVE.W	#NDIE5,D0
*	GETINFO
*	MOVE.L	#CONT.MASK+NCASE,6(A0)
*	CLR.W	FGT_Case+2(A4)		; Remove desc. of die in case
85$
	PULL_L	D0-D2/A0-A1
90$
	EXG	D0,D5
	RET

***********************************
*
* SP.PutGum - catch them vandalising things with the gum
*

	XREF	GumFlag

	XDEF	SP.PutGum

SP.PutGum

	BTST	#2,GumFlag(A4)		;has it been chewed?
	BEQ.S	90$			;no - its just normal

	TEST_W	D5			;something to put it on/in?
	BLS.S	90$

	POPSP				;we won't want to go back

	BTST	#4,3(A0)		;check movable bit 
	BEQ.S	80$			;DBS - u have already stuck it!

	MOVE.B	1(A5),D1		;get hardness of obj we're putting
	LSR.B	#4,D1			;gum onto
	CMP.B	#2,D1			;is it too soft/void?
	BLS.S	80$
	BTST	#4,6(A5)		;a room?
	BNE.S	80$
	BTST	#6,4(A5)		;is it an NPC
	BNE.S	80$
	BTST	#0,5(A5)		;a psuedo?
	BEQ.S	10$
	MOVE.W	ROOM(A4),D1
	MOVE.B	D1,7(A0)		;The SECONDARY LOCATOR!!!!!
10$
	MOVE.B	#$01,6(A0)		;simply relate so it say 'there is
	MOVE.W	D5,8(A0)		;some gum on it'

	CMP.W	#NKEYHOLE,D5
	BNE.S	15$
	MOVE.B	#$30,6(A0)		;put in inseperably IN the keyhole

15$
	BCLR	#4,3(A0)		;fod movable bit 

	SCN	41			;'the gum is now stuck to '
	DO	FIXCONT

	MOVE.W	D5,D0

	DOGO	P.TNSTOP
80$
	DOGO	DBS
90$
	RET



***********************************
*
* SP_MakeGlue - suss it producing
*
* see if 1- the noun is one of the ingredients
*        2- the rest of them are there too
*
* D0 = ingredient

SP_MakeGlue

	PUSH_L	D0-D2/D5/A0-A1/A5
	MOVE.W	D0,D2			;so as we can check if it's n list
	MOVE.W	#NCAULDRON,D0
	GETINFO
	MOVE.L	D0,D5
	MOVE.L	A0,A5			;make sure cauldron info is avail
	LEA	List_magic_potion(PC),A1
	MOVEQ	#4,D1			;no. of ingredients less 1
10$
	MOVE.W	(A1)+,D0
	GETINFO
	CMP.L	#CONT.MASK+NCAULDRON,6(A0)
	BNE.S	80$
	CMP.W	D0,D2
	BNE.S	20$
	MOVEQ	#0,D2
20$
	DBRA	D1,10$
	TEST_W	D2
	BNE.S	80$			;not in list
*
* success!!!
*
*	
	LEA	List_magic_potion(PC),A1
	MOVEQ	#4,D1			;no. of ingredients less 1
30$
	MOVE.W	(A1)+,D0
	DO	SETOUT			;fod ingredients from game
	DBRA	D1,30$
	MOVE.W	#NCAULDRON,D0		;anything else there?
	DO	GET_CONT		;returns A1 pointing at list.
40$
	MOVE.W	(A1)+,D0
	BEQ.S	50$			;zero terminated
	BMI.S	40$			;Hi bit set means n/a
	CMP.W	#NCUBE1,D0		;is it a cube that we are changing?
	BNE.S	50$			;all goes up
	GETINFO				
	BSET	#7,6(A0)		;set it ou
	MOVE.W	#NANTECUBE,D0		;get the ante cube
	GETINFO
	BCLR	#7,6(A0)		;set it in
	SCN	328			;a bang
	SCORE	ANTECUBE
	BRA.S	70$			;go and do it
50$
* get rid of the cauldron
	MOVE.W	#NCAULDRON,D0		;here it is
	DO	SETOUT			;it goes away
	SCN	89			;with a bang
70$
	ST	WHOOPS(A4)		;kill process loop for multi D.O.

80$
	PULL_L	D0-D2/D5/A0-A1/A5

90$
	RET


List_magic_potion

	DC.W	NPOWDER,NEYE,NHEART,NBONE,NBERRIES
 
*--------------------------------
 
 
*--------------------------------
 
SP.EMPTY
	CMP.W	#RNFOURROOM,ROOM(A4) 	; In the four room no empty allowed
	BNE.S	99$
	SCN	70			; Can't be done chaps
	POPSP
99$
	RET

*--------------------------------
	        
	
*--------------------------------
*
* SP.MEND
*

	XREF	FlagLute,FGT_Lute


SmearFish

	MOVE.W	#NPOISON,D0
	DO	SETOUT			; remove the poison
	MOVE.W	#NPFISH,D0
	CALL	SETPRON			; 'It' is now fish
	GETINFO
	MOVE.L	A0,A2
	MOVE.W	#NDFISH,D0
	GETINFO
	MOVE.L	6(A0),6(A2)		; Switch locations
	SCN	477			; You put poison on fish
	DOGO	SETOUT

*--------------------------------

        XREF    MillerWineFlag,SetValue

SP.BUY

	CMP.W	#NLUTE,D0		;buying the lute?
	BNE.S	90$
	CMP.W	#NCHEQUE,D5		;with the cheque?
	BNE.S	90$
        TEST_B  MillerWineFlag(A4)
        BEQ.S   10$
        SCN     485                     ;accepts lower price due to wine
        BRA.S   80$
10$        
	SCN	516			;miller accepts 55f cheque
80$
	SETEQ
90$
	RET

SP.BUY_Before
        
	PUSH_L	D0/D1/D5
	CMP.W	#NLUTE,D0		;buying the lute?
	BNE.S	90$
	CMP.W	#NCHEQUE,D5		;with the cheque?
	BNE.S	90$
        EXG     D0,D5
	CALL	GetValue                ;of cheque(d0)
	CMP.B	#55,D1
	BPL.S	90$			;>=55 fergs is enough
        TEST_B  MillerWineFlag(A4)
        BEQ.S   10$                     ;not given the wine (remaster)
        CALL    SetValue                ;set lute(d5) to cheque value(d1)
        BRA.S   90$
10$
	SCN	527			;sorry that's not enough
	PULL_L	D0/D1/D5
	POPSP
	RET
90$
	PULL_L	D0/D1/D5
	RET

*---------------------------

SP.LEAVE

	CMP.W	#NQUEUE2,D0		;leaving the longer queue?
	BNE.S	5$
	TEST_B	TIMER7(A4)		;teller talking means in long queue
	BEQ.S	30$
	BRA.S	20$
5$
	CMP.W	#NQUEUE1,D0		;leaving the shorter queue
	BNE.S	90$
	TEST_B	TIMERF(A4)		;<>0 means we're in the queue
	BEQ.S	30$
20$
	SCN	541			;you leave the q
	MOVE.W	#RNINBANK,ROOM(A4)	;and go to the main bank area
	CLR.W	SEATED(A4)		;could be sitting on the counter
	CLR.B	TIMER7(A4)		;shut up teller
	CLR.B	TIMERF(A4)		;no more descriptions
	BRA.S	40$
30$
	SCN	542			;you must be in a queue to leave it
40$
	POPSP
90$
	RET

*--------------------------

	XREF	NOUNBUF,FIXCONT
	XREF	SpotBits,FIXCHK
	XDEF	DoMachine,MachineDied

DoMachine:

* it will get here if the verb is put - 
* and the room is Fourrroom.

	CMP.W	#NWMACHINE,D5		; onto machine?
	BNE  	40$			; nope.
	CMP.B	#2,Weigh_Flag(A4)	; and has it been done?
	BEQ  	70$ 
	CMP.W	#NANTECUBE,D0		; the ante cube? 
	BEQ.S	10$
	CMP.W	#NDIE5,D0		; Or the dice...?
	BNE.S	01$			;MachineDied- You are killed by the machine
	CMP.W	#NANTECUBE,2+NOUNBUF(A4)	; and the ante cube?
	BEQ.S	20$
01$
	BRA.S	21$				;MachineDied
10$
	CMP.W	#NDIE5,2+NOUNBUF(A4)		; + die?
	BNE.S	21$				;MachineDied	; you die.
20$
	CMP.B	#%00111111,SpotBits(A4)		; dice done?
	BEQ.S	30$				; yup.
	SCN	547				; dice is just too light...
21$
	BRA	MachineDied			; and you die...

30$
* okay they are putting the ante cube and the dice onto the machine
* it should work....
* Are either of them fixed to anything?
	PUSH_L	D0/D1/A1		; general purpose...
	MOVE.W	#NDIE5,D0
	DO	FIXCHK			; NE => things fixed.
	BNE.S	35$
	MOVE.W	#NANTECUBE,D0		; NE => things fixed
	DO	FIXCHK			; NE => things fixed.
	BNE.S	35$
	PULL_L 	D0/D1/A1
	BRA.S	37$
35$
	PULL_L	D0/D1/A1		; things werwe tied.
	BRA.S	MachineDied
37$
	MOVE.B	#1,Weigh_Flag(A4)	; it has been done..
	SCN	546			; the machine shudders slightly..
	MOVE.W	#NDIE5,D0
	DO	FIXCONT			; Just in case it was contained
	GETINFO
	MOVE.L	#REL.MASK+NWMACHINE,6(A0)
	MOVE.W	#NANTECUBE,D0
	DO	FIXCONT
	GETINFO
	MOVE.L	#REL.MASK+NWMACHINE,6(A0)	; put it on machine
	POPSP				; we don't want to go back
	ST	WHOOPS(A4)		; NO MOre parsing please
40$
	RET

70$

* only 'allowed combination' is cube and anticube...

	CMP.W	#NCUBE2,D0		; CUBE?
	BEQ.S	80$
	CMP.W	#NCUBE2,NOUNBUF+2(A4)	; or other noun prase?
	BNE.S	MachineDied
	CMP.W	#NANTECUBE,D0		; With antecube
	BNE.S	MachineDied
75$
	SCN	411			; a bit clumsy this time
	BRA.S	MachineDied
80$
	CMP.W	#NANTECUBE,NOUNBUF+2(A4)
	BEQ.S	75$

****************
*
* FALLS THROUGH!!!!!!

MachineDied 

	SCN	540			; alarm bells ring out
	POPSP				; no verb
	DOGO	DEAD			; and you die.

	XREF	SP.FISH			; 
NeedleMoat:
* are we trying to put the needle into the maot?
	CMP.W	#NNEEDLE,D0		
	BNE.S	99$
	CMP.W	#NMOAT,D5
	BNE.S	99$
	POPSP				; so as we don't return from this call
	CLR.W	D2
	CLR.W	D3
	MOVE.W	D5,D4			; strange format for sp.fish
	BRA	SP.FISH
99$
	RET


      END


