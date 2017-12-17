******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALS2
*
*--------------------------------


	include	"equates.asm"
	include "adjequ.i"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"roomequ.i"
	include	"macros.asm"
	include	"scoreref.asm"


      XDEF	SP.RIDE,SP.GO.N,SP.TEAR,SP.READ,SP.KNOCK,SP.LEAN
      XDEF	SP.ASK,SP.ASK2,SP.GET,SP.MOVE.,SP.DIG,SP.BET,SP.STOP
      XDEF	SP.SWIM,SP.PLAY,SP.FLUSH,SP.PISS,SP.SLEEP,SP.SET
      XDEF	SP.WAITUNTIL,SP.WIND,SP.CHEW,SP.MOVE.BOOKS,SP.SIT
      XDEF	SP.MOVE.ABSTRACT,SP.SHAKE,SP.FISH,SP.ROLL
      XDEF	SP.HOORAY,SP.INVENT,SPA.INVENT,SP.TearBefore

	XREF	VERB,GO.N,ROOM,DO.SCN,SUBJECT,PRTTXT,DEAD,SETOUT,GET_CONT
	XREF	DIED,P.VERB,W.ATMOMT,YCANT,CONNECT3,TBL.MOVE,SP3.FLAG,GETDATA,DO.SCORE
	XREF	SEATED,SCRPNT,YNHANDLR,P.TN,ISARE,RND.D1,P.SCN,WHOOPS,W.NHAP
	XREF	SetValue,MessageFromTable,FULLSTOP,SETPRON
	XREF	FlagPiconwall,FGT_Bed2,FlagLute,FGT_System2,StartFlood
	XREF	FlagChampers,FGT_Junk2,FGT_Junk,NRT_COCONUTLOOK,STOPRON,FGT_Die1
	XREF	W.OUCH,VALID,TIMER9,VALID.NP,CARRIED,FlagChewing
	XREF	LastThrow,SpotBits,TIMER7,TIMERF,SP.HIT2,FIXCONT,SP.HIT
	XREF	SP.OPENBags,FlagInAir,SP.OPEN

SP.STOP

	CMP.W	#NFAKEMILLER,SUBJECT(A4)
	BEQ.S	10$
	CMP.W	#NMILLER,SUBJECT(A4)
	BNE.S	90$
10$
	CMP.W	#NMILL2,D0
	BEQ.S	StopMill
	CMP.W	#NVANES,D0
	BEQ.S	StopMill
90$
	RET

*
* StopMill - comes here if u do (say to) miller,stop mill [vanes]

StopMill

	XREF	ODT_MILL2,FlagMillStopped

	POPSP
	BSET	#0,FlagMillStopped(A4)
	BNE.S	10$			;NE=> already stopped
	BCLR	#7,ODT_MILL2(A4)	;set the mill to other description
	MOVE.W	#NMILL2,D0
	GETINFO
	BSET	#1,5(A0)		;no seeing miller through door.
	SCN	159			;voice in the distance shouts ok
	SCN	457
	RET
10$
	SCN	327

* The miller loudly curses those who cannot tell 
* whether a windmill has stopped or not!^

	RET
	

*******************************************************
*
* SP.BET
*
* this should catch the stuff about betting with the gatekeeper
*
* u can bet with the two freg piece (NPIECE) and the tem freg note (NNOTE)
*
* by the time the special is called
*
*       D0=money to bet
*       D5=what to bet on
*	D1=who to bet with always NPC (may be zero => default)
*
* If this RET's then either
*
*       'Xyzzy refuses your bet'
*       'Nobody will take your bet'
*
* depending on whether D1 is zero or an NPC
*

	XREF	Bet_Rat,Bet_Stake,Bet_RatList,FlagRatRace,TIMER1,FGT_Cheque

SP.BET

	BTST	#2,FlagRatRace(A4)	;is this a valid time to bet
	BEQ.S	90$

* check 'bookie'

	CMP.W	#NGATEKEEPER,D1		;betting with gatekeeper?
	BEQ.S	10$			;EQ=> yes
	TEST_W	D1			;betting with default?
	BNE.S	90$			;no, exit
10$

* check stake money

	MOVEQ	#3,D3			;use D3 to calc msg# offset for
					;desc of cheque (coin first)
	CMP.W	#NNOTE,D0		; either note (5)...
	BEQ.S	20$
	CMP.W	#NPIECE,D0		; Or coin (2)...???
	BNE.S	90$

	MOVEQ	#-1,D3			;starts at coin at 1-1
20$

* check runner

	LEA	Bet_RatList(A4),A0
30$
	MOVE.W	(A0)+,D2
	BEQ.S	90$			;not in list - no bet
	ADDQ.W	#1,D3			;next msg along (must be word)
	CMP.W	D2,D5			;our runner?
	BNE.S	30$

