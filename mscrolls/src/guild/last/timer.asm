******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.TIMER
*
*--------------------------------

*	Guild of Thieves !!!!!!!!!!!!!!!!!!!!!!

	include	"equates.asm"
	include	"roomequ.i"
	include	"nounequ1.i"
	include	"macros.asm"

	include "scen.i"

	XREF    CHEAT,TIMERS,GETDATA,ROOM,P.TN,ISARE,DO.SCN,DEAD,STOPRON
	XREF    W.REBUIL,GETNOUN2,TIMER2,P.SCN,TIMER3,SETPRON,TIMERA
	XREF    FlagSnake,CHKPAUSE,R.NOUN,R.NOUN2,SEATED,QCARRIED
	XREF    TIMER7,TIMER8,TIMERB,SETOUT,C.STATUS,FindRoom
	XREF	PRTTXT,LINE,PRTHEX,ODT_Explode,VALID.NP,FlagChampers,BombRoom
	XREF	FlagLute,TIMERD,TIMERF,TIMER10,TIMER11,TIMER12,GETRELN
	XREF	YNHANDLR,NPCINRM,UTIL.BS,CHAROUT,FULLSTOP
	XREF	P.NOUN,CloseDoor,CONNECT3,CALCNET,NPCINRM
	XREF	SPACE,TIMER14,P.LIST,TIMER15,DIED

	XDEF    TIMER
        XREF    REMASTER
        

*
* Handle all the kicking-off of various timer routines.
*
NumTimers	EQU	22		; # of top timer. (Bottom one is 0) 

TIMER
 
      IFNE    YCHEAT
	TEST_W  CHEAT(A4)		; Don't do timers in debug
	BNE.S   99$
      ENDC
 
	MOVEQ   #NumTimers-1,D4
10$
	LEA     TIMERS(A4),A1
15$
	TEST_B  1(A1,D4)		; If timer isn't running,just loop
	DBNE	D4,15$
	BEQ.S   99$

	SUBQ.B  #1,1(A1,D4)		; Decrement this timer
	BNE.S   20$			; If not expired, then loop
	MOVE.L  D4,-(A7)		; Stash D4
	LSL.W   #1,D4
	LEA     TimeTable(PC),A1	
	MOVE.W  2(A1,D4),D4		; Get routine address
      
      IFNE	YCHEAT
	BEQ.S	BadTimer		; no code to call?
      ENDC

	JSR     0(A1,D4)		; Execute timer call
	MOVE.L  (A7)+,D4		; Restore regs.
	TEST_B	DIED(A4)		; did we die in the last timer?
	BNE.S	99$			; yeah no more timers please..
20$
	DBRA	D4,10$			; ... and try the next one
99$
	RET

      IFNE	YCHEAT

BadTimer

	PRINT	<'^Waaa: bad timer - no code for timer #'>
	MOVE.L	(A7)+,D6
	CALL	PRTHEX
	MSGGO	LINE

      ENDC
 
TimeTable

	DC.W    0				;padding
	DC.W    RatRaceTime-TimeTable		;timer 1
	DC.W    SnakeKilling-TimeTable		;TIMER 2
	DC.W    SuccFinish-TimeTable		;TIMER 3
	DC.W    MynahFlies-TimeTable		;TIMER 4
	DC.W    GemFalling-TimeTable		;TIMER 5
	DC.W    SP.Flood-TimeTable		;TIMER 6
	DC.W    TellerTalk-TimeTable		;TIMER 7
	DC.W    ChampShake-TimeTable		;TIMER 8
	DC.W	PlaySingTime-TimeTable		;TIMER 9
	DC.W    DOPOISON-TimeTable		;timer A
	DC.W	MynahSquawk-TimeTable		;timer B 
	DC.W	OfferLute-TimeTable		;timer C 
 	DC.W	TreasureGo-TimeTable		;timer D
	DC.W	GKeeperDesc-TimeTable		;timer E
	DC.W	Queueing-TimeTable		;timer F
	DC.W	Bankmanager-TimeTable		;timer 10
	DC.W	Bankmanager1-TimeTable		;timer 11
	DC.W	GetGumUnlock-TimeTable		;timer 12
	DC.W	HereComesMrPeckham-TimeTable	;timer 13
	DC.W	GetGumEnter-TimeTable		;timer 14
	DC.W	BeesKill-TimeTable		;timer 15
	DC.W	LittleMan-TimeTable		;timer 16

*--------------------------------
  
 
DOPOISON 

        MOVE.W  C.STATUS(A4),D1
        BEQ.S   90$                     ;not poisoned
        SUBQ.W  #1,D1
	BNE.S   10$          		;FATAL POISON?
	FIXED_SCN Poison3		;OK U DIE NOW
	DOGO	DEAD
