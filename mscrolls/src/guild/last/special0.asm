*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALS0
*
*--------------------------------


	include	"equates.asm"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"roomequ.i"
	include	"macros.asm"
	include	"scoreref.asm"


	XDEF     SPECIALS,SP.ATTAC,SP.MELT,SP.WEAR,SP.UNWEA,SP.JUMP
	XDEF     SP.GIVE,SP.LEVER,SP.LOWER,SP.PUSH,SP.RUB,SP.TOUCH
	XDEF	SP.RUB2,SP.FEEL
	XDEF	SP.LIFT

	XDEF	ProcessSpecials,DecodeSpecials,ContextSpecials,ScanSpecials
	XDEF	SPECIALS_Before,RubFeet

	XREF	NOUNBUF2,NOUNBUF3,NPC.OK,SP.SCORE,V.FEED,P.TNSTOP
	XREF	SP.LIGHT,TIMER,FULLSTOP,CHKPAUSE,LINE,GETDATA,DO.SCORE
	XREF	TBL.ATTA,P.SCN,DEAD,PRTTXT,DO.SCN,DIED,GLOBAL.F
	XREF	SCRPNT,ROOM,CLOCK,SCORE_,PRTNUM,OldScore
	XREF	SETOUT,CARRIED,SUBJECT,P.TN,NearNPC
	XREF	VALID.NP,TIMER1,CHEAT,RND.D1,SETPRON
	XREF	OPEN,CLOSE,COUNTEXIT,SP.GO.N,TOPLOC,VERB,GameFinished
	XREF	FlagBeam,SP.Beam,DoHelpMan,SmearFish,LeaveMill
	XREF	TIMER7,P.TNSTOP
	XREF	TIMER10,ManagerMessage,TIMERB,TIMER13,MATCHBUF
	XREF	VLEN.GO2,VLST.GO2,FlagInAir,SP.BREAK,SEATED
        XREF    REMASTER

*
* SPECIALS - this is called by screen each move to let the specials suss
*		if they should do spontaneous things (as opposed to specials
*		triggered by verbs)
*


SPECIALS

      IFNE	YCHEAT
	TEST_W	CHEAT(A4)
	BNE.S	90$
      ENDC

	CALL    SP.LIGHT		;suss if the player can see
	CALL    TIMER			;do timers
	CALL	SP.Miller		;gum etc
	CALL	Weather			;hmmmmmm
	CALL	SP.Gravedigger
*	CALL	SP.BeeSting		;bees going to get you?
	CALL	SP.Flies		;Flies and spider dying!
	CALL	SP.Mynah		;opened the cage and let it out
	CALL	SP.GKeeper3		;say hello?
	CALL	SP.Disappear		; objects leaving a location.

      IFNE	YCHEAT
	CALL	SP.Inform
      ENDC
90$  
	RET


*
* SPECIALS_Before, called before deciding if we've changed room
*
* thus if stuff called here sets SCRPNT room desc happens
*

SPECIALS_Before

      IFNE	YCHEAT
	TEST_W	CHEAT(A4)
	BNE.S	90$
      ENDC
	
	MSG	LINE

	CALL	SP.Thief		;too long in boat/back to boat?
	CALL	SP.GKeeper2		;arrived at the gatehouse?
	CALL	SP.Underwater		; handle underwater stuff
	CALL	SP.GoOffice		;go to the manager's office
	CALL	SP.SCORE

	MSG	LINE
90$
	RET


**********************************************************************
*
* ScanSpecials
*
*
* Called from SCAN to enable people to give a more interesting message
* than "You find the xyzzy" D0 = noun number of discovered object
* returns EQ => stop scan printing
*         NE => let scan print "You find the xyzzy"
*
**********************************************************************

ScanSpecials

	SETNE
	RET

**********************************************************************
*
* ContextSpecials - called from CONTEXT before it tries too hard to
*                   suss out what to throw out.
*                    
* Context specials looks/hacks matchbuf, no 'real' params
* but should leave regs as found (bar D4, if successful == 1)
* returns EQ => i resolved it, don't do any more
*
**********************************************************************


Queue1Match	EQU	(SECT.NOUN*$10000)+NQUEUE1-1
Queue2Match	EQU	(SECT.NOUN*$10000)+NQUEUE2-1

ContextSpecials

* this tries to stop 'leave queue, which one hassles'
	
	CMP.L	#Queue1Match,MATCHBUF(A4)
	BNE.S	90$
	CMP.L	#Queue2Match,MATCHBUF+4(A4)
	BNE.S	90$
	CMP.W	#VNLEAVE,VERB(A4)		;only if it's leave
	BNE.S	90$

	TEST_B	TIMER7(A4)			;NE => queue2 (long)
	BEQ.S	10$
	MOVE.L	MATCHBUF+4(A4),MATCHBUF(A4)	;use queue2
10$
	MOVE.L	#-1,MATCHBUF+4(A4)		;terminate buffer
	MOVEQ	#1,D4				;no. of matched
	SETEQ
90$
	RET

********
*
* ProcessSpecials - called from PROCESS immediately before a verb routine
*
* primary use is to spot random references to given nouns. This may be
* triggered here rather than in ALL the verb routines/specials
*
* Returns NE if something is up
*
* WARNING: 
*
*    Only verbs going via process (not SINGV/say types) are hit.
*
*    THIS SHOULD NOT BE ABUSED - side effects intoduced here could
*                                be nasty!
*
* format of table:
*
*	DC.W	Indirect obj,Direct obj
*	DC.L	code
*
* Will call code if EITHER D.O. OR I.O. match


ProcessSpecials

	PUSH_L	D1/D2/A1/A2

	CALL	AllPrSpec	;non noun specific ones, ret EQ => nothing done
	BNE.S	90$

	LEA	PS_TAB(PC),A1
10$
	MOVE.L	(A1)+,D1
	BEQ.S	90$
	MOVE.L	(A1)+,A2
	CMP.W	D0,D1
	BEQ.S	20$
	SWAP	D1
	CMP.W	D5,D1
	BEQ.S	20$
	BRA.S	10$
20$
	MOVE.W	VERB(A4),D1	;so SP.xyzzy doesn't need to do VERB(A4) lots
	JSR	0(A4,A2.L)	;should return EQ => do verb
90$
	PULL_L	D1/D2/A1/A2
	RET