* Getting here means a valid bet

	MOVE.W	D5,Bet_Rat(A4)
	MOVE.W	D0,Bet_Stake(A4)
	DO	SETOUT			;fod from game

	ADD.W	D3,FGT_Cheque(A4)	;tweek msg#
	LEA	ChequeValues(PC),A0	;get new cheque value

	MOVE.W	#NCHEQUE,D5             ; setvalue takes d5
	MOVEQ	#0,D1			;fod hi bits
	MOVE.B	0(A0,D3.W),D1		;get value
	CALL	SetValue		;sets value of D0 to be D1.W

* Message about him taking stake comes from TIMER code.

	CLR.L	YNHANDLR(A4)		;so a spurious yes/no doesn't look odd
	BSET	#7,FlagRatRace(A4)	;fod have betted flag
	MOVE.B	#1,TIMER1(A4)

	SCORE	WINRATRACE

	POPSP

90$
	RET

ChequeValues

	DC.B	4,6,12,22,10,15,30,55

*******************************************************

SP.DIG

* see if the grave digger will object

	CMP.W	#RNCEMTR,ROOM(A4)	;where are we?
	BNE.S	10$
	SCN	17			;trying to dig in graveyard 
					;produces industrial action
	POPSP
10$:
	CMP.W	#NSAND,D0		; Looking for wetsuit boots?
	BEQ.S	20$
	CMP.W	#NBANK,D0
	BNE.S	90$
20$:
	MOVE.W	#NBOOTS,D0
	GETINFO
	BTST	#1,6(A0)		; Is it still hidden ?
	BEQ.S	80$			; ... jump out if already found
	MOVE.L	#RNBANK,6(A0)		; Simply in sandy bank room
	CALL	SETPRON
	SCN	386			; You find some rubber boots!
	BRA.S	85$
80$:
	SCN	387			; You find nothing else
85$:
	POPSP
90$:
	RET
	

*******************************************************
 
SP.RIDE

* various frigs cos JUMP ON maps to ride!! And setmode looks at VERB!

	CMP.W	#NBOAT2,D0	;hmmm - trying to get into the boat?
	BEQ.S	10$
	CMP.W	#NJETTY2,D0	;or out of it...
	BNE.S	20$
10$
	MOVE.W	#VNJUMP,VERB(A4)
	POPSP			;fod return to V.RIDE
	BRA	GO.N		;catch specials via SETMODE/MOVE.TAB
20$
	RET

*******************************************************
*
* SP.PLAY - piano/organ/billiards/cards, and LUTE!
*
	XREF	SP.SING

SP.PLAY

	LEA	PlayCardList(PC),A1
10$
	MOVE.W	(A1)+,D1
	BEQ.S	20$
	CMP.W	D1,D0		;is it a card?
	BNE.S	10$
	SCN	36		;You don't have sufficient cards to play
	ST	WHOOPS(A4)	;stop at first one - else play cards looks silly
	BRA.S	80$
20$
	CMP.W	#NLUTE,D0
	BNE.S	40$
	CMP.W	#RNWELL1,ROOM(A4);no playing underwater
	BLT.S	21$
	CMP.W	#RNTUNNEL,ROOM(A4)
	BGT.S	21$
	SCN	204
	BRA.S	80$
21$
	BTST	#0,FlagLute(A4)	;is lute broken?
	BNE.S	25$
	BSET	#3,FlagLute(A4)	;so we can tell if u sing!
	BTST	#4,FlagLute(A4)	;have we been singing of late?
	BEQ.S	22$
	POPSP
	BRA	SP.SING		;yes, code is at SP.SING to cope
22$
	MOVE.B	#3,TIMER9(A4)	; For two moves, you can sing now!
	DO	P.TN
	SCN	388		;nice noise...
	BRA.S	80$
25$
	DO	P.TN
	SCN	389		;broken, so awful racket
	BRA.S	80$

*
* must consider 'play red ball into top pocket....'
*

40$:
	LEA	PlayTable(PC),A1
	CALL	MessageFromTable
	BEQ.S	90$
80$
	POPSP
90$
	RET

PlayTable:

	MFT_Scen	PIANO,157
	MFT_Scen	ORGAN,158
	MFT_Scen	BILLIARDS,329
	MFT_End

PlayCardList

	DC.W	NCARD1
	DC.W	NCARD2
	DC.W	NCARD3
	DC.W	NCARD4
	DC.W	NCARD5
	DC.W	0





*******************************************************
*
* SP.SWIM - in lake? in moat?
*

SP.SWIM

        CMP.W   #NMOAT,D0
        BEQ.S   10$
	CMP.W	#NLAKE,D0	;in the lake?
	BNE.S	90$
10$        
	SCN	121		;may be modified for pike/knees joke
	POPSP
	DOGO	DEAD
90$
	RET


*******************************************************
*
* SP.FLUSH - toilet?
*

SP.FLUSH

	CMP.W	#NTOILET,D0
	BNE.S	90$
*
* When flushed, the toilet is emptied+msg about 'water'
*
	CMP.W	SEATED(A4),D0
	BNE.S	10$
	SCN	382		; Not whilst seated,please!
	BRA.S	80$
