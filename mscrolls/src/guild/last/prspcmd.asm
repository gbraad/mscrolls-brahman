*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"roomequ.i"
	include "adjequ.i"
	include "nway.asm"

	include	"scoreref.asm"

	XREF	VERB,CHEAT,CONVNPC,SEATED,RND.D1,SUBJECT,P.TN,TIMERB,TIMER9
	XREF	SUBJECT2,NOUN,NOUNBUF,NPCCMDPT,PR20,NPCQUEST,DEAD,ROOM
	XREF	NPCMODEL,NPC.LBF1,P.SAY,P.BOTN,DO.SCORE,VALID.NP
	XREF	BACK.WRD,DO.SCN,PRTTXT,P.SAY
	XREF	GETDATA,UTIL.BS,FNDWRD,CHAROUT,GETWRD,SPACE,FlagBear
	XREF	CALCNET,TIMER7,TIMERF,GETNPC

	XDEF	PRSPCMD,PR.SPEC,PRSPDO,SP.ODREP,SP.SAY,SP.SPEECH

*-----------------------------------
*
* PRSPCMD - this deals with giving NPC's commands
*
* in The Pawn the horse stuff was here, in the Guild Miller/Gatekeeper bits
*
* we get here when stuff like 'miller, stop windmill' is hit
* 
	
PRSPCMD

	
	CMP.B   #SECT.DIR,D6   	;IS A STRIAGHT DIRECTION?
	BNE.S   10$
	MOVE.W  #VNGO,VERB(A4)
	BRA.S   20$
 
10$
	MOVE.W  D5,VERB(A4)
	
20$
	LEA	OrderNPC(PC),A0
	MOVE.W  CONVNPC+4(A4),D0	;noun # of npc in question
	MOVE.W	VERB(A4),D5		;verb in question
30$
	MOVE.W	(A0)+,D1		;get verb no. we're looking at
	BMI.S	40$			;forced match (-1)
	CMP.W	D1,D5
	BEQ.S	40$		
	ADDQ.W	#2,A0			;keep in phase with table
	BRA.S	30$

* ok, verb matches, does npc?

40$
	MOVE.W	(A0)+,D1		;get noun# we're looking for
	BEQ.S	90$			;not in table, exit 'definitely not'
					;test here 'cos 0 is allowed verb#
	BMI.S	50$			;forced match?
	CMP.W	D1,D0			;right npc?
	BNE.S	30$			;nope...
50$

	MOVE.L  CONVNPC(A4),A0		;this is probably pointless..

	MOVE.W  D0,SUBJECT(A4)		;so verbs/verbsubs know its an NPC
	MOVE.B  #1,SUBJECT2(A4)		;one day Hugh will tell me....
	MOVE.L  (A7)+,D0	;GET IT (I think this is just a stack pop - Ken)

	CLR.W   NOUN(A4)		;ZAP NOUNBUFF FOR NEW CMD
	CLR.W   NOUNBUF(A4)
	BRA     NPCCMDPT		;go parse command
	
* exit via here if not doing anything/have done all that's going to happen
* this is a quiet exit - no more will be printed

80$
	MOVE.L  (A7)+,D0       	;DOWN
	DOGO	PR20

* exit via here to let 'definitely not' happen

90$
	DOGO	P.SAY
     