AllPrSpec

	SETEQ
	RET

	XREF	SP.HOORAY

PS_TAB

	DC.W	NHOORAY,NHOORAY
	DC.L	SP.HOORAY

	DC.W	-1,NCOUNTER2
	DC.L	SP.COUNTER2

	DC.W	NANTECUBE,NANTECUBE
	DC.L	SP.ANTECUBE

	DC.W	NTELLER,NTELLER
	DC.L	SP.TELLER

	DC.W	-1,NBGLASS
	DC.L	SP.COUNTER2

	DC.W	-1,NTILL2
	DC.L	SP.COUNTER2

	DC.W	NPEOPLE,NPEOPLE
	DC.L	SP.QUEUES

	DC.W	NQUEUE1,NQUEUE1
	DC.L	SP.QUEUES

	DC.W	NQUEUE2,NQUEUE2
	DC.L	SP.QUEUES

	DC.W	NWMACHINE,-1		; ALL but put
	DC.L	SP.MachinePut

	DC.W	-1,NWMACHINE		; ALL but l
	DC.L	SP.MachineLook

	DC.W	-1,NSUCCULENTS
	DC.L	SP.Succulents

	DC.W	NSUCCULENTS,-1
	DC.L	SP.Succulents1

	DC.L	0

SP.Succulents1

	CMP.W	#VNFILL,D1
	BEQ.S	SP.Succulents
	CMP.W	#NGUM,D0
	BNE.S	SuccExit
	CMP.W	#VNSTICK,D1
	BEQ.S	SP.Succulents
	CMP.W	#VNPUT,D1
	BNE.S	SuccExit

SP.Succulents


	CMP.W	#VNFIND,D1
	BEQ.S	90$
	CMP.W	#VNMAKELOCAL,D1
	BEQ.S	90$

	TEST_B	FlagSpider(A4)
	BNE.S	90$
	SCN	350
	SETNE
	RET
90$
SuccExit
	SETEQ
	RET

	XREF	MachineDied

SP.MachineLook

	CMP.W	#VNLOOK,D1		; Doing a look
	BEQ.S	99$
	CMP.W	#VNEXAMINE,D1		;verb?
	BEQ.S	99$
	SCN	79			; TOO RISKY
	SETNE
99$
	RET

SP.MachinePut

	CMP.W	#VNPUT,D1		;verb?
	BEQ.S	99$
	SCN	79
	SETNE
99$
	RET

	
*
* people/queues ignore you
*

SP.QUEUES

	CMP.W	#VNGET,D1
	BEQ.S	90$
	CMP.W	#VNJOIN,D1		;verb?
	BEQ.S	90$
	CMP.W	#VNEXAMINE,D1		;verb? look only verb < examine
	BLS.S	90$
	CMP.W	#VNASK,D1		;verb?
	BNE.S	60$
	CMP.W	#NQUEUE1,D0
	BEQ.S	70$
	CMP.W	#NQUEUE2,D0
	BEQ.S	80$
	CMP.W	#NPEOPLE,D0
	BNE.S	90$
	BRA.S	80$
60$
	LEA	VLST.GO2(A4),A1		;allow verbs equivalent to go
	MOVEQ	#VLEN.GO2,D2
70$
	CMP.B	(A1)+,D1
	DBEQ	D2,70$
	BEQ.S	90$
80$
	SCN	543		;queues/people ignore you
	SETNE
	RET
90$
	SETEQ
	RET

*
* SP.TELLER - you can only look,examine,ask,show,teller
*

SP.TELLER

	CMP.W	#VNEXAMINE,D1		;verb?
	BEQ.S	10$
	CMP.W	#VNASK,D1		;verb?
	BEQ.S	10$
	CMP.W	#VNSHOW,D1		;verb?
	BEQ.S	10$
	CMP.W	#VNLOOK,D1		;verb?
	BEQ.S	10$
	SCN	539			;bullet proof glass in the way
	SETNE
10$					;must get here EQ
	RET

SP.ANTECUBE
* all you can do with the ante cube is get it - and examine it.
	CMP.W	#VNEXAMINE,D1		;verb?	
	BEQ.S	10$
	CMP.W	#VNTAKE,D1		;verb?	
	BEQ.S	10$
	CMP.W	#VNGET,D1		;verb?
	BEQ.S	10$
	CMP.W	#VNPUT,D1		;verb?
	BEQ.S	10$
	SCN	530		;* Being a substance of a different univer..
	SETNE			; don't DO verb
10$					;must get here EQ
	RET

*
* SP.COUNTER2 - the queues won't let u at the counter or the bullet proof glass
*

	XREF	ExplodeBankFlag

SP.COUNTER2

	CMP.W	#RNINBANK,ROOM(A4)	;treat differently if not at till
	BNE.S	90$
	CMP.W	#VNEXAMINE,D1		;verb?
	BLS.S	90$
	CMP.W	#VNGO,D1		;verb?
	BEQ.S	80$
	SCN	544			;you can't reach the
	DO	P.TNSTOP
	BRA.S	85$
80$
	TEST_B	ExplodeBankFlag(A4)	;if queues are gone, no problem
	BNE.S	90$
	SCN	251		;dont want to get beaten up by others in queue
85$
	SETNE			;NE => miss verb
	RET
90$
	SETEQ			;EQ => still do verb
	RET

********
*
* DecodeSpecials - called from DECODE after INITCOM so we can fool
*			the parser about some of the things it thinks about
*
*	Prime use is to fod NearNPC so gatekeeper/miller
*	may be shouted at
*
*	THIS SHOULD NOT BE ABUSED - side effects intoduced here could
*					     be really nasty!!!!!!!!!!
*

DecodeSpecials

	PUSH_L	D0-D7/A0-A6		;better not try to exit this stuff
					;sideways!!

	CALL_S	SP.Miller.Decode	;so u can shout at him from RNFIELD
	CALL_S	SP.GKeeper.Decode	;ditto


	PULL_L	D0-D7/A0-A6
	RET

*--------------------------------
*
* SP.GKeeper.Decode
*


SP.GKeeper.Decode

	CMP.W	#RNSCRUB3,ROOM(A4)
	BNE.S	90$			;yes - force speech with gatekeeper
	MOVE.W	#NGATEKEEPER,NearNPC(A4) ;so conversation will get to gatekeeper
90$
	RET