10$
	GETINFO
	CLR.B	D7
	BCLR	#5,(A0)		;toilet not closed so GET_CONT works
	BNE	20$
	SCN	73		; Water splashes you...
	BRA.S	30$
20$				; Lid closed...
	ST.B	D7		;indicate toilet closed
	SCN	381		; Gurgles,etc!
30$
	DO	GET_CONT	; returns A1 points at list of contents
	TEST_B	D7		;was toilet closed
	BEQ.S	40$
	BSET	#5,(A0)		;close the loo
40$
	MOVE.W	(A1)+,D0	; get items
	BEQ.S	80$		; End of list?
	BMI.S	40$		;only items in toilet!
	GETINFO
	BSET	#5,3(A0)	; Make it wet anyway.
	MOVE.B	2(A0),D1
	ANDI.B	#$F0,D1		; Just size bits
	CMPI.B	#$30,D1
	BHS.S	40$		; Too big to flush!
	DO	SETOUT		; fod item+fix all tied/contained hassles
	BRA.S	40$
80$
	POPSP
	RET
90$
	RET


*******************************************************
*
* SP.PISS
*

SP.PISS

	CMP.W	#NPOT,D0	;china chamber pot?
	BNE.S	10$
	SCN	330
* It would be a crying shame to soil such a piece as this!^
	BRA.S	80$
10$
	CMP.W	#NTOILET,D0
	BNE.S	90$
	BTST	#4,(A0)		;is it open?
	BNE.S	20$		;NE => yes
	SCN	331
* It would be rather embarrasing (not to say messy) to 
	MOVE.W	VERB(A4),D1
	DO	P.VERB
	SCN	332
*  in a closed toilet!^
	BRA.S	80$
20$
	MOVE.W	#NJEANS,D0
	GETINFO
	CMP.L	#$04000000,6(A0)	;worn?
	BNE.S	30$
	SCN	333
* Perhaps this might be more easily performed after having 
* removed any obstructing pieces of clothing?^
	BRA.S	80$
30$
	SCN	334
* Oddly, the urge seems to have subsided now!^

80$
	POPSP
90$
	RET
 
*******************************************************
*
* SP.SLEEP
*

SP.SLEEP

SP.LEAN
SP.KNOCK
SP.WIND
SP.INVENT
SP.WAITUNTIL
*******************************************************
*
* SP.SET
*
SP.SET
	RET

*******************************************************
*
* SP.ROLL
*
*

SPOTMSG1	EQU	124	; Message about 1 spot.
ONEMSG		EQU	459

SP.ROLL
	CLR.W	D2
	CMP.W	#NDIE1,D0	;	 Red?
	BEQ.S	10$
	ADDQ.W	#4,D2
	CMP.W	#NDIE2,D0	;	 Green?
	BEQ.S	10$
	ADDQ.W	#4,D2
	CMP.W	#NDIE3,D0	;	 Blue?
	BEQ.S	10$
	ADDQ.W	#4,D2
	CMP.W	#NDIE4,D0	;	 Yellow?
	BNE.S	20$
10$:
	MOVE.W	D0,STOPRON(A4)	;
	CALL	LadyLuck	; Do they want some luck?
	BNE.S	15$		; NE=> they did and they have
	MOVEQ	#6,D1
	RND_D1
15$
	SCN	431		; You roll the die. When it comes to rest
				; each side now has
	EXT.W	D1
	ADD.W	#SPOTMSG1,D1
	MOVE.W	D1,-(A7)
	DO	P.SCN
	LEA	FGT_Die1(A4),A0
	MOVE.W	(A7)+,D1
	BSET	#15,D1		; Set top bit to keep 1st glance.
	MOVE.W	D1,0(A0,D2)	; Set up spots message
	BRA.S	80$

20$:
	CMP.W	#NDIE5,D0	; Jewelled die ?
	BNE	SP.HIT2		;to deal with roll balls
	CALL	LadyLuck	;Feeling lucky?
	BNE.S	22$ 
	MOVEQ	#6,D1
	RND_D1
22$
	MOVE.W	D1,D2
	BTST	D1,SpotBits(A4)	; Is that face clear?
	BNE.S	25$
	SCN	465		; 'You throw a '
	ADD.W	#ONEMSG,D1
	DO	P.SCN
	BRA.S	30$
25$:
	SCN	466		; 'blank'
30$:
	MOVE.W	D2,LastThrow(A4)
	DO	FULLSTOP

80$:
	POPSP
	RET

*******************************************************
	XREF	Lucky,INSIST

LadyL	EQU	503
LadyL1	EQU	504
LadyL2	EQU	505
LadyL3	EQU	506

* Are we lucky tonight.....

        XREF    REMASTER
        
LadyLuck

	TEST_B	Lucky(A4)		;have they rubbed the horseshoe?
	BEQ.S	99$			;nope
	LEA	MatchNums(PC),A1	;the list of numbers
	LEA	PrintNums(PC),A0	;What is to be printed
	SCN	LadyL			;She appears and asks you..
	MOVEQ	#3,D0			;You only have three chances.
	CALL	INSIST			;Please give me one of the above
	CMP.B	#-1,D0			;-1 => timed out
	BNE.S	10$
	SCN	LadyL2			;She gives up in a huff
        TEST_B  REMASTER(A4)
        BNE.S   05$
	CLR.B	Lucky(A4)		;And your luck goes off