***
*
* OrderNPC - table of things u can tell NPC's to do
*
* format:
*
* Order	NPC (noun#),VERB
*
* where 'any' lets anything go
*
**

Order	MACRO	* npc,verb,noun1,conj,noun2,code

      IFNC	'any','\2'
	DC.W	VN\2
      ENDC
      IFC	'any','\2'
	DC.W	-1
      ENDC

      IFNC	'any','\1'
	DC.W	N\1
      ENDC
      IFC	'any','\1'
	DC.W	-1
      ENDC

	ENDM


OrderNPC

	Order	MILLER,STOP
	Order	FAKEMILLER,STOP
	Order	GATEKEEPER,LOWER
	Order	GATEKEEPER,LIFT
	Order	GATEKEEPER,RAISE
	Order	GATEKEEPER,PULL

	DC.L	$FFFF0000			;NB long word terminator

*--------------------------------
*	       
*	PR.SPEC
*	
* SPECIALS FOR NPC INTERACTION
* LIVE HERE
*
*	ENTRY:
*	 D2=PERSON
*	 CONVNPC+4 = NPC IN CONVERSATION
*	 CONVNPC   = DATA ON NPC
*
* we get here when u ask an npc a question eg, guru,why are u laughing

PR.SPEC
	
	LEA     L.PRSP(PC),A0
02$
	MOVE.W  (A0),D1
	BEQ.S   90$            	;NOT IN LIST
	CMP.W   CONVNPC+4(A4),D1
	BEQ.S   10$            	;OK
05$
	ADD.W   #14,A0         	;SKIP TO NEXT SECTION
	BRA.S   02$            	;AGAIN
10$
	MOVE.W  2(A0),D1
	BMI.S   15$            	;SKIPP
	CMP.W   NPCQUEST(A4),D1
	BNE.S   05$            	;FAIL
15$
	MOVE.W  4(A0),D1       	;GET MODEL
	BMI.S   20$            	;OPTIONAL
	CMP.W   NPCMODEL(A4),D1	;SAME?
	BNE.S   05$            	;FAIL
20$
	MOVE.W  6(A0),D1      	;GET MAIN NOUN
	BMI.S   30$            	;OPTIONAL
	CMP.W   #2,D1          	;3RD OR NOUN?
	BLT.S   25$            	;NO, IS PERSON CHK
	CMP.W   NPC.LBF1(A4),D1	;AS IN BUFFER?
	BNE.S   05$            	;NO, FAIL
	BRA.S   30$            	;OK PASS
25$
	CMP.W   D2,D1          	;AS PERSON?
	BNE.S   05$            	;FAILK
30$
	MOVE.W  8(A0),D1       	;GET VERB
	BMI.S   40$            	;OPTIONAL
	CMP.W   VERB(A4),D1
	BNE.S   05$            	;FAIL
40$
	MOVE.W  10(A0),D1      	;INDIRECT NOUN
	BMI.S   50$            	;OPTIONAL
	CMP.W   NOUNBUF(A4),D1
	BNE.S   05$            	;NOPE
50$
*
*
*  SUCESS!
*
	MOVE.W  12(A0),D1      	;displacement
	LEA     PRSPDO(PC),A0
	ADD.W   D1,A0
	JSR     (A0)
	MOVE.L  (A7)+,D0       	;GET RET
	DOGO	PR20           	;EXIT
	
90$
	RET

*--------------------------------
*
*
*
* FORMAT FOR SP.SPECIALS
*
* /NPC (ESSENTIAL)
* /QUEST (OPTIONAL=-1) ,
* /MODEL (OPTIONAL=-1),  (can =1 may =2)
* /DIRECT OBJECT (OPTIONAL=-1)
*         0=IST PERSON
*         1=2ND PERSON
*      NOUN= MAIN NOUN
* /VERB (OPTIONAL =-1)
* /INDIRECT OBJECT (OPTIONAL=-1)
* /ADDR OF HANDLER (REL TO PRSPDO)
*
*--------------------------------
	
L.PRSP
     
* TABLE OF SPECIALS

	IFNE	0		;use this one as an example
	
	       DC.W    NGURU
	       DC.W    4             	;WHY?
	       DC.W    -1            	;NO MODEL
	       DC.W    1             	;YOU
	       DC.W    VNLAUGH       	;LAUGHING
	       DC.W    -1            	;NONE
	       DC.W    PRSP1-PRSPDO

	ENDC

	DC.W	NGUEST
	DC.W	-1
	DC.W	1		;can
	DC.W	0		;I
	DC.W	VNHELP
	DC.W	-1
	DC.W	CanHelp-PRSPDO

	DC.W	NGUEST
	DC.W	-1
	DC.W	2		;may
	DC.W	0		;I
	DC.W	VNHELP
	DC.W	-1
	DC.W	CanHelp-PRSPDO

	DC.W	NMILLER
	DC.W	-1
	DC.W	1		;can
	DC.W	0		;I
	DC.W	VNTAKE
	DC.W	NLUTE
	DC.W	BorrowLute-PRSPDO

	DC.W	NMILLER
	DC.W	-1
	DC.W	1		;can
	DC.W	0		;I
	DC.W	VNGET
	DC.W	NLUTE
	DC.W	BorrowLute-PRSPDO
	       
	DC.W	NMILLER
	DC.W	-1
	DC.W	2		;may
	DC.W	0		;I
	DC.W	VNTAKE
	DC.W	NLUTE
	DC.W	BorrowLute-PRSPDO

	DC.W	NMILLER
	DC.W	-1
	DC.W	2		;may
	DC.W	0		;I
	DC.W	VNGET
	DC.W	NLUTE
	DC.W	BorrowLute-PRSPDO
	       
	DC.W    0             	;TERMINATE
	
*--------------------------------
*	
* NON-STANDARD REPLYS
*
* Does things like the SNOWMAN always failing to communicate speil
*
	
SP.ODREP

	CMP.W	#NBIRD3,D0		;teaching mynah bird?
	BEQ	TeachBird
	CMP.W	#NHOORAY,D0
	BEQ	SayHooray
	CMP.W	#NFAKEMILLER,D0
	BNE.S	10$
	CMP.W	#VNSHOUT,VERB(A4)
	BEQ.S	10$
	SCN	425
	BRA	80$
10$
	PUSH_L	D0/A0/A3
	DO	GETNPC
	BTST	#2,2(A3)		;is it an animal?
	BNE.S	15$			;yes, don't worry about manners
	MOVE.W	#NGUM,D0		; Chewing the gum?
	GETINFO
	CMP.L	#$08000000,6(A0)	; Intrinsic?
15$
	PULL_L	D0/A0/A3			;leaves flags alone
	BNE.S	20$			;NE=> no gum or is an animal
	DO	P.TN
	DO	UTIL.BS
	SCN	446			;looking at in an odd fashion..
	BRA.S	80$
20$
	CMP.W	#NPEOPLE,D0		;speaking to the customers
	BEQ.S	25$
	CMP.W	#NQUEUE1,D0		;speaking to a queue?
	BEQ.S	25$
	CMP.W	#NQUEUE2,D0
	BNE.S	28$
25$
	SCN	543			;they ignore you
	BRA.S	80$
28$
	CMP.W	#NBEAR,D0
	BNE.S	40$
	TEST_B	FlagBear(A4)		;Bear passive?
	BEQ.S	30$			;=> nope
	SCN	60			;Bear is unconscious
	BRA.S	80$
30$
	SCN	306			;bear stares at u and growls
	BRA.S	80$
40$
	CMP.W	#NTELLER,D0
	BNE.S	50$
	CMP.W	#VNASK,VERB(A4)
	BEQ.S	90$
	SCN	133			;no conversation in a public place
	BRA.S	80$
50$
	CMP.W	#NMANAGER,D0
	BNE.S	90$
	SCN	100			;market forces
80$
	POPSP
	SETNE
90$	
	RET

SP.SPEECH

	MOVE.L	A6,-(A7)
	CMP.W	#RNWELL1,ROOM(A4)
	BLT.S	05$			;underwater?
	CMP.W	#RNTUNNEL,ROOM(A4)
	BGT.S	05$
	SCN	197			;glug!
	SETEQ
	BRA.S	90$
05$
	DO	GETWRD
	CMP.W	#SECT.NOUN,D6
	BNE.S	90$
	CMP.W	#NHOORAY-1,D5
	BNE.S	10$
	CALL	SP.HOORAY
	SETEQ
	BRA.S	90$
10$
	CMP.W	#NSPELL-1,D5		;random song, pendra
	BNE.S	90$
	CALL_S	SP.SING
	SETEQ				;EQ => did something (!?!)
90$
	MOVE.L	(A7)+,A6
	RET

	XREF	FlagLute
*
* WARNING!!!! SP.PLAY will come here if u play lute after singing
*

	XDEF	SP.SING
	XREF	FlagInAir,SETOUT,GETRELN
        XREF    REMASTER

SP.SING

	BTST	#3,FlagLute(A4)		; Has he played the lute...?
	BNE.S	10$
	SCN	433			; Needs musical accomp.
	MOVE.B	#3,TIMER9(A4)		;u get 2 moves to play lute
	BSET	#4,FlagLute(A4)		;say u have sung song
	BRA	90$
10$:
	BTST	#0,FlagLute(A4)		; Is it Broken??
	BNE	70$			; Yep, so KILL!!!
	MOVE.W	#NLUTE,D0
	DO	VALID.NP		;is lute about still?
	BEQ.S	11$
	MSGGO	NHAP
11$
        TEST_B  REMASTER(A4)
        BEQ.S   12$
        SCN	566                     ; uplifting but does not disintegrate
        BRA.S   13$
12$        
	DO	SETOUT
	SCN	235			; Sing stuff. - sounds uplifting!!

* make 'em drop everything

13$        
	MOVEQ	#0,D2			;target == invent.
	DO	GETRELN

	SF	D7			;flag, NE => they did drop something 
15$
	MOVE.W	(A1)+,D0		;give me a noun!
	BEQ.S	20$			;end of list
	BMI.S	15$			;MI => n/a
        CMP.W   #NLUTE,D0
        BEQ.S   15$                     ;dont drop the lute
        CMP.W   #NANTECUBE,D0
        BEQ.S   15$                     ;dont drop anticube
	GETINFO
	TEST_W	6(A0)			;ignore intrinsic/worn
	BNE.S	15$
	BTST	#0,5(A0)		;pseudo?
	BNE.S	15$			;NE=> yes, don't drop!
	ST	D7			;flag we dropped at least one
	MOVE.W	ROOM(A4),8(A0)		;we know locinfo = 0 so it's ok...
	BRA.S	15$			;next please..
20$
	TEST_B	D7			;did we drop anything?
	BEQ.S	25$			;EQ => nope, no message
	SCN	230			;about objects lacking spirit
25$
	ST	FlagInAir(A4)		;so setmode etc knows
	CLR.W	SEATED(A4)		;get off seat if on one
	MOVE.B	#2,TIMER9(A4)		;how long u get in air, 1 move

	MOVEQ	#ExitTable-ExitRooms-1,D7
	LEA	ExitRooms(PC),A2
30$
	SUBQ.W	#1,D7
	MOVE.W	(A2)+,D0
	BEQ.S	90$			;no rooms to modify
	CMP.W	ROOM(A4),D0		;outside the bank?
	BNE.S	30$
	BRA.S	ConnectRooms
70$:
	SCN	488
	DOGO	DEAD
	
90$:
	RET

	XREF	RoomDirStore
	XREF	RoomMod

ConnectRooms				;D0=current room on entry

	CLR.B	TIMERF(A4)		;leave the short queue if in it
	CLR.B	TIMER7(A4)		;shut up teller
	MOVE.B	0(A2,D7.W),D7		;get displacement from KbankDirs
	LEA	KbankDirs(PC),A1	;open up some directions
	ADD.L	D7,A1
	MOVE.W	D0,RoomMod(A4)
	DO	CALCNET			;A0 points to room data
	PUSH_L	A0
	LEA	RoomDirStore(A4),A2	;data store
	MOVEQ	#2,D0
5$
	MOVE.L	(A0)+,(A2)+		;store 12 bytes of room data
	DBRA	D0,5$
	PULL_L	A0
	CLR.L	D1
10$
	MOVE.B	(A1)+,D1		;direction to change
	TEST_B	(A1)			;endmarker?
	BEQ.S	90$
	MOVE.B	(A1)+,0(A0,D1)		;put new room number in
	BRA.S	10$
90$
	RET

KbankDirs
	DC.B	DIR_S,RNINBANK,0,0

InbankDirs
	DC.B	DIR_N,RNKBANK,DIR_S,RNMANOFFIS,0,0

ByMoatDirs
	DC.B	DIR_E,RNSCRUB1,DIR_W,RNSCRUB1,DIR_N,nway_wall,DIR_SE,RNSCRUB1
	DC.B	DIR_S,RNSCRUB1,DIR_SW,RNSCRUB1,0,0

ScrubDirs
	DC.B	DIR_N,RNPASSAGE3,DIR_NE,RNPASSAGE3,DIR_NW,RNPASSAGE3,0,0

BhseDirs
	DC.B	DIR_NE,RNBHSE,DIR_E,RNBHSE,0,0

Drm5Dirs
	DC.B	DIR_N,RNDRM5,DIR_NE,RNDRM5,0,0

ExitRooms
	DC.W	RNKBANK,RNINBANK,RNPASSAGE3,RNSCRUB1,RNBHSE,RNDRM5,0

ExitTable

	DC.B	KbankDirs-KbankDirs
	DC.B	InbankDirs-KbankDirs
	DC.B	ByMoatDirs-KbankDirs
	DC.B	ScrubDirs-KbankDirs
	DC.B	BhseDirs-KbankDirs
	DC.B	Drm5Dirs-KbankDirs

**********
*
* TeachBird - this is the bit that teaches the mynah bird to say things
*             it is FRAGILE, bird & mynah MUST be adjectives + about 99
*             other annoying/non-obvious things
*

	XREF	BirdKnowsHooray,BirdTextNo,BirdText1,BirdText2,BirdText3
	XREF	MynahSays

TeachBird

	CALL	BACK.WRD

	MOVE.L	A6,-(A7)
	DO	FNDWRD			;get to start of our words
	MOVE.L	(A7)+,A6
	CMP.B	#1,D7			;error from FNDWRD 1=> end of line
	BEQ	80$
10$
	MOVE.L	A6,A0
	LEA	BirdText1(A4),A1
	MOVE.B	BirdTextNo(A4),D0	;get which text to use next
	BEQ.S	30$
	LEA	BirdText2(A4),A1
	CMP.B	#1,D0
	BEQ.S	30$
	LEA	BirdText3(A4),A1
	ST	BirdTextNo(A4)		;make it -1
30$
	MOVE.L	A1,A3			;incase they swear
	ADDQ.B	#1,BirdTextNo(A4)	;so it's ok next time though
	MOVEQ	#17,D2			;counter
	
	BCLR	D0,BirdKnowsHooray(A4)	;make him forget this one
	MOVE.W	D0,-(A7)		;save which text we're at
50$
	PUSH_L	D2/A1/A3		;counter & pointer to stored text
	DO	GETWRD
	PULL_L	D2/A1/A3

* loose 'and' & 'bird' (extra tacky)

	CMP.W	#SECT.ADJ,D6
	BNE.S	51$
	CMP.W	#ADJ_mynah-4,D5		;not nice!!!
	BEQ.S	57$
	CMP.W	#ADJ_bird-4,D5		;not nice!!!
	BEQ.S	57$
51$
	CMP.W	#SECT.INSULT,D6		;swearing at the nice birdy?
	BEQ	SworeAtBird
	CMP.W	#SECT.SPEC,D6
	BNE.S	52$
	TEST_W	D5			;and?
	BEQ.S	57$
52$
	MOVE.L	A6,A0
53$
	CMP.B	#' ',-1(A0)		;find start of word
	BEQ.S	55$
	TEST_B	-(A0)
	BNE.S	53$
	ADDQ.W	#1,A0
55$
	MOVE.B	(A0)+,D1		;print word
	BEQ.S	56$
	CMP.B	#' ',D1
	BEQ.S	56$
*	DO	CHAROUT
	MOVE.B	D1,(A1)+
	SUBQ.W	#1,D2			;don't get overflow!
	BMI.S	60$
	BRA.S	55$
56$
*	MSG	SPACE
*	MOVE.B	#' ',(A1)+		;4/9/87 hack supercedes

* next 3 lines added 4/9/87 bt ken to fix hassle with BankedTreasure
* see also scenrw.asm for anti-fencepost hack

	MOVE.B	D1,(A1)+		;count spaces!
	SUBQ.W	#1,D2			;don't get overflow!
	BMI.S	60$
57$
	CMP.B	#2,D7
	BEQ.S	50$
	CMP.B	#1,D7
	BEQ.S	60$
	CMP.W	#SECT.NOUN,D6
	BNE.S	50$
	CMP.W	#NHOORAY-1,D5
	BNE.S	50$
* yes!! we got it!
	MOVE.W	(A7),D0
	BSET	D0,BirdKnowsHooray(A4)	;leave msg# which contains hooray
	BRA.S	50$
60$
	MOVE.W	(A7)+,D0
80$
	CLR.B	(A1)
	TEST_B	D2			;were we entirely within the buffer?
	BPL.S	85$
82$
	MOVE.B	-(A1),D1		;kill crap end of word at end of
	BEQ.S	85$			;buffer
	CMP.B	#' ',D1
	BNE.S	82$
	CLR.B	(A1)

85$

	SCORE	TEACHBIRD

	CALL	MynahSays
	BRA.S	ExitTeachBird

SworeAtBird

	MOVE.W	(A7)+,D0		;clean up stack (contains msg#)
	CLR.B	(A3)			;fod early part of message
	SCN	510			;bird glares at u severly

ExitTeachBird

* avoid simultaneous squawks by re-triggering timer

	MOVEQ	#4,D1			; Squawk interval...
	RND_D1                          ; is random...
	ADDQ.W	#3,D1			; but at least 3 moves.
	MOVE.B	D1,TIMERB(A4)		; so reset timer.

	POPSP
	SETNE
	RET


	XREF	SP.HOORAY

SayHooray

	CALL	SP.HOORAY
	POPSP
	SETNE
	RET
 
*--------------------------------
*
*  SP.SAY, CALLED BY P.SAY(2)
*          DOES STUFF LIKE
*          GURU, BETWEEN FITS..
*
* ACCTUAL STUFF GOES HERE
*
*--------------------------------
 
SP.SAY

	XREF	SETPAUSE

	MOVE.L	D5,-(A7)
	MOVEQ	#0,D5			;to stop SETPAUSE trying to pause a
					;random noun!!
	GETINFO
	CALL	SETPAUSE
	MOVE.L	(A7)+,D5
	RET

*--------------------------------
*	
* ROUTINES TO HANDLE SPECIAL
* QUESTIONS and commands

* lable used to calc address of these bits of code

PRSPDO

*--------------------------------
*
* Specials required by above tables can live down here..
*
*--------------------------------

	XREF	DoHelpMan

CanHelp

	BRA	DoHelpMan
*
* BorrowLute - tells you you want to buy it if u ask for it
*

BorrowLute

	SCN	498
	RET


    END