*--------------------------------
*
* SP.Miller.Decode - called via DecodeSpecials
*


SP.Miller.Decode

	CMP.W	#RNFIELD,ROOM(A4)	;just outside mill?
	BNE.S	90$			;yes - force speech with miller
	MOVE.W	#NMILLER,NearNPC(A4)	;so conversation will get to miller
90$
	RET



*
* SP.GKeeper3 gatekeeper, won't let out with treasure
*
	XREF	FlagVoodoo,IsTreasure,IsTreasureNotCastle,GETRELN,NPC.OK

SP.GKeeper3

	CMP.W	#RNGATEHOUSE,ROOM(A4)
	BNE.S	99$
10$
	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	CALL	NPC.OK			;is he dead/unconsious?
	BNE.S	99$			;NE => yes!
	GETINFO
	CMP.L	#REL.MASK+NGATEHOUSE,6(A0)	;is he here?
	BNE.S	99$			;nope - no hassle then!

* shall he be nice (no swag) or throw u in the moat?

	MOVEQ	#0,D2			;set target object to invent
	DO	GETRELN

	MOVEQ	#0,D7			;temp flag NE => some non-castle t
11$
	MOVE.W	(A1)+,D0
	BEQ.S	15$			;end of list
	CALL	IsTreasure
	BEQ.S	11$
	CALL	IsTreasureNotCastle
	BEQ.S	12$
	ST	D7			;found some non-castle stuff
	BRA.S	11$

12$

* ok, takes bag from u + throws u in the moat

	SCN	13			;running off with the silver?
	SCN	14			;part time securicor man,+float off
	ST	DIED(A4)
	RET

15$
	TEST_B	D7			;was any treasure carried?
	BEQ.S	99$			;EQ => mope - no watch out mesg
	SCN	419
	BRA.S	99$
20$
	BSET	#3,FlagVoodoo(A4)	;so we only get these once
	BNE.S	99$
	SCN	12			;greetings from gatekeeper
99$
	RET

*--------------------------------
*
* SP.Gravedigger - he looks at u + considers how much ur custom is worth
*
*--------------------------------

SP.Gravedigger

	CMP.W	#RNCEMTR,ROOM(A4)
	BNE.S	90$
	MOVEQ	#20,D1
	RND_D1
	CMP.B	#1,D1
	BNE.S	90$
	SCN	495			;he looks at u a bit
90$
	RET


*
* RandThieves -   Randomly spews forth a scenario message at certain
*		  rooms describing the antics of some Master Thief who
*		  is following you around the game.
*

	XREF	FlagRoomDone,LIGHT

RandThieves

	TEST_B	LIGHT(A4)		;check for a light source.	
	BEQ.S	90$			;EQ => dark

	LEA	ThiefTableEnd(PC),A1	;end of msg,room# table
	MOVEQ	#ThiefTable_SIZE-1,D0
10$
	MOVE.L	-(A1),D1		;we go from end to start (downwards)
	CMP.W	ROOM(A4),D1
	DBEQ	D0,10$
	BNE.S	90$
	SWAP	D1
	BSET	D0,FlagRoomDone(A4)
	BEQ	P.SCN
90$
	RET


THIEFMAC	MACRO	*MESS,ROOMNO.
		DC.W	\1,RN\2
		ENDM


ThiefTable

	THIEFMAC	 545,YARD
	THIEFMAC	 548,CELLAR
	THIEFMAC	 549,BATHROOM
	THIEFMAC	 550,CAVE2
	THIEFMAC	 551,GDN2
	THIEFMAC	 552,SHOP2
	THIEFMAC	 553,TPWFL
	THIEFMAC	 554,BHSE

ThiefTableEnd

ThiefTable_SIZE	EQU	(ThiefTableEnd-ThiefTable)/4


*--------------------------------
*
* SP.Miller
*


	XREF	GumFlag

SP.Miller

	CMP.W	#RNMILL1,ROOM(A4)
	BNE.S	90$
	BSET	#1,GumFlag(A4)		;set got gum from miller
	BNE.S	90$			;NE => already done
	MOVE.W	#NGUM,D0		;gum starts off as OUT on invent
	GETINFO
	CALL	SETPRON
	BCLR	#7,6(A0)		;clear 'out' bit
90$
	RET


*--------------------------------
*
* SP.GKeeper2 - gatekeeper, moving u to dining room
*

	XREF	FlagVoodoo


SP.GKeeper2

	CMP.W	#RNGATEHOUSE,ROOM(A4)	;in the gatehouse?
	BNE.S	90$
*	BTST	#0,FlagVoodoo(A4)	;has gatekeeper been knobbled?
*	BNE.S	99$			;EQ => nope

	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	CALL	CHKPAUSE
	BNE.S	90$			;must be talking to him...
	CMP.L	#REL.MASK+NGATEHOUSE,6(A0)	;is he here too?
	BNE.S	99$

	BSET	#2,FlagVoodoo(A4)	;u get a spare move
	BEQ.S	99$
	SCN	16			;he leads u to the drawing room
	MOVE.W	#RNROOM1,ROOM(A4)	
	MOVE.W	#1,SCRPNT(A4)		;force room desc
90$
	BCLR	#2,FlagVoodoo(A4)
99$
	RET


*--------------------------------
*
* SP.BeeSting - bees going to nobble u?
*
*

*	XREF	TimeInGDN1

*SP.BeeSting
*
*	MOVE.W	#NHONEY,D0		;have u stolen the bee's honey?
*	GETINFO
*	CMP.L	#CONT.MASK+NHIVE,6(A0)	;still in the hive?
*	BEQ.S	90$			;yes - quick exit
*	DO	CARRIED			;carried?
*	BEQ.S	05$
*	DO	TOPLOC			;sneakilly possesed?
*	TEST_W	D0
*	BNE.S	90$
*05$
*
*      IFNE	YCHEAT
*
*	TEST_B	TimeInGDN1(A4)		;active (ie not died yet?)
*	BEQ.S	90$			;EQ=> nope
*	
*      ENDC
*	
*	SUBQ.B	#1,TimeInGDN1(A4)	;get two moves safe
*	BNE.S	10$
*	SCN	96			;rest of bees join in...
*	DOGO	DEAD
*10$
*	CMP.B	#1,TimeInGDN1(A4)	;half stung to death?
*	BNE.S	20$
*	SCN	95
*	BRA.S	90$
*20$
*	SCN	94
*90$
*	RET
*	
* this is remmed out by me - AJS
*--------------------------------
*
* SP.Thief - thief getting bored waiting for u to start
*		coming back to boat to deliver goods
*
*
	XREF	FlagLeftBoat,ThiefHere