10$
	MOVE.W  D1,C.STATUS(A4)		;update state; 1 or 2
        CMP.B   #1,D1
        BNE.S   20$                     ; (was >=3) start to get ill
	FIXED_SCN Poison2		;OK VERY ILL  (MAYBE SOMETHIN U ATE)
        BRA.S   80$                     ;keep going
20$
	FIXED_SCN Poison1		;DONT FEEL TO GOOD..
80$        
        MOVE.B  #3,TIMERA(A4)           ;keep going
90$        
	RET

*
* Timer entry for 'play lute'. While the notes reverberate, you can
* sing 'urfanore pendra' but kill flags if it times out
*

	XREF	RoomDirStore
	XREF	RoomMod
	XREF	FlagInAir

PlaySingTime

	TEST_B	FlagInAir(A4)
	BNE.S	10$

	AND.B	#$E7,FlagLute(A4)	;kill have sung/have played lute
	RET
10$
	CMP.W	#RNWELL1,ROOM(A4)
	BLT.S	15$			;don't fall back if underwater
	CMP.W	#RNTUNNEL,ROOM(A4)
	BLE.S	20$
15$
	SCN	236			;end of time in air
20$
	SF	FlagInAir(A4)
	TEST_W	RoomMod(A4)		;has a room been changed?
	BEQ.S	90$

	MOVE.W	ROOM(A4),D7		;temporary store
	MOVE.W	RoomMod(A4),ROOM(A4)	;find out about this room
	DO	CALCNET			;get A0 pointing to exit data

	LEA	RoomDirStore(A4),A1

	MOVE.L	(A1)+,(A0)+		;retrieve old room data (12 bytes)
	MOVE.L	(A1)+,(A0)+	
	MOVE.L	(A1)+,(A0)+

	MOVE.W	D7,ROOM(A4)		;restore current room
	CMP.W	#RNPASSAGE3,ROOM(A4)
	BNE.S	30$
	SCN	206			;drown in the moat
	DOGO	DEAD
30$
	TEST_B	ExplodeBankFlag(A4)	;teller still here?
	BNE.S	90$
	CMP.W	#RNTILL1,D7		;at the till?
	BNE.S	90$
	MOVE.B	#1,TIMER7(A4)		;start the teller
	MOVE.W	#533,TellerMessage(A4)	;with this message
90$
	RET



*
* RatRaceTime - deal with the rat race held in the courtyard by the 
*               gatekeeper
*
* When the player crosses drawbridge, weather starts which will trigger timer1
* After rat race is announced, player forces instant race by entering
* courtyard
*
	XREF	FlagRatRace,TIMER1,Bet_RatList,Bet_Stake,Bet_Rat,RND.D1
	XREF	FlagRaceDue,FlagRRAnnounced

RatRaceTime

	TEST_B	FlagRaceDue(A4)		;eq=> waiting to trigger weather
	BNE.S	05$
	MOVE.W	#NWRATRACE,D0		;put rat race noun IN so weather
	GETINFO				;can happen
	BCLR	#7,6(A0)                ;clear OUT flag
	RET
05$
	BSET	#0,FlagRatRace(A4)	;how far through are we?
	BNE	RatRacePart2		;not part 1

	SF	FlagRRAnnounced(A4)	;so setmode won't re-trigger
	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	MOVE.L	#RNYARD,6(A0)
	CMP.W	#RNYARD,ROOM(A4)	;we in same room?
	BNE.S	07$
	MOVE.W	D0,NPCINRM(A4)		;yes - set NPC in room to gatekeeper
07$

* put rats 'IN'

	LEA	Bet_RatList(A4),A1
10$
	MOVE.W	(A1)+,D0
	BEQ.S	20$
	GETINFO
	BCLR	#7,6(A0)		;kill 'out' bit
	BRA.S	10$
20$
	CMP.W	#RNGATEHOUSE,ROOM(A4)	;see the gatekeeper go north?
	BNE.S	30$
	SCN	347
* The gatekeeper has just left for the courtyard.^
30$
	CMP.W	#RNYARD,ROOM(A4)	;happens in courtyard
	BNE.S	40$			;oops missed start!
	SCN	20			;gatekeeper arrives with rats
40$
	MOVE.B	#1,TIMER1(A4)		;set up for part 2
90$
	RET

*
* Ask them if they want to bet - setup YES/NO handler pointer
*