05$        
	SETEQ				;coz it don't work
	BRA.S	99$			;and go away
10$
	SCN	LadyL3			;She disappears in a flash
	SETNE				;NE => coz we have one
99$
	RET

MatchNums:
	DC.W	SECT.ONE,0
	DC.W	SECT.ADJ,ADJ_two-4
	DC.W	SECT.ADJ,ADJ_three-4
	DC.W	SECT.ADJ,ADJ_four-4
	DC.W	SECT.ADJ,ADJ_five-4
	DC.W	SECT.ADJ,ADJ_six-4
	DC.L	-1
PrintNums:
	DC.W	$C000+LadyL1
	DC.W	0
 
 	XREF	GoQueue

SP.GO.N

	CMP.W	#NFINBANK,D0		;going to the bank interior?
	BNE.S	01$
	CLR.B	TIMERF(A4)		;in case we're leaving the long queue
	CLR.B	TIMER7(A4)		;in case we're leaving the short queue
	BRA.S	04$
01$
	CMP.W	#NQUEUE1,D0		;'get in queue1'?
	BEQ.S	02$
	CMP.W	#NQUEUE2,D0		;'get in queue2'?
02$
	BEQ	GoQueue
03$
	CMP.W	#NKBANK,D0		;outside the bank?
	BNE.S	05$
	CMP.W	#RNKBANK,ROOM(A4)	;entering,going there?
	BNE.S	05$
	MOVE.W	#NOAKDOOR,D0
	GETINFO
	BTST	#4,(A0)			;is the door open
	BNE.S	04$
	SCN	531			;there is a door in the way
	BRA	80$
04$
	MOVE.W	#RNINBANK,D0		;then go in_the_bank
	BRA.S	40$
05$
	CMP.W	#NCASKET,D0
	BNE.S	20$
	BTST	#4,(A0)			;open?
	BNE.S	10$
	SCN	335 			; Surely you should open it first?^
	BRA	80$
10$
	SCN	142			;buried alive
	ST	DIED(A4)		;bye...
	BRA	80$
20$
	CMP.W	#NSTREAM,D0
	BNE.S	22$
	MOVE.W	#NRVR,D0		;go stream = go In The Stream
	BRA.S	24$
22$
	CMP.W	#NCASTLE,D0
	BNE.S	25$
	CMP.W	#RNSCRUB3,ROOM(A4)
	BNE.S	25$
	MOVE.W	#NDRAWBRIDGE,D0
24$
	GETINFO
	BRA.S	90$
25$
	CMP.W	#NCOUNTER,D0
	BNE.S	50$
	CMP.W	#VNCLIMB,VERB(A4)	;climb/jump allowed
	BEQ.S	30$
	CMP.W	#VNJUMP,VERB(A4)
	BNE.S	90$
30$
	MOVE.W	#RNSHOP2,D0
	CMP.W	ROOM(A4),D0
	BNE.S	40$
	MOVE.W	#RNSHOP3,D0
40$
	MOVE.W	D0,ROOM(A4)
	MOVE.W	#1,SCRPNT(A4)
	BRA.S	80$

50$
	CMP.W	#NHOLE,D0	;Hole behind painting
	BNE.S	60$
	SCN	376		;Death!
	ST	DIED(A4)
	BRA.S	80$

* handle rainbow room pseudos
60$
	LEA	RainbowList(PC),A1
65$
	MOVE.W	(A1)+,D1		;get a noun#
	BEQ.S	70$			;end of list
	CMP.W	D0,D1			;ours?
	BNE.S	65$			;next
	SCN	426			;yes! u die!!
	ST	DIED(A4)
	BRA.S	80$

70$:
	CMP.W	#NCAGE3,D0		; Bears cage?
	BNE.S	90$
	BTST	#4,(A0)			; open yet ??
	BEQ.S	90$			; Yes, so let them open it
	SCN	479

80$
	POPSP
90$
	RET

* Rainbow list - list of pseudo squares that kill when u step on them

RainbowList

	DC.W	NWHITESQUAREP
	DC.W	NBLACKSQUAREP
	DC.W	NREDSQUAREP
	DC.W	NORANGESQUAREP
	DC.W	NYELLOWSQUAREP
	DC.W	NGREENSQUAREP
	DC.W	NBLUESQUAREP
	DC.W	NINDIGOSQUAREP
	DC.W	NVIOLETSQUAREP

	DC.W	0			;term
	

SP.TEAR

	CMP.W	#NBAG2,D0		;tearing the plastic bag?
	BEQ	SP.OPEN
	RET

SP.TearBefore

	CMP.W	#NSACHET,D0
	BEQ.S	TearSachet
	CMP.W	#NRUBBAG,D0
	BEQ	SP.OPENBags
	RET

	XDEF	TearSachet
	XREF	SP_MakeGlue