SP.Thief

	CMP.W	#RNBOAT,ROOM(A4)	;in the boat?
	BNE.S	90$

	CMP.B	#-1,ThiefHere(A4)	; Is the thief around?
	BEQ.S	90$			; yes
	TEST_B	FlagLeftBoat(A4)	;if the player has left the boat
	BNE.S	SP.Thief2		;then he must have returned to get
					;score
	CMP.W	#7,CLOCK(A4)		;here too long?
	BNE.S	10$
	SCN	4			;sarcastic comment from thief
	RET
10$
	CMP.W	#10,CLOCK(A4)		;here lots too long?
	BNE.S	90$
	SCN	3			;thief throws u onto jetty
	MOVE.W	#RNJETTY,ROOM(A4)	;change room
	MOVE.W	#1,SCRPNT(A4)		;force screen to do description etc
90$
	RET

*
* SP.Thief2 - concerned with returns to the boat/end of game
*

	XREF	T_Carried
SP.Thief2

MAXSCORE	EQU	100		;to be finalised!!!!!
MAX_T		EQU	16

	IFNE	YCHEAT
	SF	FlagLeftBoat(A4)	;avoid multiple deaths
	ENDC

	SCN	7
	CALL	T_Carried		;get the number of t's
	CMP.W	#MAX_T/2,D1		; less than half.
	BPL.S	10$
	SCN	8			;yes..
	SCN	3			;thief throws u onto jetty
	MOVE.W	#RNJETTY,ROOM(A4)	;change room
	MOVE.W	#1,SCRPNT(A4)		;force screen to do description etc
	RET
10$
	CMP.W	#MAX_T,D1		; most
	BNE.S	20$
	SCN	10			;100% score!!!!
	BRA.S	30$
20$
	SCN	9			;between 50 and 100%
30$
	SCORE	WINNING			;1 point for winning
	ST	GameFinished(A4)	;let the game no you've one
	RET



*--------------------------------
*
* sp.attack - catch attempts to murder the population
*
* TBL.ATTA consists of 2 words/entry - noun#,msg#
* if the hi bit of the msg# is set they get to live 
*


SP.ATTAC

10$
	LEA     TBL.ATTA(A4),A1
15$
	CMP.W   (A1)+,D0		;find noun in table
	BEQ.S   20$			; found it!
	TEST_W  (A1)+			;keep track of position in table
	BNE.S   15$			;EQ=> hit end of table
	RET				;bye...
 
20$
	POPSP
	MOVE.W  (A1),D1			;GET MSG#
	BMI.S   30$   
	PEA     DEAD(PC)	     ;IF HIGH BIT CLR THEN RET VIA READ
30$
	AND.W   #$7FFF,D1
	DO	P.SCN

*
* Any tweeking of the table goes here - EG changing hi (death) bit of MSG#
* or the msg itself (like HJ in The Pawn, kills u second time)
*

	CMP.W	#NMILLER,D0
	BEQ.S	AttackMiller
	CMP.W	#NBIRD3,D0		;Mynah brd
	BEQ.S	FreeBird
	RET

*--------------------------------

AttackMiller

	MOVE.W	#RNFIELD,ROOM(A4)	;miller throws u out
	MOVE.W	#1,SCRPNT(A4)		;let screen know
	CALL	LeaveMill		;Break lute, etc!
	RET

*--------------------------------


*
* Happens if you attack the mynah bird.
* 

	XREF	TIMER4,TIMERB


FreeBird
	DO	SETOUT			;Fod the mynah bird
	CLR.B	TIMER4(A4)		;Stop it flying
	CLR.B	TIMERB(A4)		;Stop it squawking
	ST	FlagMynah(A4)		;This MIGHT just work!
	RET

*--------------------------------
*
* SP.JUMP (verb will NOT be VAULT)
*

SP.JUMP

	CMP.W	#NQUEUE1,D0
	BEQ.S	10$
	CMP.W	#NQUEUE2,D0
	BEQ.S	10$
	CMP.W	#NPEOPLE,D0
	BNE	SP.GO.N
10$
	SCN	132			;death from jumping queue
	POPSP
	DOGO	DEAD


*--------------------------------
   
SP.MELT
   	CMP.W	#NWAX,D0		;trying to melt the wax
	BNE.S	99$			;nope
	CMP.W	#NBEAM,D5		;with the beam?
	BNE.S	10$
	CMP.B	#1,FlagBeam(A4)		;is the mirror in the beam?
	BEQ	SP.Beam			;yeah kill the wax
	CMP.W	#NMIRROR,NOUNBUF3(A4)	;with the mirror?
	BEQ	SP.Beam
	CMP.W	#NMIRROR,NOUNBUF2+2(A4)	;with beam and mirror?
	BEQ	SP.Beam
	BRA.S	40$			;no go mate!
10$
	CMP.W	#NMIRROR,D5		;melt wax with mirror?
	BNE.S	99$
	CMP.B	#1,FlagBeam(A4)		;is the mirror in the beam?
	BEQ	SP.Beam			;yeah kill the wax
	CMP.W	#NBEAM,NOUNBUF3(A4)	;with the BEAM?
	BEQ	SP.Beam
	CMP.W	#NBEAM,NOUNBUF2+2(A4)	;with beam and mirror?
	BEQ	SP.Beam
40$
	SCN	372			;you need more components...
	POPSP				;no verbs Thank You
99$
	RET

*--------------------------------

SP.LEVER

	CMP.W	#NBARS,D0
	BEQ	SP.BREAK
	RET

*--------------------------------
  
SP.TOUCH
SP.WEAR
	

*--------------------------------
   
SP.UNWEA

	RET
*--------------------------------
   
*--------------------------------
* So you can feed things to macaw/bear using 'give x to bear'
*
* D5 = npc, D0 = object
*   

	XREF	TellerMessage,BankVisitCount

SP.GIVE
	TEST_B	FlagInAir(A4)	;in the air?
	BEQ.S	05$
	SCN	122		;not from up here
	BRA	80$