RatRacePart2
	
	BSET	#6,FlagRatRace(A4)	;so SETMODE wont interfere

	BSET	#2,FlagRatRace(A4)	;in part 3?
	BNE.S	RatRacePart3

	CMP.W	#RNYARD,ROOM(A4)	;same place as gatekeeper?
	BNE	RatRaceEnd		;nope, failed!
	SCN	21			;would you like to bet?
	LEA	RatRaceYESNO(PC),A0
	MOVE.L	A0,YNHANDLR(A4)		;catch reply
10$
	MOVE.B	#4,TIMER1(A4)		;no of moves u get to say yes
90$
	RET

RatRacePart3

	CLR.L	YNHANDLR(A4)		;fod yes/no as answer to betting
	BCLR	#2,FlagRatRace(A4)	;so as SP.BET knows its a valid time

	MOVEQ	#23,D1			;didn't bet - just watch then
	MOVEQ	#26,D2			;second msg if u didn't bet
	BTST	#7,FlagRatRace(A4)	;did u bet?
	BEQ.S	10$			;EQ => no

	MOVE.W	#NCHEQUE,D0		;since u always win (if u bet)
	GETINFO
	BCLR	#7,6(A0)		;set cheque in

	MOVEQ	#22,D1			;first msg if u did bet (hope he wins)
	MOVEQ	#25,D2			;msg after race (if u did bet)
10$
	CMP.W	#RNYARD,ROOM(A4)
	BNE.S	19$
	DO	P.SCN

	MOVE.W	Bet_Rat(A4),D0		;get the one u bet on
	BNE.S	18$

	MOVEQ	#100,D1
	RND_D1
	LEA	Bet_RatList(A4),A0
	MOVE.W	(A0)+,D0		;black (probabilities)
	CMP.W	#50,D1
	BPL.S	18$
	MOVE.W	(A0)+,D0		;brown
	CMP.W	#83,D1
	BPL.S	18$
	MOVE.W	(A0)+,D0		;white
	CMP.W	#93,D1
	BPL.S	18$
	MOVE.W	(A0)+,D0		;grey

18$

* at this point the winning rat is be setup

	MOVE.W	D0,STOPRON(A4)		;' @ ' translates to P.TN (D0)

	SCN	24			;about race (inc which wins)
	MOVE.W	D2,D1			;second message
	DO	P.SCN
	SCN	27			;gatekeeper clears up
19$


RatRaceEnd

	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	MOVE.L	#REL.MASK+NGATEHOUSE,6(A0)	;replace him

	MOVE.W	#NCAGE4,D0		;small bird cage
	GETINFO
	BCLR	#7,6(A0)

* put rats 'OUT'

	LEA	Bet_RatList(A4),A1
20$
	MOVE.W	(A1)+,D0
	BEQ.S	30$
	DO	SETOUT			;incase they've become attached in
	BRA.S	20$			;weird ways
30$
	CMP.W	#RNGATEHOUSE,ROOM(A4)	;we likely so see him come back?
	BNE.S	90$
	SCN	494			;yes, tell 'em about it
90$

	RET


	
*
* RatRaceYESNO - handles answer to 'u gonna bet?' question from gatekeeper
*

RatRaceYESNO

	CMP.W	#RNYARD,ROOM(A4)	;in right room still?
	BEQ.S	05$
	SCN	418			;go to right room!
	RET

05$
	CLR.L	YNHANDLR(A4)		;fod yes/no as answer to betting

	TEST_W	D5			;yes=0, no=1
	BEQ.S	10$			;said yes
	MOVE.B	#3,TIMER1(A4)		;give chance to use bet verb
	RET
10$
*	BSET	#7,FlagRatRace(A4)	;flag u betted
	MOVE.B	#5,TIMER1(A4)		;give time to bet/inspect rats
	SCN	348
* The gatekeeper, looking pleased with himself, says "you 
* had better be quick - you have to bet BEFORE the race!"^
90$
	RET




SnakeKilling

* okay this is gonna kill the player unless he gets to the hot house quick!

	LEA	SnakeSCNTable(PC),A0
	CLR.W	D1				;incase of high byte being
	MOVE.B	FlagSnake(A4),D1		;number where we are
	MOVE.W	0(A0,D1.W),D1			;move the number of scen 
;	BEQ.S	10$
	DO	P.SCN				;print scen mess in D1
10$:
	ADDQ.B	#2,FlagSnake(A4)		;Set it to next count
	CMP.B	#10,FlagSnake(A4)		;have you died?
	BEQ	DEAD				;Sorry mate but...
	MOVE.B	#1,TIMER2(A4)			;Set up timer for next round
	RET

SnakeSCNTable
	DC.W	111,111,112,112,113		;table of death mess


	XREF	FlagSucc