TearSachet
 
	POPSP
	MOVE.L	6(A0),D1
	MOVE.L	#$80000001,6(A0)		;* Making this out
	MOVE.W	#NPOWDER,D0
	GETINFO
	MOVE.L	D1,6(A0)
	SCN	336
* ^There is a fizzing noise as the substance inside the sachet
*  reacts with the air, leaving a powdery residue.^
	CALL	SP_MakeGlue		;last ingredient?
90$
	RET
	
 
*--------------------------------
*
* SP.READ
*
 
SP.READ

	CMP.W	#NBOOK1,D0
	BEQ.S	SP.ReadBooks
	RET

	XREF	BooksFlags

SP.ReadBooks

	SCN	279			;you take a random book, it is...
10$
	MOVEQ	#25,D1			;25 books to choose from
	RND_D1
	MOVE.L	BooksFlags(A4),D7	;get bits about read books
15$
	BSET	D1,D7
	BEQ.S	90$
20$
	SUBQ.W	#1,D1
	BMI.S	30$			;ran out of books, start from begining
	BTST	D1,D7
	BEQ.S	15$			;found one that will be ok, set & use it
	BRA.S	20$

30$
	CLR.L	BooksFlags(A4)		;start again!
	BRA.S	10$


90$
	MOVE.L	D7,BooksFlags(A4)	;save new flags
	ADD.W	#281,D1			;get msg # (rnd produces 0-(n-1))
	DO	P.SCN
	SCN	280			;and u put it back

	POPSP

	RET


SP.ASK

	CMP.W	#NQUEUE1,D0
	BEQ.S	10$
	CMP.W	#NQUEUE2,D0
	BEQ.S	10$
	CMP.W	#NPEOPLE,D0
	BNE.S	20$
10$
	SCN	543			;people and queues ignore questions
	BRA.S	80$
20$
	CMP.W	#NTELLER,D0
	BNE.S	90$
	SCN	43			;fill in the form please
	MOVE.B	#2,TIMER7(A4)		;asking teller things delays her
80$
	POPSP
90$
	RET

	XREF	QuoteFlag
 
SP.ASK2
	CMP.W	#NGRAVEDIGGER,D5		;noun asked about is in d0
	BNE.S	10$
	BSET	#0,QuoteFlag(A4)		;don't put quotes around it
10$
	RET                  ;LEAVE IT IN!!!!


*--------------------------------
*
*  SRC.ADV.SPECIALS3
*
*--------------------------------
 
SP.MOVE.

	XREF	SP.SCORE.NOUN,DoBarsSpecial

	CALL	SP.SCORE.NOUN		;get any points avail


	CMP.W	#NBARS,D0
	BEQ	DoBarsSpecial

	CMP.W	#NPIPE,D0
	BEQ	WrenchPipe

	ST      SP3.FLAG(A4)        ;FLAG TO SUPPRESS SCAN'S MESSAGES
	PUSH_L  D0/D1/D5/A0/A5/A6

	LEA     TBL.MOVE(A4),A6
10$
	CMP.W   (A6)+,D0
	BEQ.S   30$
20$
	TEST_L  (A6)+
	BNE.S   10$
	
	SETNE
	BRA.S   40$	            ;NOTHING REVEALED
 
30$    
	BSET    #7,(A6)             ;TEST TO SEE IF ALREADY DONE+SET FLAG
	BNE.S   20$
      
	MOVE.L  (A6),D7
	AND.L   #$7FFFFFFF,D7           ;MASK OUT FLAG
	JSR     0(A4,D7.L)          ;GOTO SPECIAL (A6 INTACT)
40$:
	PULL_L  D0/D1/D5/A0/A5/A6
	BNE.S	SP3.RET			; Zero set = special DONE!
	POPSP
SP3.RET
	CLR.W   SP3.FLAG(A4)
	RET

*********************************************************

	XDEF	SP.MOVE.STATUE

* DON'T PUT A POP IN THIS SPECIAL AS IT'S CALLED FROM SP.GET TOO.
* Reason is to stop 'you have got the statue' followed by SCN mess
* saying it falls over.
* Paul.

SP.MOVE.STATUE

	CLR.L	6(A0)			;put statue on inv

	SCN	81			;msg about it crashing down
	MOVE.W	#1,SCRPNT(A4)
	MOVE.W	#RNWELL1,ROOM(A4)

	Connect	TEMPLE,WELL1,D

*	MOVE.W  #RNTEMPLE,D0
*	MOVE.W  #RNWELL1,D2
*	MOVEQ   #DIR_D,D1
*	CALL    CONNECT2

	SETEQ				;something happened
	RET
	


*********************************************************
 
	XDEF	SP.MOVE.CHEST

	XREF	ODT_SKELETON

SP.MOVE.CHEST

	BCLR	#7,ODT_SKELETON(A4)	; Other description
	SETNE
	RET

*--------------------------------
 
	XDEF	SP.MOVE.JUNK