05$
	MOVE.B	11(A0),D1	
	AND.B	#$E0,D1		; Top 3 bits 0 = not edible
	BEQ.S	10$
08$
	POPSP
	BRA	V.FEED
10$:
	CMP.W	#NBEAR,D5	;giving things to bear = feeding him
	BEQ	08$
        CMP.W   #NMILLER,D5
        BEQ     GiveToMiller
	CMP.W	#NTELLER,D5	;showing the teller..
	BNE.S	30$
	CMP.W	#NKEY5,D0	;the deposit key?
	BNE.S	90$
	SCN	101		;the teller goes
	CMP.B	#2,BankVisitCount(A4)
	BNE.S	15$
	SCN	39		;the manager won't see you
	MOVE.W	#536,TellerMessage(A4)
	BRA.S	80$
15$
	ADDQ.B	#1,BankVisitCount(A4)
	CLR.B	TIMER7(A4)	;shut up teller
	MOVE.W	#NGUM,D0
	GETINFO
	CMP.L	#NKEYHOLE+$30000000,6(A0)
	BNE.S	20$
	MOVE.B	#1,TIMER13(A4)	;the gum delays the manager
	BRA.S	80$
20$
	CALL	GoOffice
	SCN	102		;after a few moments
	SCN	538		;the manager comes
	BRA.S	80$
30$
	CMP.W	#NGATEKEEPER,D5
	BNE.S	90$
	CALL	IsTreasure		;is it treasure?
	BNE.S	90$			;NE => nope, no worry
	CALL	IsTreasureNotCastle	;but is it from the castle?
	BEQ.S	90$			;EQ => nope, no death

* getting here means u showed/tried to give treasure from the castle to
* the gatekeeper - this is death...

	SCN	13			;running off with the silver?
	SCN	14			;part time securicor man,+float off
	ST	DIED(A4)
80$
	POPSP			;we've done all the work
90$
	RET 

*--------------------------------

        XREF MillerWineFlag
        
GiveToMiller
        TEST_B  REMASTER(A4)
        BEQ.S   90$                     ; old version requested
        CMP.W   #NBOTTLE1,D0            ; red wine bottle
        BNE.S   90$                     ; no
        
        ;; check wine is inside
        PUSH_L  D0/A0
        MOVE.W  #NWINE1,D0
        GETINFO
        CMP.L   #CONT.MASK+NBOTTLE1,6(A0) ; wine in the bottle?
        PULL_L  D0/A0
        BEQ.S   20$
        SCN     497                     ;rejects empty bottle
        BRA.S   80$
20$
70$
        ST      MillerWineFlag(A4)      ; miller has received the wine
        SCN     496                     ; miller accepts wine
        DO      SETOUT
        MOVE.W  D5,8(A0)        
	MOVE.B  #1,6(A0)                ; related to miller
*	BSET    #6,5(A0)                ; not set as given!
80$     POPSP
90$
        RET
 
 
*--------------------------------
 
SP.LOWER

	XREF	FlagDBlowered,SP.UNTIE

	CMP.W	#NLADDER2,D0
	BEQ	SP.UNTIE

	CMP.W	#NGATEKEEPER,SUBJECT(A4)
	BNE.S	20$

* telling gatekeeper to lower drawbridge?

	CMP.W	#NDRAWBRIDGE,D0
	BNE.S	90$

	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	CMP.L	#REL.MASK+NGATEHOUSE,6(A0)	;but is he in the gatehouse?
	BEQ.S	05$
	SCN	532
	BRA.S	80$
05$
	BCLR	#2,FlagVoodoo(A4)	;so he doesn't move u to dining room
	BCHG	#0,FlagDBlowered(A4)
	BNE.S	10$
	SCN	56			;creaky drawbridge
	SCN	59			;lowered
	DO	FULLSTOP
	MOVE.W	#NGUEST,D0		;guess he'll get their alone
	DO	SETOUT

	BRA.S	80$
10$
	SCN	314
	SCN	315

* The gatekeeper looks at you strangely, considers for 
* a moment and deciding that he misheard you 
* raises 

	MOVE.W	#NDRAWBRIDGE,D0

	DO	P.TNSTOP
	BRA.S	80$
20$
	CMP.W	#NCOUNTER,D0	;lift/open counter == flap
	BEQ.S	30$
	CMP.W	#NFLAP,D0	;lower flap = close flap
	BNE.S	90$
30$
	POPSP
	BRA	CLOSE
80$
	POPSP
90$
	RET
	
*--------------------------------

SP.LIFT

	CMP.W	#NGATEKEEPER,SUBJECT(A4)
	BNE.S	20$

* telling gatekeeper to raise drawbridge?

	CMP.W	#NDRAWBRIDGE,D0
	BNE.S	20$

	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	CMP.L	#REL.MASK+NGATEHOUSE,6(A0)	;but is he in the GATEHOUSE?
	BEQ.S	05$
	SCN	532
	BRA.S	80$
05$
	BCLR	#2,FlagVoodoo(A4)	;so he doesn't move u to dining room
	BCHG	#0,FlagDBlowered(A4)
	BEQ.S	10$			;Was an NE - Paul
	SCN	56			;creaky drawbridge
	SCN	58			;raised
	DO	FULLSTOP
	BRA.S	80$
10$

	SCN	314
	SCN	316			;lowers

* The gatekeeper looks at you strangely, considers for 
* a moment and deciding that he misheard you 
* lowers 

	MOVE.W	#NDRAWBRIDGE,D0
	DO	P.TNSTOP
	BRA.S	80$
20$
	CMP.W	#NCOUNTER,D0
	BEQ.S	30$
	CMP.W	#NFLAP,D0		;lift flap = open flap
	BNE.S	90$
30$
	POPSP
	BRA	OPEN

80$
	POPSP
90$
	RET
	
*--------------------------------

	XREF	DoBarsSpecial,WrenchPipe
	XREF	SP.PRESS2
 
SP.PUSH

	CMP.W	#NBUTTON,D0
	BEQ	SP.PRESS2		; I know this will always catch it
 
	CMP.W	#NPIPE,D0
	BEQ	WrenchPipe
	CMP.W	#NLEVER,D0
	BEQ.S	PushPullLever
	CMP.W	#NBARS,D0
	BEQ	DoBarsSpecial
	CMP.W	#NTRUNK,D0
	BNE.S	90$
	CALL	DoHelpMan