SuccFinish
* This will be called when there have been 7 moves after rubbing feet
* with the succulents
	CLR.B	FlagSucc(A4)		;in case you walk accross the coals
	SCN	353			;the succ's wear off
	RET

	XREF	FlagMynah
MynahFlies
* this is called 3 moves after you first go into the cage
* it will check to see if the flag is non zero
* if it is (meaning you've shut the cage with the mynah in it)
* it will leave it alone
* otherwise it will set up MynahFlag to be local 

	TEST_B	FlagMynah(A4)
	BNE.S	99$
	MOVE.B	#2,FlagMynah(A4)		;2 = local
99$
	RET

	XREF	FlagGem

* if the gem is falling and you haven't caught it it will drop
* into the pool

GemFalling:

	TEST_B	FlagGem(A4)	;NE=> it is still falling....
	BEQ.S	99$		;wow they caught it!
	CLR.B	FlagGem(A4)	;it is no longer falling
	MOVE.W	#NGEM,D0
	DO	SETOUT		;no longer in the game
	DO	P.TN		;print The Gem
	SCN	118		;drops into the pool with a sploosh
99$
	RET


**************************************************************888
*
* SP.Flood susses bits to do with flooding the cellar.
*

	XREF	Flooding,TIMER6,FloodClock,ODT_RATS,FGT_Steps

SP.Flood

	TEST_B	Flooding(A4)		; Flooding in progress?
	BEQ.S	90$			; No, so quit

	MOVE.B	#1,TIMER6(A4)		; Continue timer.

	ADDQ.B	#1,FloodClock(A4)	; Tick clock
	CMP.B	#1,FloodClock(A4)	; First turn - we must fod
	BNE.S	20$

	BCLR	#7,ODT_RATS(A4)		; new rat desc, new 1st glance
	MOVE.W	#HI+410,FGT_Steps(A4)
	MOVE.W	#NRATWATER,D0		; Bring in the water..
	GETINFO
	BCLR	#7,6(A0)		; Water now IN!

	BRA.S	80$
20$:
	CMP.B	#2,FloodClock(A4)	; Second turn
	BNE.S	30$
	MOVE.W	#HI+409,FGT_Steps(A4)	; Dead rats description - 
	MOVE.W	#175,ODT_RATS-2(A4)	; Also, rats themselves
					; NB this fods scn #, not noun #
	SCN	65			; Half flooded...
*
* Here we kill rats
*
	MOVE.W	#NRATS,D0
	GETINFO
	BSET	#1,(A0)			; Now DEAD
	BRA.S	80$

30$:
	CLR.B	Flooding(A4)
	CLR.B	FloodClock(A4)
	SCN	72			; Slip into water and drown
	DOGO	DEAD
80$:
90$:
	RET

*
* If the mynah is in cage, and cage is carried, or simply in room,
* then the mynah releases an expletive. If expletive is Hooray,
* and bottle is local, then explode bottle, etc...
*
	XREF	BirdKnowsHooray,BirdText1,BirdText2,BirdText3

QuietRoom

	DC.W	RNMILL1		;is the mynah in the mill?
	DC.W	RNCEMTR		;is the mynah in the cemetery?
	DC.W	RNHALL1		;is the mynah in the dining hall?
	DC.W	RNGATEHOUSE	;is the mynah in the gatehouse?
	DC.W	0

*
* Hacks 4/8/87 - Ken (pre Atari 800 & CPC6128) for v1.1


MynahSquawk

	MOVE.W	#NBIRD3,D0
	GETINFO
	BTST	#7,6(A0)
	BNE	99$			; If 'out' then no more squawks
	CALL	FindRoom		; Get top room
	LEA	QuietRoom(PC),A1
03$
	MOVE.W	(A1)+,D1
	BEQ.S	04$
	CMP.W	D1,D0			;forbidden room?
	BNE.S	03$
	BRA	90$
04$
	CMP.W	ROOM(A4),D0		; Is mynah here?
	BNE.S	40$			; If not, just check for explode
	CMP.B	#1,FlagChampers(A4)	; Shaken, not stirred?
	BNE.S	10$	
	MOVE.W	#NBOTTLE4,D0
	DO	VALID.NP		; If he is, is champagne here ?
	BEQ.S	20$			; If not, then random quip!
10$:
	CALL	MynahSays		;print one of the messages
	BRA	90$
*
* Oh goodie! Player,mynah and champers in same room!
*

20$:
	CALL	MynahSays		;print one of the messages
	BEQ	90$			;NE => said 'hooray'
	SCN	147
*	POPSP
	DOGO	DEAD
*
* Check if bird left in same place as bottle?
*

40$:
	CMP.B	#1,FlagChampers(A4)
	BNE	90$

        MOVE.W	#NBIRD3,D0
	CALL	FindRoom
	MOVE.W	D0,-(SP)
	
	MOVE.W	#NBOTTLE4,D0
	CALL	FindRoom
	CMP.W	(SP)+,D0		; Same loc as bottle?	

* was a CMP.W	(SP),D0 (Ken 4/8/87)
	BNE	90$			; Nope.. so it boring!
        
        ;; D0 = room of bird and bottle

        CMP.W   #RNMANOFFIS,D0          ; are they both in the manager office
        BNE.S   42$                     ; no, then its random

        ;;; but if they want it the original hard way!
        TEST_B  REMASTER(A4)
        BEQ.S   42$

        TEST_B  BirdKnowsHooray(A4)     ; any bird message hooray?
        BNE.S   46$                     ; yes, then its not random!
42$        
	MOVEQ	#3,D1
	RND_D1
	BTST	D1,BirdKnowsHooray(A4)	;will this one make it go bang?
	BEQ	90$

46$
        
* this line added (Ken 4/8/87) to keep stack even...
	MOVE.W	D0,-(A7)

* above NOTE!! TEST_W -(A7) is not allowed 'cos an interrupt on a Mac would
* kill us!

	MOVE.W	D0,D2
	MOVEQ	#8,D1			;loc info
	ST	D6			;non-valid allowed
	ST	D5			;want wordwise please
	DO	GETNOUN2		;kill all objs in room
50$
	MOVE.W	(A1)+,D0
	BEQ.S	60$
	GETINFO
	BTST	#0,5(A0)		;pseudo?
	BNE.S	50$			;don't set out!!
	BTST	#6,6(A0)		;is a room
	BNE.S	50$			;don't set out
	TEST_W	6(A0)			;Is randomly the same noun # as...
	BNE.S	50$			;the room #!!
	DO	SETOUT
	BRA.S	50$
60$
	SCN	146			; Message about explosion.
	MOVE.W	#NBIRD3,D0
	DO	SETOUT			; No bird.
	MOVE.W	#NBOTTLE4,D0
	DO	SETOUT			; No Bottle.
	ST	FlagMynah(A4)		;no mynah here
	MOVE.W	(A7)+,D2		;get room#
	DO	R.NOUN2
	MOVE.W	D0,ODT_Explode(A4)	; Set the room to be devastated
	MOVE.W	D0,BombRoom(A4)		; Set up explosion room
	CMP.W	#NMANOFFIS,D0
	BEQ	ExplodingOffice
	CMP.W	#NINBANK,D0
	BEQ	ExplodingInbank
	CMP.W	#NTILL1,D0
	BEQ	ExplodingAtTill
	BRA.S	99$	

90$:	
	MOVEQ	#4,D1			; Squawk interval...
	RND_D1				; is random...
	ADDQ.W	#3,D1			; but at least 3 moves.
	MOVE.B	D1,TIMERB(A4)		; so reset timer.
99$:
	RET


	XDEF	MynahSays

MynahSays

	PUSH_L	D0-D2/A0

	MOVEQ	#3,D1
	RND_D1
	LEA	BirdText1(A4),A0
	TEST_B	D1
	BEQ.S	10$
	LEA	BirdText2(A4),A0
	CMP.B	#1,D1
	BEQ.S	10$
	LEA	BirdText3(A4),A0
10$
	MOVE.B	D1,D2			;save message #
	SCN	508			;<'The mynah bird squawks loudly'>
	TEST_B	(A0)
	BNE.S	20$			;eq => no text here!
	DO	FULLSTOP
	BRA.S	90$
20$
	PRINT	<', "'>
30$
	MOVE.B	(A0)+,D0
	BEQ.S	40$
	MOVE.B	D0,D1
	DO	CHAROUT
	BRA.S	30$
40$
	CMP.B	#' ',D1
	BNE.S	50$
	DO	UTIL.BS
50$
	PRINT	<'".'>
90$
	MSG	LINE
	BTST	D2,BirdKnowsHooray(A4)	;will this one make it go bang?
	
	PULL_L	D0-D2/A0
	RET


OfferLute

	MOVE.W	#NLUTE,D0
	GETINFO
	CMP.W	#NMILLER,8(A0)		; Location = miller ?
	BNE.S	90$
	BTST	#6,5(A0)		;ahhh but was he given it?
	BNE.S	90$			;(in case u buy it then give it to him)
	SCN	467			; Miller offers lute
90$:
	BSET	#2,FlagLute(A4)		; Show we've made the offer (or he has lute)
	RET

TreasureGo

	CALL	DisappearTimer		;random delay to next call
	LEA	TreasureList(PC),A1	;point at base of treasure table
05$
	MOVE.W	(A1)+,D0		;noun number of a treasure
	BEQ.S	90$			;zero end marker
	GETINFO				;find out about the treasure
	TEST_B	6(A0)			;is it simply in a room?
	BNE.S	05$
	LEA	StealRoomTable(PC),A2	;base of room table
10$
	MOVE.W	(A2)+,D1		;get room number
	BEQ.S	05$			;zero end marker
	CMP.W	8(A0),D1		;is the treasure not there?
	BNE.S	10$			;then try the next room
20$
	CMP.W	ROOM(A4),D1		;is the player in the room?
	BEQ.S	05$			;then try the next treasure
* was setout2
	DO	SETOUT			;A0 and D0 identify the treasure
90$
	RET

	XREF	DisappearTimer

StealRoomTable

	DC.W	RNCEMTR
	DC.W	RNFRK
	DC.W	RNGATEHOUSE
	DC.W	RNYARD
	DC.W	RNMILL1
	DC.W	RNFIELD
	DC.W	RNBOAT
	DC.W	RNJETTY
	DC.W	RNOFFIS
	DC.W	RNTJUNC3
	DC.W	RNFOREST4
	DC.W	RNKITCHEN
	DC.W	RNQUARTERS
	DC.W	0

TreasureList

	DC.W	NPLECTRUM
	DC.W	NFOSSIL
	DC.W	NPAINTING1
	DC.W	NRUBY
	DC.W	NPOT
	DC.W	NDRESS
	DC.W	NRING
	DC.W	NBURNER
	DC.W	NSTATUETTE
	DC.W	NBROOCH
	DC.W	NRHINO
	DC.W	NGEM
	DC.W	NBAG2
	DC.W	NCHALICE
	DC.W	NCHIPS
	DC.W	0


*
* GkeeperDesc - changes how the gate keeper is described in room randomly
*

	XREF	UDT_GKeeper,TIMERE

GKeeperDesc

	MOVEQ	#80,D1
	RND_D1
	MOVE.B	D1,TIMERE(A4)		;re-trigger
	MOVEQ	#2,D1
	RND_D1
	MOVEQ	#29,D0
	ADD.B	D1,D0
	MOVE.W	D0,UDT_GKeeper(A4)
	RET

*****************************************************
*
* wait in short queue
*
Queueing

	SCN	529
	MOVE.B	#1,TIMERF(A4)
	RET

****************************************************
*
* the teller harangues you several times and then you give up
*

	XREF	TellerMessage

TellerTalk

	MOVE.W	TellerMessage(A4),D1	;do this message
	DO	P.SCN
	CMP.W	#536,TellerMessage(A4)	;leaving the till?
	BEQ.S	LeaveTill
	ADDQ.W	#1,TellerMessage(A4)	;next message
	MOVE.B	#1,TIMER7(A4)		;ready for next message
	RET

LeaveTill

	MOVE.W	#RNINBANK,ROOM(A4)	;go to main bank area
	CLR.W	SEATED(A4)		;in case we're sitting there
	CLR.B	TIMER7(A4)		;shut up teller
	RET

*---------------------------------------

        XREF    FlagChampers
        
ChampShake

        BCLR  #0,FlagChampers(A4)       ; as if not shaken
        PUSH_L  D0/A0
	MOVE.W	#NCHAMPAGNE,D0
        GETINFO
        MOVE.B  #$34,3(A0)              ; back to cold after shake subsides
        PULL_L  D0/A0        
90$        
        RET        

*---------------------------------------

	XDEF	ShowOut
	XREF	ManagerMessage

Bankmanager

	MOVE.W	ManagerMessage(A4),D1	;do this message
	DO	P.SCN
	CMP.W	#558,ManagerMessage(A4)	;shown out of office
	BNE.S	10$
	MOVE.B	#1,D7                   ; time allowed to put gum
        TEST_B  REMASTER(A4)
        BEQ.S   05$
        MOVE.B  #2,D7                   ; more time in remastered version
05$        
	CALL	ShowOut
	MOVE.W	#RNINBANK,ROOM(A4)
	CLR.W	SEATED(A4)
	RET
10$
	ADDQ.W	#1,ManagerMessage(A4)	;next message
	MOVE.B	#1,TIMER10(A4)		;ready for next message
	RET

ShowOut

	MOVE.W	#NMANDOOR,D0
	GETINFO
	MOVE.L	A0,A3
	CALL	CloseDoor
	MOVE.W	#NGUM,D0
	GETINFO
	CMP.L	#$30000000+NKEYHOLE,6(A0)
	BNE.S	10$
	SCN	166
	MOVE.B	D7,TIMER14(A4)
	BRA.S	20$
10$
	SCN	83			;and locks the door
	MOVE.B	D7,TIMER11(A4)		;go back in next move
	BSET	#6,(A3)			;and lock it
20$
	MOVE.W	#NMANTHING,D0		;make the manthing out
	DO	SETOUT
	MOVE.W	#NMANAGER,D0
	GETINFO
	MOVE.L	#RNINBANK,6(A0)
	RET

*--------------------------------

Bankmanager1		;office door will be locked if we get here (honest!)

	MOVE.W	#NGUM,D0
	GETINFO
	CMP.L	#$30000000+NKEYHOLE,6(A0)
	BNE.S	ManIntoOffice
	SCN	564			;manager can't open the door
	MOVE.B	#1,TIMER12(A4)		;but he will next move
	RET

*--------------------------------------

GetGumEnter

	CMP.W	#RNINBANK,ROOM(A4)
	BNE.S	GoGum
	SCN	82
	BRA.S	GoGum

GetGumUnlock

	CMP.W	#RNINBANK,ROOM(A4)		;can we see it happening?
	BNE.S	10$
	SCN	565				;gets gum unlocks door
10$
GoGum

	MOVE.W	#NGUM,D0
	DO	SETOUT

ManIntoOffice

	MOVE.W	ROOM(A4),D7		;where we are
	CMP.W	#RNINBANK,D7		;in the bank
	BNE.S	10$
	SCN	562			;manager goes back into office etc
10$
	MOVE.W	#RNMANOFFIS,ROOM(A4)	;temporary
	MOVE.W	#NMANAGER,D0
	GETINFO
	MOVE.L	#REL.MASK+NMANOFFIS,6(A0) ;the manager's back in his office
	CLR.L	D6			;count of objects found
	PUSH_W	D6
	MOVE.L	#RNMANOFFIS,D2		;check for this room
	CALL	GETITEMS
	MOVE.L	#NMANAGER+$1000000,D2	;things simply related to manager
	CALL	GETITEMS
	TEST_B	D6			;any items?
	BEQ.S	20$
	SCN	559
	MOVE.L	A7,A0
	DO	P.LIST
	SCN	560			;returns locking the door
20$
	ADDQ.B	#2,D6
	ADD.L	D6,A7
	MOVE.W	D7,ROOM(A4)		;you are in the bank
	MOVE.W	#NMANDOOR,D0
	GETINFO
	BSET	#6,(A0)			;lock the door
	BRA	CloseDoor

*----------------------------------

	XREF	GoOfficeFlag

HereComesMrPeckham

	ST	GoOfficeFlag(A4)		;trigger specials_before
	RET

*--------------------------------

	XREF	ODT_TILL1,ODT_INBANK,ODT_FINBANK,FGT_Kbank,ExplodeBankFlag

ExplodingOffice

	Connect	MANOFFIS,THREEROOM,E

*	MOVE.W	#RNMANOFFIS,D0
*	MOVEQ	#DIR_E,D1
*	MOVE.W	#RNTHREEROOM,D2
*	CALL	CONNECT2		;opening from office to vault

	BCLR	#7,ODT_TILL1(A4)
	BCLR	#7,ODT_INBANK(A4)
	BCLR	#7,ODT_FINBANK(A4)
	MOVE.W	#140,ODT_Explode-2(A4)	;devastated office with opening

PeopleGo

	MOVE.W	#-1,NPCINRM(A4)		;there's noone here now
	ST	ExplodeBankFlag(A4)	;office exploded
	MOVE.W	#NKBANK,FGT_Kbank+2(A4)
	LEA	PeopleTable(PC),A0	;everyone buggers off
02$
	MOVE.W	(A0)+,D0
	BEQ.S	05$
	DO	SETOUT
	BRA.S	02$
05$
	CMP.W	#RNKBANK,ROOM(A4)
	BEQ.S	10$
	CMP.W	#RNINBANK,ROOM(A4)
	BEQ.S	10$
	CMP.W	#RNTILL1,ROOM(A4)
	BEQ.S	10$
	CMP.W	#RNMANOFFIS,ROOM(A4)
	BNE.S	20$
10$
	SCN	148                     ; everyone rushes out
20$
	MOVEQ	#5,D0
	LEA	TIMERF(A4),A0		;clear the bank timers
30$
	CLR.B	(A0)+
	DBRA	D0,30$
	CLR.B	TIMER7(A4)		;clear teller timer
	CLR.B	GoOfficeFlag(A4)
	RET

ExplodingInbank

	MOVE.W	#453,ODT_TILL1-2(A4)	;till devastated
	BCLR	#7,ODT_TILL1(A4)

ExplodeBank

	MOVE.W	#453,ODT_FINBANK-2(A4)	;bank devastated
	BCLR	#7,ODT_FINBANK(A4)
	MOVE.W	#NMANTHING,D0
	GETINFO
	MOVE.L	#NMANOFFIS+$1000000,6(A0)	;put things in the office
	BRA	PeopleGo

ExplodingAtTill

	MOVE.W	#453,ODT_INBANK-2(A4)	;in the bank devastated
	BCLR	#7,ODT_INBANK(A4)
	BRA	ExplodeBank

PeopleTable

	DC.W	NMANAGER
	DC.W	NTELLER
	DC.W	NQUEUE1
	DC.W	NQUEUE2
	DC.W	NSTAFF
	DC.W	NPEOPLE
	DC.W	0

GETITEMS

	PULL_L	A6
	PUSH_W	D6
	DO	GETRELN			;find out what's in the room
	PULL_W	D6
10$
	MOVE.W	(A1)+,D0		;find out about this item
	BEQ.S	20$			;zero terminator
	GETINFO
	BTST	#0,5(A0)		;not a pseudo
	BNE.S	10$
	CMP.L	6(A0),D2		;is the item simply in the office?
	BNE.S	10$
	BTST	#24,D2			;looking for related to npc?
	BEQ.S	12$
	BCLR	#6,5(A0)		;given to npc?
	BEQ.S	10$
12$
	MOVE.L	#RNINBANK,6(A0)		;put it in the bank
	CMP.W	#RNINBANK,D7
	BNE.S	10$			;messages only when in bank
	ADDQ.B	#2,D6			;another object returned
	MOVE.W	D0,-(A7)
	BRA.S	10$
20$
	PUSH_L	A6
	RET
	
	XREF	TimeInGDN1
        XREF    CANTSAVE
        
BeesKill
* if you open the hive - this starts this timer which will eventually
* kill you after three moves.

        BSET    #3,CANTSAVE(A4)
	CMP.W	#RNWELL1,ROOM(A4)	; gone to seek refuge in the well?
	BNE.S	5$
	SCN	207
	DOGO	DEAD
5$
	MOVE.W	#94,D1			; begining of scen messages
	ADD.B	TimeInGDN1(A4),D1	; add the number in we are
	DO	P.SCN			; print the scenario message number
	ADD.B	#1,TimeInGDN1(A4)
	CMP.B	#3,TimeInGDN1(A4)	; at 3 we die..
	BNE.S	10$
	DOGO	DEAD			; go and kill people.
10$
	MOVE.B	#1,TIMER15(A4)		; Reset the timer...
	RET

*------------------------------

	XREF RubbishFlag,BankedTreasure,BankNotice

LittleMan

	CLR.W	D0
*	TEST_B	ExplodeBankFlag(A4)	;anyone in the bank?
*	BNE	90$
* duplicate test - ajs

	MOVE.B	BankedTreasure(A4),D0
	CMP.B	#15,D0			;more than 15 treasures(can go in a 2nd
	BGT	90$			;time when game finished)
	ADDQ.B	#1,D0			;change message after 1st deposit
	LSR.B	#1,D0			;and then every 2 deposits
	ADD.W	#517,D0			;add in base message number
	CLR.B	D7
	CMP.W	BankNotice+2(A4),D0	;message changed?
	BEQ.S	50$
	ST.B	D7			;the message has changed
	MOVE.W	D0,BankNotice+2(A4)	;the notice has this message now
	CMP.W	#525,D0			;all the treasure?
	BNE.S	50$
	MOVE.W	#NOAKDOOR,D0		;find out about bank door
	GETINFO
	BCLR	#6,(A0)			;unlock it
50$
	TEST_B	RubbishFlag(A4)		;any rubbish?
	BEQ.S	60$
	MOVE.W	#NRUBBAG,D0
	GETINFO
	BCLR	#7,6(A0)		;put the bags there
	BCLR	#4,(A0)			;and not open
	BSET	#5,(A0)			;and closed
	BSET	#31,D7			;something to see
60$
	CMP.W	#RNKBANK,ROOM(A4)	;can we see it brought out?
	BNE.S	90$
	TEST_L	D7			;anything to see?
	BEQ.S	90$
	SCN	136			;man comes out
	TEST_B	RubbishFlag(A4)		;throwing out rubbish?
	BEQ.S	65$
	SCN	150			;little man throws out rubbish
	TEST_B	D7			;changing notice?
	BEQ.S	70$
	PRINT	<', '>
65$
	SCN	134			;changes notice
70$
	SCN	156			;and back into the bank he goes
90$
	SF	RubbishFlag(A4)
	RET

	END