SP.MOVE.JUNK

	SCORE	MOVEJUNK		;then we must be in the junk room

	MOVE.W	#NROOM2,FGT_Junk+2(A4)	; First glance for junk room

	Connect	ROOM2,PASSAGE3,S

*	MOVE.W  #RNROOM2,D0
*	MOVE.W  #RNPASSAGE3,D2
*	MOVEQ   #DIR_S,D1
*	CALL    CONNECT2
 
	SCN     152 			;moveing junk reveals a passage south
	SETEQ				; Indicate special done
	RET
 
*--------------------------------
 
 
 
*--------------------------------
*

SP.MOVE.BOOKS

	PEA	SP.MBOOKS2(PC)
	CALL	SP.ReadBooks		;always pops

SP.MBOOKS2:

	SETEQ
	RET
*--------------------------------
 
	XREF	FlagBear,SP.CATCH,ODT_SKELETON
        XREF    MynahStart
 
SP.GET

	TEST_B	FlagInAir(A4)		;floating?
	BEQ.S	01$
	SCN	122			;not from up here you don't
	BRA	80$
01$
	CMP.W	#NGEM,D0		; gettting (catching) gem?
	BEQ	SP.CATCH
	CMP.W	#NSTATUE,D0		;Statue?
	BNE.S	02$
	CMP.W	#RNTEMPLE,ROOM(A4)	;In the temple?
	BNE.S	02$
	CALL	SP.MOVE.STATUE		;Treat as move etc
	BRA	80$
02$
	CMP.W	#RNFOURROOM,ROOM(A4)	; in the four room?
	BEQ	GetCubes		; get them
	CMP.W	#NSKELETON,D0
	BNE.S	10$
	BCLR	#7,ODT_SKELETON(A4)
	BRA	90$
10$
	CMP.W	#NPIPE,D0		; pipe in drainage system?
	BEQ	WrenchPipe
	CMP.W	#NMINERAL,D0		; Getting the mineral with the pick?
	BNE.S	11$
	CMP.W	#NPICK,D5
	BEQ	SP.HIT
11$
	CMP.W	#NGUM,D0		; Gum from mouth?
	BNE.S	15$
	CMP.B	#8,6(A0)		; Intrinsic (Inmouth?)
	BNE.S	15$
	CLR.L	6(A0)			; On inventory
	SCN	475
	BRA	80$
15$:
	CMP.W	#NBOOK1,D0		;getting a book in the library?
	BEQ	SP.ReadBooks
 
	IFNE	0		;this is now N/A cos of new transp.  containers

	CMP.L	#$20000000+NCAGE3,6(A0)	;still in cage?
	BNE.S	30$
	TEST_B	FlagBear(A4)		;bear passive?
	BNE.S	20$			;NE=> yes
 
	SCN	337
* The bear starts towards your arm, which you obviously 
* remove very quickly!
	BRA.S	80$
20$
	PUSH_L	D0/A0			;lets suss if the cage is open
	MOVE.W	#NCAGE3,D0
	GETINFO
	BTST	#4,(A0)			;open?
	PULL_L	D0/A0
	BNE.S	30$
	DO	P.TN
	DO	ISARE
	SCN	338
* far inside the cage, beyond your reach.^
	BRA.S	80$

	ENDC

30$
	CMP.L   #NTOILET+CONT.MASK,6(A0) ;been put in the toilet?
	BNE.S   40$
	BSET	#5,3(A0)		 ; Yes, make it wet!
* ... and fall through
40$

*	CMP.L	#NBAG2+CONT.MASK,6(A0)	;in the plastic bag?
*	BNE.S	50$
*	SCN	339
* Just because the bag is transparent doesn't mean 
* that you don't need to open it!^
*
*	BRA.S	80$
50$
	CMP.W	#NSHARDS,D0
	BNE.S	60$
	SCN	340
* Picking up the shards would be the last thing your hands 
* could be used for!^
	BRA.S	80$

60$
        CMP.W   #NBIRD3,D0      ; mynah bird
        BNE.S   70$
        CALL    MynahStart      ; kick off bird fly timer
70$
	CALL_S	SP.MOVE.ABSTRACT
	BRA.S	90$
80$
	POPSP
90$
	RET

	XDEF	WrenchPipe

WrenchPipe

	TEST_W	FGT_System2(A4)		; System already destroyed?
	BNE.S	90$

	SCN	469			; Wrench pipe, throw it in room
	MOVE.L	#RNSWAY5,6(A0)		; Pipe now simply in room

 	MOVE.W	#NSYSTEM,FGT_System2(A4) ; System has been vandalised
	MOVE.W	#NSTOPCOCK,D0
	GETINFO				; Stopcock open?
	BTST	#4,(A0)
	BEQ.S	05$
	CALL	StartFlood		; Removing pipe with open stopcock
05$
	POPSP
90$
	RET