80$:
	POPSP
90$:
	RET

*
* PushPullLever - deal with people doing DIY drawbridge open close
*

	XDEF	PushPullLever

PushPullLever

	POPSP			;we're not going back to verb!
	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	DO	VALID.NP
	BNE.S	10$
	CALL	NPC.OK		;is he dead/unconsious?
	BNE.S	10$		;NE => he is, so u can use lever
	SCN	402		;gatekeeper wont let u 
	RET			;bye...

* ok, u get to fiddle with drawbridge

10$
	MOVEQ	#58,D1			;lowered message
	BCHG	#0,FlagDBlowered(A4)	;was it down (eq => yes)
	BNE.S	20$
	MOVEQ	#59,D1			;raised message#
20$
	SCN	56			;bit about the creaky drawbridge
	DOGO	P.SCN			;exit with raised/lowered message

*--------------------------------
 
	XREF	Lucky,Brokemirror
SP.RUB
* this happens before it has insisted on there being 4 parts
	CMP.W	#NHORSESHOE,D0		;rubbing horse shoe = lucky
	BNE.S 	99$
	TEST_B	Brokemirror(A4)		;Is the mirror broken?
	BEQ.S	80$			;=> Nope
	SCN	153			;Nothing happens
	BRA.S	95$
80$
	TEST_B	Lucky(A4)		;Already lucky?
	BEQ	90$			;=> nope
	SCN	31			;Don't push your luck!
	BRA.S	95$
90$
	ST	Lucky(A4)		;Makes you lucky
	SCN	507			;Horse shoe glows mirroring a fairy
95$
	POPSP
99$
	RET

	XREF	TIMER3,SuccLife,FlagSucc

SP.RUB2
* this happens after it has insisted on there being 4 parts
	CMP.W	#NFEET,D0		;are we trying to get succs on feet
	BNE.S	50$			;nope
	CMP.W	#NSUCCULENTS,D5		;with succulents?
	BNE.S	50$
* is the spider around?
	CALL	RubFeet
	BRA.S	80$
50$
	CMP.W	#NDFISH,D0		; Rub dead fish?
	BNE.S	90$
	CMP.W	#NPOISON,D5		; With poison
	BNE.S	90$
	CALL	SmearFish
80$
	POPSP				;return outside verb routine!
90$
	RET

*
* Rubbing feet with succulents
RubFeet
	TEST_B	FlagSpider(A4)		;NE=> spider dead
	BNE.S	5$
	SCN	350			;Can't get the succulents
	BRA.S	80$
5$
	ADDQ.B	#1,SuccLife(A4)
	CMP.B	#6,SuccLife(A4)		;you can only do this 6 times!
	BNE.S	10$
	SCN 	351			;the succulents disentegrate
	MOVE.W	#NSUCCULENTS,D0		;to get succ's into D0
	DO	SETOUT			;set the succulents 'out'
	BRA.S	80$			;and go away
10$
	TEST_B	FlagSucc(A4)		;have they already done it?
	BEQ.S	20$			;nope not in current use
	SCN	355
	BRA.S	30$
20$
	SCN	352			;as you rub succ's on feet they numb
	ST	FlagSucc(A4)		;to let the world know your feet r numb
30$
	MOVE.B	#7,TIMER3(A4)		;it only has a life of 7 moves!
80$:
	RET	


SP.FEEL
	CMP.W	#NFEET,D0		;are we trying to feel the feet?
	BNE.S	99$
	TEST_B	FlagSucc(A4)		;under the influence of succulents
	BEQ.S	99$			;nope they are normal
	SCN	354			;feet are numb
	POPSP
99$
	RET



******************************************************************
*
*	Weather - code to think about the weather
*
*	caller each move via SPECIALS
*
* The idea is this - 'TypeOfWeather' has 8 bits, each represents
* one sort of weather that's happening NOW (yes, so it can rain+shine at
* the same time!)
*
* each type of weather is related to a PSEUDO noun which is in all the rooms
* that it applies to, eg rain is in all outside rooms so is related to OUTSIDE
*
* if the 'weather' is active then it will be local, otherwise it is set out
* the pseudo is never out, so several types of weather (eq rain+bird) can
* use the same pseudo (which might have a LONG list of locations!)

	XREF	TypeOfWeather,WeatherTime

c_weathertype	SET	0

WEntry	MACRO	*code to start,code to end,prob of event,type
WT_\4		EQU	c_weathertype
c_weathertype	SET	c_weathertype+1
		DC.W	\3/8+1
		DC.W	N\4
		DC.W	\1-WeatherTable
		DC.W	\2-WeatherTable
		ENDM




* the prob. of event is the (approx) no. of move before an event is expected

WeatherTable

* raining?
	WEntry	StartRain,EndRain,100,WRAIN

* windy?
	WEntry	StartWind,EndWind,100,WWIND

* Cloudy?
	WEntry	StartCloud,EndCloud,100,WCLOUD

* Rat race call likely?
	WEntry	RatRaceCall,RatRaceCall,16,WRATRACE

* Dripping noises in caves
	WEntry	DripNoise,DripNoise,35,WDRIP

* fish leaping in moat
	WEntry	FishSplash,FishSplash,35,MOAT

* random thieves
	WEntry	RandThieves,RandThieves,16,THIEF	;noun irrel.

* spares (8 bits in TypeOfWeather today...)
	DC.L	0,0	


Weather

	ADDQ.B	#1,WeatherTime(A4)	;only consider weather every 8 moves
	AND.B	#7,WeatherTime(A4)
	BNE.S	90$			;NE => not time yet

	LEA	WeatherTable(PC),A1
	MOVE.L	A1,A2
	MOVEQ	#-1,D6			;weather type
10$
	ADDQ.W	#1,D6
	MOVE.W	(A1)+,D1		;get probability of an event
	BEQ.S	90$			;EQ=>finished
	RND_D1                          ;returns a BYTE!!!
	CMP.B	#1,D1			;this time?
	BEQ.S	20$			;EQ=> yes
	ADDQ.L	#6,A1
	BRA.S	10$

* now call relevant stuff

20$
	MOVE.W	(A1)+,D0		;get noun in question
	GETINFO
	MOVE.W	(A1)+,D1		;get offset to first event
	BCHG	D6,TypeOfWeather(A4)
	BEQ.S	30$			;EQ=> use first
	MOVE.W	(A1),D1			;get offset to second event