SP.MOVE.ABSTRACT

	CMP.W	#NABSTRACT,D0	; Move the abstract painting...?
	BNE.S	40$
	TEST_B	FlagPiconwall(A4); Don't do anything if pic not on wall. 
	BEQ.S	40$
	MOVE.W	#375,FGT_Bed2(A4)
	SCN	371		; A dark hole is revealed...
	MOVE.W  D0,-(SP)
	CLR.B	FlagPiconwall(A4) ; The piccy ain't on the wall.
	MOVE.W	#NHOLE,D0	; Save noun #
	GETINFO			; Move the hole 'in'
	BCLR	#7,6(A0)
	MOVE.W	(SP)+,D0	; Restore noun #
	GETINFO
	MOVE.L	#RNBEDROOM2,6(A0)
	SETEQ
	RET
40$:
	SETNE
	RET			; You now get the painting


	XREF	GumFlag

SP.CHEW
	CMP.W	#NGUM,D0
	BNE.S	90$

	BTST	#4,3(A0)		;moveable?
	BNE.S	10$			;NE => yes, not stuck yet, may chew
	SCN	123			;'what a revolting thing to do!'
	BRA.S	80$
10$
	BSET	#2,GumFlag(A4)		;so put knows it's been chewed on
	DO	FIXCONT			;in case it's contained
	BSET	#3,6(A0)		; set intrinsic
	BNE.S	20$			; was it already??
	MOVE.L	#$08000000,6(A0)	;make intrinsic + fod contained etc	
	SCN	444			; pop gum in mouth
	BRA.S	80$
20$:
	SCN	443			; You chew the gum	
80$:
	POPSP
90$:
	RET


SPA.INVENT
	MOVE.W	#NGUM,D0
	GETINFO
	BTST	#7,6(A0)		; out? (i.e. eaten/etc...)
	BNE.S	10$
	BTST	#3,6(A0)		; Intrinsic
	BEQ.S	10$
	SCN	445			; Chewing the gum.
10$:
	RET


SP.SHAKE

        XREF    ShakeIt,TIMER8

        CMP.W	#NTREE1,D0		; Shaking palm tree
	BNE.S	20$
	MOVE.W	#NCOCONUT,D0		; Coconut fallen?
	GETINFO
	CMP.W	#NTREE1,8(A0)		; Coconut on tree?
	BNE.S	15$
	SCN	423			; Coconut falls.
	CLR.W	NRT_COCONUTLOOK(A4)	; So you can pick it up!
	MOVE.L	#RNGDN2,6(A0)		; Coconut now in room!
	CALL	SETPRON			;so it==coconut
	BRA.S	80$
15$:
	SCN	424			; Multiple shakes! - buried 
	POPSP				; under avalanche of nuts
	DOGO	DEAD
20$:
	CMP.W	#NBOTTLE4,D0		; Shake champagne bottle
	BNE.S	90$
	BTST	#1,FlagChampers(A4)	; Been opened ?
	BNE.S	90$			; If it has, just a normal shake.

	BSET	#0,FlagChampers(A4)	; Set 'shaken'
	BNE.S	90$			; Only do it once...
        TEST_B  REMASTER(A4)
        BEQ.S   30$
        PUSH_L  D0/A0
	MOVE.W	#NCHAMPAGNE,D0
        GETINFO
        MOVE.B  #$3A,3(A0)              ; warm after shaken
        PULL_L  D0/A0
        MOVE.B  #15,TIMER8(A4)          ; 15 turns to unshake 
        POPSP
        BRA     ShakeIt
30$        
	PUSH_L	D0/A0
	MOVE.W	#NCHAMPAGNE,D0		; Now empty the bottle (Hmmm!)
* was setout2
	DO	SETOUT
	PULL_L	D0/A0
	BRA.S	90$
80$:
	POPSP
90$
	RET


*
* SP.FISH - arrives with D2/D3/D4 containing the noun#'s
*
* fish for x with y in z  gives x=d2 y=d3 z=d4
*
	XREF	FlagFish

SP.FISH

	CMP.W	#NCUE,D3
	BEQ.S	10$
	TEST_W	D3			;was something else specified?
	BNE.S	20$			;yes - no good

	CMP.W	#NLFISH,D2
	BEQ.S	10$

	CMP.W	#NMOAT,D4
	BNE	90$			;bloody range errors!

10$
	CALL	RodBuilt		;EQ=> yes
	BNE.S	20$
	MOVE.W	#NCUE,D0		;got it?
	DO	CARRIED
	BEQ.S	30$
20$
	SCN	435			;u need a rod
	BRA.S	80$
30$
	MOVE.W	#NMOAT,D0		;no local moat - 
	DO	VALID.NP
	BNE.S	90$			;back to verb routine

	BTST	#0,FlagFish(A4)		;done before?
	BEQ.S	40$
35$
	SCN	436			;fish seem to have learned
	BRA.S	80$
40$
	MOVE.W	#NMAGGOT,D0
	GETINFO
	CMP.L	#REL.MASK+NNEEDLE,6(A0)
	BEQ.S	50$
	SCN	436			;fish not interested
	SCN	438			;lack of suitable bait
	BRA.S	80$