30$
	PUSH_L	D6/A1/A2
	JSR	0(A2,D1.W)		;do the event
	PULL_L	D6/A1/A2
	ADDQ.L	#2,A1
	BRA.S	10$
90$
	RET
	

*****
**
** Rain
**
**
****

StartRain
	
	BCLR	#7,6(A0)		;set rain 'in'
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	90$			;NE=> nope
	SCN	317
	
* It seems to have started to rain.^

90$
	RET

EndRain
	
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	10$			;NE=> nope
	SCN	318

* It appears to have stopped raining.^

10$
	BSET	#7,6(A0)		;set rain 'out'
90$
	RET


*****
**
** Wind
**
**
****

StartWind
	
	BCLR	#7,6(A0)		;set wind 'in'
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	90$			;NE=> nope
* Is it raining?
	BTST	#WT_WRAIN,TypeOfWeather(A4)
	BEQ.S	10$			;nope it is not raining...
	SCN	319

* The wind picks up, making the rain feel much colder.^

	BRA.S	90$
10$
	SCN	320

* A Light wind starts blowing.^

90$
	RET

EndWind
	
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	10$			;NE=> nope
	SCN	321

* The wind dies down.^

10$
	BSET	#7,6(A0)		;set rain 'out'
90$
	RET

*****
**
** Clouds - I wandered lonely
**
**
****

StartCloud
	
	BCLR	#7,6(A0)		;set clouds 'in'
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	90$			;NE=> nope
* Is it raining?
	BTST	#WT_WRAIN,TypeOfWeather(A4)
	BEQ.S	10$			;nope it is not raining...
	SCN	322

* Large clouds are forming, possibly the rain will turn into a storm.^

	BRA.S	90$
10$
	SCN	323

* Feather like clouds are drifting above you.^

90$
	RET

EndCloud
	
	DO	VALID.NP		;we in a room where it matters?
	BNE.S	10$			;NE=> nope
* is there rain around?
	BTST	#WT_WRAIN,TypeOfWeather(A4)
	BEQ.S	5$			;no rain.
	SCN	324

* Some of the clouds clear, allowing rainbows to form around you.^

	BRA.S	10$
5$
* there is no rain so...

	SCN	325

* The clouds drift away.^

10$
	BSET	#7,6(A0)		;set rain 'out'
90$ 
	RET



* RatRaceCall - If u'r in the castle somewhere u hear gatekeeper call

	XREF	FlagRaceDue,FlagGK1,FlagRRAnnounced

RatRaceCall

	TEST_B	FlagGK1(A4)		;been past gatekeeper?
	BEQ.S	90$
	TEST_B	FlagRaceDue(A4)		;been here before?
	BNE.S	90$			;yes, 
	BTST	#7,6(A0)		;but is it out?
	BNE.S	90$			;ok, not due yet
	DO	VALID.NP
	BNE.S	10$			;not going to hear it!
	SCN	19			;keeper shouts about rat race
10$

        MOVE.W  D0,-(A7)
        MOVEQ   #10,D0                  ; classic time to rat race
        TEST_B  REMASTER(A4)
        BEQ.S   15$
        CLR.W   D0                      ; disable timer
15$
        MOVE.B	D0,TIMER1(A4)		;count down to race
        MOVE.W  (A7)+,D0                ;restore
	ST	FlagRaceDue(A4)
	ST	FlagRRAnnounced(A4)
90$
	RET

* DripNoise - noisy dripping etc if in the caves

DripNoise

	DO	VALID.NP
	BNE.S	90$
	SCN	326

* Sounds of drips and splashes reach your ears.^

90$
	RET


* FishSplash - Lets player see fish in the moat

FishSplash

	DO	VALID.NP
	BNE.S	90$
	SCN	474		; Kersplosh

90$
	RET


	XREF	FlagSpider,LASTA0,NoCanReach

SP.Flies		

* if the jam jar is open inside the stable and contains jam
* the flies die!
* if the jam jar containing dead flies is dropped (open) inside
* the spiders lair the spider dies.

* i am changing this simply to be that if the jam is 'local' then
* the flies will swarm into it..

	MOVE.W	VERB(A4),-(A7)
	MOVE.W	#VNFEEL,VERB(A4)
	CMP.W	#RNSTABLE,ROOM(A4)		;are we in the stable?
	BNE.S	50$
	MOVE.W	#NJAM,D0			;is the jar local?
	DO	VALID.NP			;we'll find out
	BNE.S	99$				;no jam 
	CALL	NoCanReach			; is it reachable
	BNE.S	99$
	GETINFO					;A0 = data
* Wow now we are gonna kill dem flies!
	SCN	92		;The flies swarm into the pot
	MOVE.L	6(A0),A5
	MOVE.L	#$80000000,6(A0)	;NOT calling SETOUT because it is being 
					;replaced by another jam!!
; Note we MOVE the jam,so 'put jam in jar' parse properly

	MOVE.W	#NJAM2,D0
	GETINFO
	MOVE.L	A5,6(A0)	;Put Jam2 into the relevant place
	BRA.S	99$
50$

* have we got an open jam jar with flies on the floor in the Spider's lair?

	CMP.W	#RNGDN2,ROOM(A4)	;in spider's lair?
	BNE.S	99$		;nope
	TEST_B	FlagSpider(A4)	;is the spider already dead
	BNE.S	99$
	MOVE.W	#NJAM2,D0	
	DO	VALID.NP
	BNE.S	99$			; jam is not local...
	CALL	NoCanReach
	BNE.S	99$
	CMP.L	#RNGDN2,LASTA0(A4)	; is the last thing returned from valid
					; the garden (ie not carried?)
	BNE.S	99$
	SCORE	KILLSPIDER

	SCN	115				;Spider goes into jar!!
	MOVE.W	#NSPIDER,D0
	GETINFO
	BSET	#1,(A0)				;The spider is dead!
	MOVE.L	#$11000000+NJAM2,6(A0)		; Spider now in jar!
	ST	FlagSpider(A4)		;So the world knows the spider is dead
99$
	MOVE.W	(A7)+,VERB(A4)
	RET

* Handle drowning, dragged down, etc.
	XREF	UWClock
        XREF    CANTSAVE

SP.Underwater

	MOVE.W	ROOM(A4),D0
	MOVE.W	#RNWELL2,D1	; Set force move room in D1
	CMP.W	#RNWELL1,D0	; Well 0?
	BEQ.S	10$
	MOVE.W	#RNWELL3,D1	; Force to well 3
	CMP.W	#RNWELL2,D0	; .. if well 2
	BEQ.S	10$
	CMP.W	#RNWELL3,D0	; Well 3 forces to well 3!
	BEQ.S	10$
	CMP.W	#RNTUNNEL,D0	; Tunnel also forces to well 3, but aint used!
        BEQ.S   10$
        
	CLR.B   UWClock(A4)     ; Not underwater, so clear clock
        BCLR    #1,CANTSAVE(A4) ; can save again now
        BRA.S   90$
10$:
        BSET    #1,CANTSAVE(A4) ; no saving underwater
	TEST_B	UWClock(A4)	; Dont force-move on 1st turn in well...
	BEQ.S	70$
	MOVE.W	#NSTATUE,D0	; Carrying statue?
	DO	CARRIED
	BNE.S	70$		; No, just check drowning
	MOVE.W	D1,ROOM(A4)	; YES! Force the move
	MOVE.W	#1,SCRPNT(A4)
	SCN	98		; Statue drags u down...
70$:
        MOVE.B  #5,D0
        TEST_B  REMASTER(A4)
        BEQ.S   80$
        MOVE.B  #7,D0           ; more time before you drown!
80$        
	ADDQ.B	#1,UWClock(A4)	; Tick!
	CMP.B	UWClock(A4),D0	; Drowned yet ??
	BGE.S	90$		; No.. what a shame!
	SCN	99		; Drowneded!! 
	ST	DIED(A4)	; Yeah!
	CLR.W	SCRPNT(A4)	;no room desc if we're moving
	POPSP			; Frig the return address!
	CLR.B	UWClock(A4)     ; will cycle if dead
90$:
	RET


	XREF	FlagMynah

SP.Mynah

* have we opened the cage with the mynah bird in it in 
* a room with exits?

	CMP.B	#2,FlagMynah(A4)	;2 = it is local but not in cage
	BNE.S	99$
	CALL	COUNTEXIT		;d7 returns with status (already 
					;tested!
	BEQ.S	99$			;No exits in this room
	MOVE.W	#NBIRD3,D0
	DO	CARRIED
	BNE.S	20$
	SCN	420
20$:	
	SCN	116			;mynah bird flies off ne'er to be
					;seen again
*	ST	FlagMynah(A4)		;to tell the world!
*	DO	SETOUT			;get rid of it!
        BRA.S   BirdFlies
99$
	RET

BirdFlies        

        XREF    REMASTER
        TEST_B  REMASTER(A4)
        BEQ     FreeBird                ; remove bird from game
        GETINFO
        MOVE.L  #RNTJUNC2,6(A0)         ; put bird in junction2
        CLR.B   FlagMynah(A4)           ; reset state
        RET

        
	IFNE	YCHEAT
*
* Inform player if score changes
*

SP.Inform

	MOVE.W	SCORE_(A4),D0
	CMP.W	OldScore(A4),D0
	BEQ.S	90$
	PRINT	<'^Your score just went '>
	SUB.W	OldScore(A4),D0
	BLT.S	10$
	PRINT	<'up by '>
	BRA.S	20$
10$
	NEG.W	D0
	PRINT	<'down by '>
20$
	MOVE.W	D0,D6		
	CALL	PRTNUM
	PRINT	<' points.^'>
	MOVE.W	SCORE_(A4),OldScore(A4)
90$
	RET

	ENDC

*-----------------------------------------------

	XREF	GoOfficeFlag

SP.GoOffice

	TEST_B	GoOfficeFlag(A4)
	BNE.S	10$
	RET
10$
	MOVE.W	#NGUM,D0
	DO	SETOUT			;manager takes the gum
	CLR.B	GoOfficeFlag(A4)
	SCN	538			;go into the office
	SCN	130			;sorry,gum

GoOffice

	MOVE.W	#NMANTHING,D0
	GETINFO
	BCLR	#7,6(A0)		;the manthing is in
	MOVE.B	#2,TIMER10(A4)		;start up manager
	MOVE.W	#555,ManagerMessage(A4)
	MOVE.W	#RNMANOFFIS,ROOM(A4)
	CLR.W	SEATED(A4)		;u might be sitting on the counter
	MOVE.W	#1,SCRPNT(A4)
	MOVE.W	#NMANDOOR,D0
	GETINFO
	BCLR	#6,(A0)			;door unlocked
	MOVE.B	#6,TIMERB(A4)	;the mynah will squawk when you leave office
        TEST_B  REMASTER(A4)
        BEQ.S   90$
        ADD.B   #1,TIMERB(A4)   ; need an extra delay otherwise dont need gum
90$        
	RET

SP.Disappear
* this will cope with any rooms where if an object gets left lying
* around it will disappear from view.

	LEA	DisList(PC),A1		; list of rooms where objects dis.
	
10$
	MOVE.W	(A1)+,D2		; zero terminated.
	BEQ.S	99$
	CMP.W	ROOM(A4),D2		; in a room we are after?
	BNE.S	10$
	ST	GLOBAL.F(A4)		; in case we are in the dark.
	DO	GETRELN			; returns a list pointed to by a1.
	SF	GLOBAL.F(A4)		; no more GLOBAL.F stuff
20$
	MOVE.W	(A1)+,D0		
	BMI.S	20$			; hi bit set means n/a
	BEQ.S	99$			; ZERO terminated
	GETINFO
	BTST	#0,5(A0)		; is it a pseudo?
	BNE.S	20$			; yup - next one please
	TEST_W	12(A0)			; is it tied?
	BNE.S	20$			; YUP go away
	TEST_B	6(A0)			; must be just lying around.
	BNE.S	20$			; get the next one
	DO	SETOUT			; and now set it out.
	TEST_B	LIGHT(A4)		; if there is no light we print nothing
	BEQ.S	20$
	DO	P.TN			; print the xyzzy
	SCN	487			; disappears from view.
	BRA.S	20$			; and continue....
99$
	RET				
DisList:
	DC.W	RNWELL1,RNWELL2,RNWELL3,RNSHAFT1,0

	END