50$
	BSET	#7,6(A0)		;nobble maggot
	BSET	#0,FlagFish(A4)		;flag done

	SCN	437			;about u catching fish

	SCORE	CATCHFISH

	MOVE.W	#NDFISH,D0
	GETINFO
	BCLR	#7,6(A0)
	MOVE.W	#NLFISH,D0		;to avoid 'which fish' hassles
	DO	SETOUT
80$
	POPSP
90$
	RET

	
*
* RodBuild
*
* If rod is built, returns EQ, else NE
*

RodBuilt

	PUSH_L	D0/D1/A0

	MOVE.W	#NCOTTON,D0
	GETINFO
	MOVE.W	12(A0),D1
	AND.W	#$3FFF,D1
	CMP.W	#NCUE,D1		; Check cue tied to cotton
	BNE.S	99$
	MOVE.W	#NNEEDLE,D0
	GETINFO
	MOVE.W	12(A0),D1
	AND.W	#$3FFF,D1
	CMP.W	#NCOTTON,D1		; Check cotton tied to hook
99$:
	PULL_L	D0/D1/A0
	RET

*-----------------------------------------
*
* SP.SIT
*

SP.SIT
	CMP.W	#NCOUCH,D0
	BNE.S	80$
	SCN	434
80$

	CMP.W	#NNEEDLE,D0
	BNE.S	90$
	MSG	OUCH
	POPSP
90$:
	RET

*
* Hooray mentioned by player - 
*
* called via process specials

SP.HOORAY

	MOVE.W	#NBOTTLE4,D0	
	GETINFO	
	BTST	#2,(A0)			; Broken ?
	BNE.S	90$	
	DO	VALID.NP		; Check champagne bottle is local
	BNE.S	90$
	CMP.B	#1,FlagChampers(A4)	; Shaken, but never opened
	BNE.S	90$
	SCN	147			; Kill!
	DO	DEAD
	BRA.S	99$
	
90$:
	MSG	NHAP
99$
	SETNE
	RET

	XREF	DoMachine,Weigh_Flag,MachineDied,NOUNBUF

GetCubes

* it will get here if we are in the fourroom...

	CMP.W	#NCUBE2,D0		; trying to get the cube?
	BEQ.S	10$
	CMP.W	#NCUBE2,NOUNBUF+2(A4)	; In either case?
	BEQ.S	10$
*	CMP.B	#1,Weigh_Flag(A4)	; ANything else on the weighing machine?
*	BEQ.S	10$			; yes.. check all
	TEST_B	Weigh_Flag(A4)		; even if things r off it ...
	BNE.S	10$			; it should be tested..
	RET				; no more special then...
10$
* Okay they could be trying to get any number of things from the 
* weighing machine... 
* first test the death condition (Getting the cube without doing anything..)
	TEST_B	Weigh_Flag(A4)		; put stuff on it?
	BNE.S	20$
	CMP.W	#NCUBE2,D0		; Getting the cube?
	BEQ	MachineDied		; yes die...
20$
* okay the correct condition - is get cube and ante cube are they doing it?
	CMP.W	#NCUBE2,D0
	BEQ.S	30$
	CMP.W	#NCUBE2,NOUNBUF+2(A4)	; and the ante cube?
	BNE.S	40$			; they are not.
	CMP.W	#NANTECUBE,D0		; And the ante cube... (please)
	BNE	MachineDied
	BRA	GETCUBEANTE		; wow get them...
30$
	CMP.W	#NANTECUBE,NOUNBUF+2(A4)	; and the ante cube
	BEQ	GETCUBEANTE			; YEAH...
40$
* okay they are not trying to get the right combination...
* Are they trying to get the ante cube & the dice?
	CMP.W	#NANTECUBE,D0		; The ante cube?
	BEQ.S	50$
	CMP.W	#NANTECUBE,NOUNBUF+2(A4)	; SECONd word?
	BNE.S	60$				; Something else maybe?
	CMP.W	#NDIE5,D0			; And the dice?
	BNE.S	60$
45$
	SCN	393			; pushing your luck a bit isn't this
	ST	WHOOPS(A4)		; No more parsing please.
	POPSP
	RET
50$
	CMP.W	#NDIE5,NOUNBUF+2(A4)	; And the die?
	BEQ.S	45$			; HA HA HA..
60$
* whatever they are trying to get is it on the weighing machine?
	GETINFO				
	CMP.L	#REL.MASK+NWMACHINE,6(A0)	; is it related to wmachine?
	BEQ	MachineDied			; go and die..
	RET

GETCUBEANTE

	MOVE.B	#2,Weigh_Flag(A4)		; set it to be done..
	SCN	77
	MOVE.W	#NCUBE2,D0
	GETINFO
	CLR.L	6(A0)			; ON  Inventory
	MOVE.W	#NANTECUBE,D0
	GETINFO
	CLR.L	6(A0)			; ON inventory
	SCORE	GETCUBE			; well some points for this...
	POPSP
	ST	WHOOPS(A4)		; Set whoops
	RET


	END
