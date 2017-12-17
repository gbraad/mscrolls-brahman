*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SETMODE1
*
*--------------------------------

	include	"equates.asm"
	include	"roomequ.i"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"macros.asm"

	include	"scoreref.asm"

 
	XREF	ROOM,RIDING,SEATED,P.SUB,W.GETOFF,CARRIED,DO.SCORE,R.NOUN2
	XREF	WHRIDING,CHEAT,USER.DB,NEXITFLG,QCARRIED,P.TN
	XREF	W.ROPED,GETDATA,SCRPNT,PRTTXT,VERB,DO.SCN,FULLSTOP
	XREF	TIMER2,TIMER4,TIMERB,TIMERC,DEAD
	XREF	Flooding,Flooded,BombRoom,SETOUT,DOBREAK
	XREF	FlagRaceDue,FlagRatRace,TIMER1,LastDirection,FlagRRAnnounced
	XREF	TIMER7,TIMERF,TIMER11,TIMER10,VALID.NP

	XDEF	SETMODE,NEWROOM,NEWROOM2

	XDEF	KilledByRats,LeaveMill
        XDEF    MynahStart

*--------------------------------
*
* SETMODE - HANDLES SPECIALS
*	     THAT CONTROL MOVEMENT
*	     BETWEEN LOCATIONS 
*
* NOTE   ROOM TO ENTER IS IN D1.B
*
*	  BYTESWISE
*	 +++++++++++
*
*--------------------------------
  
  
 
SETMODE
 
	LEA     ROOM(A4),A5		;SAVE MEM
	AND.W   #$FF,D1			;URK!! - USUALLY FFXX - XX = ROOM
 
	TEST_W   RIDING(A4)		;ON A VEHICLE?
	BNE.S   01$			;OK - DONT GET UP THEN
	TEST_W   SEATED(A4)		;SITTING ON SOMETHING?
	BEQ.S   01$			;NOPE
	DO	P.SUB			;'YOU '
	MSG     GETOFF
	CLR.W   SEATED(A4)
01$
	CALL    WHRIDING		;STOPPED COS OF VEHICLE
	TEST_W   D7			;0= NOT STOPPED
	BEQ.S   02$
	RET
02$
    
    IFNE     YCHEAT
	TEST_W   CHEAT(A4)		;IF CHERATING THEN FORGET SPECIALS
	BNE     NEWROOM2		;!!! BEWARE - THIS LETS U WALK OUT OF A
    ENDC				;ROOM WHILEST TIED TO CONTENTS OF IT!! 
	
 
	TEST_B	USER.DB(A4)		;IF IN 'USER DEBUG' THEN FORGET SPECIALS
	BNE     NEWROOM			;BUT TAKE CARE OF 'TIED TO ROOM' BITS
	

	CALL	DoRoomSpecials		;SPECIALS GO HERE!!!!

 
NEWROOM
  
	TEST_W   NEXITFLG(A4)		;CAN'T GO TO NEXT ROOM IF TIED TO THIS
	BEQ.S	10$
	MSGGO	ROPED			;ROOM'S CONTENTS
10$
 
 
NEWROOM2
  
	MOVE.W  SEATED(A4),D0		;can't have got here if not a vehicle
	BEQ.S   10$			;no vehicle
	GETINFO				;ok, we have a vehicle so lets move it
	CLR.L   6(A0) 
	MOVE.B  D1,9(A0) 
10$
	MOVE.B  D1,1(A5)		;NEW ROOM #

*
* this next line is why its called 'SETMODE'
*

	MOVE.W  #1,SCRPNT(A4)		;LET SCREEN KNOW TO DO DESC & CONTENTS
90$
	RET  

*-------------------------------------------------------------------------
	  

* RRENTRY macro for the RoomToRoom table

RRENTRY	MACRO	* room1,room2,code

	IFC	'\1','any'
	DC.B	0
	ENDC
	IFNC	'\1','any'
	DC.B	RN\1
	ENDC

	IFC	'\2','any'
	DC.B	0
	ENDC
	IFNC	'\2','any'
	DC.B	RN\2
	ENDC

	DC.W	HI+\3-*		;flag as 'not safe'!

	ENDM

  
*--------------------------------
*
* RRSAFE - room to room, but safe to makelocal via

RRSAFE	MACRO	* room1,room2,code

	IFC	'\1','any'
	DC.B	0
	ENDC
	IFNC	'\1','any'
	DC.B	RN\1
	ENDC

	IFC	'\2','any'
	DC.B	0
	ENDC
	IFNC	'\2','any'
	DC.B	RN\2
	ENDC

	DC.W	\3-*

	ENDM
  
*
* SafeSetmode
*
* this susses if a given room -> room move is likely (ish!) to be safe 
* (esp for makelocal)
*
* On entry: D0 = room to leave
*           D1 = room to enter
*
* returns EQ => is safe
* else MAY do nasties
*

		

	XDEF	SafeSetmode

SafeSetmode

	PUSH_L	D2/A2
	LEA	RoomToRoom(PC),A2	;pointer to our table
10$
	MOVE.B	(A2),D2			;source room
	BEQ.S	20$			;EQ=> wildcard
	CMP.B	D2,D0			;right room?
	BNE.S	40$
20$
	MOVE.B	1(A2),D2		;destination room
	BEQ.S	30$			;wild card?
	CMP.B	D2,D1
	BNE.S	40$
30$
	BTST	#7,2(A2)		;check 'is safe' flag
	
	BNE.S	50$			;exit (unsafe)

	ADDQ.L	#4,A2
	BRA.S	10$			;reload D0+loop

40$
	CMP.B	#-1,D2			;-1 terminator
	BEQ.S	50$
	ADDQ.L	#4,A2
	BRA.S	10$			;loop

50$
	PULL_L	D2/A2
	RET				;bye..
	

*-------------------------------
*
*
* RoomToRoom - table of room#'s and code to call if going from first
*		  room to second
*
*--------------------------------
*
* DoRoomSpecials - this is setmode done in a sensible(ish) fashion!
*
* On entry A5 points at ROOM (current room no, a WORD)
*	    D1 (.B) is the target room #
*
* On exit D1 is new room#
*
* This bit of code uses a table to match up target combinations of 
* source and destination (0 in either case => don't care) and then calls
* some code to deal with it
*

DoRoomSpecials


	LEA	RoomToRoom(PC),A2	;pointer to our table
10$
	MOVE.W	(A5),D0			;get current room# (byte really)
20$
	MOVE.B	(A2),D2			;source room
	BEQ.S	30$			;EQ=> wildcard
	CMP.B	D2,D0			;right room?
	BNE.S	50$
30$
	MOVE.B	1(A2),D2		;destination room
	BEQ.S	40$			;wild card?
	CMP.B	D2,D1
	BNE.S	50$
40$
	MOVE.W	2(A2),D0		;wow! a complete match!
	BCLR	#15,D0			;nobble 'is safe' flag
	

* now call the code concerned
*
* this may turn into a JSR one day...
* Call code with dest room # in D2.B

	JMP	2(A2,D0.W)		;THIS DEPENDS ON D0.W being PC rel!

* so should be thought of as falling through to here
* SO DON'T STOMP ON A2 !!!!

	ADDQ.L	#4,A2
	BRA.S	10$			;reload D0+loop

50$
	CMP.B	#-1,D2			;-1 terminator
	BEQ.S	60$
	ADDQ.L	#4,A2
	BRA.S	20$			;loop
60$:
	CALL	AllRoomSpecials		;any move does this
	RET				;bye..
	


*
* RoomToRoom - table of room#'s and code to call if going from first
*		  room to second
*
* Format of table:
*
* 	RRENTRY		<source room>,<destination room>,<addr. of code>
*
* in addition to rooms 'any' is allowed, matching any room.
* table is terminated by a -1 WORD
*

RoomToRoom

* getting in/out boat dry
	RRENTRY		BOAT,JETTY,OutBoat
	RRENTRY		JETTY,BOAT,InBoat

* remove the thief for a while...
	RRSAFE		JETTY,any,RemoveThief

* first meeting with gatekeeper - escorted to entrance hall
	RRSAFE		SCRUB3,GATEHOUSE,GKeeper1

* and don't let 'em out if the drawbridge is up!
	RRSAFE		GATEHOUSE,SCRUB3,GKeeper1

* check whether they are carrying any valuables
*	RRENTRY		YARD,GATEHOUSE,Setmode_GKeeper3 ;now in special0

* check to see if rat race starts.
	RRSAFE		any,YARD,ForceRace

* entering mill
	RRSAFE		FIELD,MILL1,EnterMill
* leaving mill
	RRSAFE		MILL1,FIELD,LeaveMill

* back from lab?
	RRSAFE		LAB,BEDROOM2,FromLab

* going into the gardens?
*	RRSAFE		TEMPLE,GDN1,IntoGarden


* toll gate up?
	RRSAFE		PSG4,TJUNC1,TollGate

* raised the counter?
	RRSAFE		SHOP2,SHOP3,Counter
	RRSAFE		SHOP3,SHOP2,Counter

* Leaving the Insect house?
	RRENTRY		HSE,any,SnakeKills

* Entering the Hot House (snake could die!)
	RRSAFE		any,GDN2,SnakeDies

* exit the muddy room clears the beam flag!
	RRSAFE		RMUD,any,ClearBeam

* Entering the hot passage (snake could die!)
	RRSAFE		any,PSG2,SnakeDies

* Crossing the coals (both ways!)
	RRENTRY		FNRM,STPS,WalkCoals
	RRENTRY		STPS,FNRM,WalkCoals

* entering the mynah cage sets off timer4.
	RRSAFE		PSG7,CAGE,MynahStart


	RRSAFE		TUNNEL,SUMP,FreshAir
	RRSAFE		SUMP,TUNNEL,GoUnder

	RRENTRY		WELL1,any,Dragdown
	RRENTRY		WELL2,any,Dragdown
	RRENTRY		WELL3,any,Dragdown

	RRENTRY		SHAFT1,CRAB1,SlipNFall
	RRSAFE		BANK,SHAFT1,FloatIn
	RRENTRY		CRAB1,SHAFT1,ClimbShaft

* using the rope (wet) in the caves - needs the gloves
	RRSAFE		CAVE2,CAVE3,HaveGloves
	RRSAFE		CAVE3,CAVE2,HaveGloves2

	RRENTRY		SWAY5,CELLAR,ToCellar
	RRENTRY		CELLAR,SWAY5,LeaveCellar
	RRSAFE		SWAY5,PASSAGE1,LeaveSteps

* death in the rainbow room(s)
	RRENTRY		any,SQUAREDEATH,RainbowDeath

* msg about dimming squares

	RRENTRY		BLACKSQUARE,VIOLETSQUARE1,DimSquares
	RRENTRY		WHITESQUARE,VIOLETSQUARE2,DimSquares

        RRENTRY		SWAY1,BLACKSQUARE,StartSquares
        RRENTRY		BLACKSQUARE,SWAY1,EndSquares
        RRENTRY		CRYPT,WHITESQUARE,StartSquares
        RRENTRY		WHITESQUARE,CRYPT,EndSquares
*
* whole set of squares to go here
*

* Endgame stuff - any room to dummy means they are trying to go 
* through the holes in the floor.
	RRENTRY		any,DUMMY,DoCubicRooms

* bank manager follows you when you leave the office
	RRSAFE		MANOFFIS,INBANK,ManagerFollows

* can't go to 3room until vault door is blown up
	RRSAFE		MANOFFIS,THREEROOM,VaultDoor

* queues prevent you going to the till
	RRSAFE		INBANK,TILL1,QueueHassle

* levitate over the moat from 'by the moat'
	RRSAFE		PASSAGE3,SCRUB1,LevitateMoat

* levitate from kbank to in the bank if the roof has gone
	RRSAFE		KBANK,INBANK,LevitateBank

* levitate from inbank to kbank,manager's office if roof gone
	RRSAFE		INBANK,any,LevitateBankNotTill

* levitate from at the till to kbank,manager's office if the roof has gone
	RRSAFE		TILL1,any,LevitateBankNotInbank

* trying to leave the boat other than westwards
	RRENTRY		BOAT,BOAT,WalkOnWater1

* floating out over the lake
	RRENTRY		BHSE,BHSE,OverLake
	RRENTRY		DRM5,DRM5,OverLake

	DC.W		-1


**********************************************************************
*
* Now some of the bits of code...
*
*
* WARNING!!!!!  code which relies on ROOM (eg VALID) will act in a 
*               non-obvious way when called from here since the room is not
*               changed until AFTER the code is called and so ROOM is the
*               OLD one
*

*
* counter up?
*

Counter

	MOVE.W	#NFLAP,D0
	GETINFO
	BTST	#4,(A0)			;open?
	BNE.S	90$			;EQ=> yes, u can pass
	SCN	307		;The counter has been designed to keep future 
*				;and current customers of the shop separate.
	POPSP
90$
	RET

*
* Toll gate into zoo
*

TollGate

	XREF	FlagTollGate

	TEST_B	FlagTollGate(A4)
	BNE.S	90$			;NE=> it's up - u can pass
	SCN	108			;message about it being down
80$
	POPSP
90$
	RET




*
* OutBoat/InBoat- checks out that is boat has been pulled to the jetty
*

	XREF	MoorFlag,SCORE_,FlagLeftBoat,FlagInAir

OutBoat

	TEST_B	FlagInAir(A4)		;floating?
	BEQ.S	5$
	RET				;normal movement(no boat pushed etc.)
5$
	BTST	#0,MoorFlag(A4)		;test+clear flag
	BNE.S	10$			;NE=> boat near jetty

	CMP.W	#VNJUMP,VERB(A4)	;can jump OUT, but not in again!
	BNE.S	WalkOnWater		;(unless it's near the jetty)
10$:
	ST	FlagLeftBoat(A4)	;so as we can tell when u go back!
	SCORE	JETTY			; Give him points!
	BRA.S	BoatPushed

InBoat
	TEST_B	FlagInAir(A4)
	BEQ.S	10$
	SCN	222			;move into the boat if flying
	SF	FlagInAir(A4)
	RET				
10$
	BTST	#0,MoorFlag(A4)
	BNE.S	BoatPushed		; See if near jetty	

	CMP.W	#VNJUMP,VERB(A4)
	BNE.S	WalkOnWater
	SCN	309
* Your leap clears the water between the jetty and 
* boat, however your stay in the boat isn''t 
* long as your momentum carries you into the lake beyond.
	BRA.S	FishedOut

WalkOnWater1
	TEST_B	FlagInAir(A4)
	BEQ.S	WalkOnWater
	SCN	223			;float back to where you were
	CLR.B	FlagInAir(A4)
	RET

WalkOnWater
	SCN	310
* Happily, as though without a care in the world, you 
* step onto the lake between the jetty and boat. Remarkably 
* the water fails to support your weight!

FishedOut

	CMP.B	#-1,ThiefHere(A4)	; is the thief around?
	BNE.S	05$ 
	SCN	149			; and with nobody to fish you out
	POPSP
	DOGO	DEAD			; and you have died...
05$
	SCN	5			;thief fishes u out
	TEST_W	SCORE_(A4)		;they scored yet?
	BEQ.S	10$
	SUBQ.W	#1,SCORE_(A4)		;small penalty for being a wally
10$:
	MOVEQ	#RNBOAT,D1
	RET

BoatPushed
	BCLR	#0,MoorFlag(A4)		;test+clear flag
	BEQ.S	10$			;EQ=> boat wasn't near jetty!
	SCN	308	
* As you move between the boat and the jetty,
* the boat is pushed out into the lake again.
10$:
	RET				;ok, let them off...


*********************************************
*
* GKeeper1 - first crosing of drawbridge
*

	XREF	FlagGK1
	XREF	FlagDBlowered,TIMER1

GKeeper1

	BTST	#0,FlagDBlowered(A4)	;eq=> bridge still up
	BNE.S	10$			;NE=> no, can get onto it

	TEST_B	FlagInAir(A4)
	BEQ.S	01$
	SCN	51			;you hover over the bridge
	BRA.S	10$			;then get spiel
01$
	SCN	57			;at present the drawbridge is
	SCN	58			;raised
	POPSP				;pop stack to avoid room change
	DOGO	FULLSTOP

10$
	MOVE.W	#NGATEKEEPER,D0
	GETINFO
	CMP.L	#REL.MASK+NGATEHOUSE,6(A0)	;is he here?
	BNE.S	20$
	BSET	#0,FlagGK1(A4)		;this only happens once
	BNE.S	20$			;NE => has happened
	SCN	11			;gatekeeper says...
	MOVE.W	#RNHALL2,D1		;he escorts u.. here
	MOVE.B	#30,TIMER1(A4)		;30 free moves to get note to bet
20$
	RET

*
* EnterMill have vanes stoped?
*

	XREF	FlagMillStopped

EnterMill

	TEST_B	FlagInAir(A4)			;are we flying?
	BEQ.S	01$
	SCN	50				;no no - painful!
	BRA.S	80$
01$
	BTST	#0,FlagMillStopped(A4)
	BNE.S	10$				;ne => has stopped, ok
	SCN	188				;beware of the vanes!!
	BRA.S	80$
10$

* stop u reentering the mill with broken lute?

	MOVE.W	#NLUTE,D0
	GETINFO
	BTST	#0,FlagLute(A4)
	BEQ.S	20$				;Not broken, So OK	
	DO	CARRIED				; Toting it ?
	BNE.S	20$				;NE=> not carried
	SCN	311
	
* The miller wouldn''t like to see the lute in it''s 
* current state!^
	BRA.S	80$
20$:
	BTST	#2,FlagLute(A4)			; Offered lute?
	BNE.S	90$
	MOVE.B	#5,TIMERC(A4)			; Set up the offer...
	BRA.S	90$
80$
	POPSP
90$
	RET

* leaving mill with lute?

LeaveMill

	XREF	FlagLute,FGT_Lute,FlagFell,IGNORAJ
        XREF    REMASTER

* CLR moved from AFTER CMP on 'carefully' to avoid miller trying to sell
* you the lute after you have left mill!
*
* Ken 16/8/87 (post atari800/all 68k/c64/apple/pcw)

	CLR.B	TIMERC(A4)		; Stop lute being offered

	MOVE.W	#NLUTE,D0		;has he got the lute?
	CALL	QCARRIED		;Carried ?
	BNE.S	90$                     ;no
	BSET	#0,FlagFell(A4)		;only once john
	BNE.S	90$
        
        CMP.W	#1,IGNORAJ(A4)		;carefully?
	BEQ.S	70$                     ;fix - you get points
        TEST_B  REMASTER(A4)
        BNE.S   70$                     ;no clue about being careful
        
	SCN	90			;as u leave u fall heavily
	GETINFO
	CMP.L	#CONT.MASK+NBAG1,6(A0)	;in the swag bag?
	BNE.S	10$                     ;no
	SCN	87			;bag protects it
        SCN	84			;watch step next time
70$        
        SCORE	OUTMILL
        BRA.S   90$
10$
	BSET	#0,FlagLute(A4)		; If already broken,
	BNE.S	80$                     ;yes, but watch step
	SCN	86			;whoops - landed on lute
	MOVE.W	#HI+264,FGT_Lute(A4)	;set desc to fucked
80$
	SCN	84			;watch step next time
90$
	RET



********************************
*
* FromLab, going back to bedroom via 'weird exit'
*

FromLab

	SCN	312

* A slight feeling of disorientation moves through you.^

	RET

********************************
*
* IntoGarden - going from temple to gardens
*
* this now does this special based upon opening the hives

*	XREF	TimeInGDN1

*IntoGarden
*
*	MOVE.B	#3,TimeInGDN1(A4)
*	RET
*
	XREF	FlagSnake
        XREF    CANTSAVE

SnakeKills

* This will set the snake up to kill you as you leave the Insectory
* You can only stop the snake from killing you by going into the hot house 
* or the room of hot coals
        
	TEST_B	FlagSnake(A4)		;already done it?
	BNE.S	99$			; zero means we can do it
	MOVE.B	#2,TIMER2(A4)		;Set up the timer
	MOVE.W	#NSNAKE,D0		;Not out any more
	GETINFO
	BCLR	#7,6(A0)		;Set it in!
        BSET    #2,CANTSAVE(A4)         ;no save whilst dying
	SCN	110			;Snake drops on you, etc...
99$
	RET

SnakeDies

* if you enter the hot house with the snake strangling you the heat 
* kills it!
* or the room of hot coals

	TEST_B	FlagSnake(A4)		;EQ=> Snake not strangling you
	BLE.S	99$			;-1 = it has happened snake is dead!
	ST	FlagSnake(A4)		;Snake is dead (long live the snake)
	CLR.B	TIMER2(A4)		;stop the timer from happening
	MOVE.W	#NSNAKE,D0
	DO	SETOUT
	SCORE	KILLSNAKE
        BCLR    #2,CANTSAVE(A4)
	SCN	234			;The snake dies ...
99$
	RET

	XREF	FlagSucc,FlagInAir

WalkCoals

	TEST_B	FlagInAir(A4)
	BEQ.S	01$
	SCN	49			;you hover over the coals
	RET
01$

* if your trying to cross the coals without having succ'ed your feet
* you die.!

	TEST_B	FlagSucc(A4)		;are they active?
	BNE.S	20$			;yes
	SCN	120			;yes you die a rightfull
	POPSP				;pop de stack for dead
	DOGO	DEAD			;you are dead (now what)!
20$
	SCN	119			;you skip over the coals
	RET

MynahStart

	MOVE.B	#2,TIMERB(A4)		; Start it squawking!
	MOVE.B	#3,TIMER4(A4)		; in 3 moves time it will start...
					; ...checking for exits.
	RET

	XREF	FlagBeam

ClearBeam

	CLR.B	FlagBeam(A4)		;just in case...

        PUSH_L  D0/A0
        MOVE.W  #NMIRROR,D0
        GETINFO
        MOVE.B  #$19,3(A0)              ; back to blunt/hard
        PULL_L  D0/A0
	RET

	XREF	UWClock

FreshAir

	SCN	103
	RET

GoUnder

	SCN	390
	RET

* Checks prohibiting underwater moves when toting statue
Dragdown

	MOVE.W	#NSTATUE,D0
	DO	CARRIED
	BNE.S	10$
	POPSP
10$
	RET	

* trying to descend thin shafts?

SlipNFall

	TEST_B	FlagInAir(A4)	;floating?
	BEQ.S	05$
	SCN	480
	CLR.B	FlagInAir(A4)	;don't fall back to the ground
	BRA.S	10$
05$
	MOVE.W	#NBOOTS,D0	;The wetsuit boots....
	GETINFO
	CMP.B	#4,6(A0)	;are they just worn??
	BEQ.S	10$		;Yep, so OK!
	SCN	104		;Player slips and falls
	POPSP
	DOGO	DEAD		;... end of player
10$
	RET

* trying to accend thin shafts?

FloatIn

	TEST_B	FlagInAir(A4)
	BEQ.S	10$
	SCN	481		;float into shaft
	CLR.B	FlagInAir(A4)
10$
	RET

ClimbShaft

	TEST_B	FlagInAir(A4)
	BEQ.S	10$
	SCN	483		;float into shaft
	BRA.S	80$
10$
	MOVE.W	#NBOOTS,D0
	GETINFO
	CMP.B	#4,6(A0)
	BEQ.S	90$
	SCN	80		;no good - too slippery
80$
	POPSP
90$
	RET

* Leaving the cellar

RATDATA
	DC.W	VNLOOK,NRATS

	XREF	NoReachRats

LeaveCellar

	MOVE.L	RATDATA(PC),NoReachRats(A4)	;rats out of reach
	RET

* Going into cellar from steps...

ToCellar

	TEST_B	Flooded(A4)
	BEQ.S	KilledByRats
	SCN	442		; U wade into the cellar...
	CLR.L	NoReachRats(A4)	;make rats reachable
	RET

* Leaving steps, check for flooding

LeaveSteps

	TEST_B	Flooding(A4)	; Zero if not flooding
	BEQ.S	Leave_exit	; OK
	SCN	233		; Rats come through door!

KilledByRats

	SCN	69		; they eat u!
	POPSP
	DOGO	DEAD

Leave_exit

	RET
*
* This code is called on every attempted move, AFTER all other
* Setmode specials are called

AllRoomSpecials

10$
	AND.W	#$FF,D1		; Clear top bits
	CMP.W	BombRoom(A4),D1	;Walking into bombed-out room?
	BNE.S	20$
	SCN	456		; Yes, So Bird flaps out over your head!
	MOVE.W	#2,SCRPNT(A4)	; Force new description.
	CLR.W	BombRoom(A4)	; Show we've dobne it
20$:
	RET


***************************************
*
* HaveGloves - you have to be wearing the gloves to climb
*              the wet rope in the caves (cave2/passage2)
*

HaveGloves

	TEST_B	FlagInAir(A4)
	BEQ.S	05$
	SCN	391			;you float up gently
	BRA.S	90$
05$
	MOVE.W	#NGLOVES,D0
	GETINFO
	CMP.B	#4,6(A0)		;just worn (by player)
	BNE.S	10$
	SCN	403			;ok
	RET
10$
	SCN	404			;too slippey john!
	POPSP
90$
	RET

* HaveGloves2 - just a silly message

HaveGloves2

	TEST_B	FlagInAir(A4)
	BEQ.S	05$
	SCN	422			;you glide gently down
	RET
05$
	SCN	405			;wheee - u slide down rope
	RET

*
* Death because you stepped on the wrong square!
*

RainbowDeath

	SCN	426		;u die john
	POPSP
	DOGO	DEAD

DimSquares

	SCN	430		;rest of squares dim
	MOVEQ	#0,D2
	MOVE.B	D1,D2
	DO	R.NOUN2		;room's noun# to D0
	DO	P.TN
	SCN	486
	RET

StartSquares
        BSET    #4,CANTSAVE(A4)
        RET

EndSquares
        BCLR    #4,CANTSAVE(A4)
        RET

ForceRace

	TEST_B	FlagRRAnnounced(A4)		; Due??
	BEQ.S	90$
	MOVE.B	#1,TIMER1(A4)		; Now IMMINENT!
	SF	FlagRRAnnounced(A4)
90$:
	RET



	XREF	LastThrow,SpotBits,CardOut
DoCubicRooms
JumpHole	EQU	499
Jump2Holes	EQU	500
WrongExit	EQU	502
* This means that they are going from one of the 'dice' rooms...
	
* Should they genuinely be going to dummy?
	CMP.W	#RNFOURROOM,ROOM(A4)		;trying to leave four room?
	BEQ.S	LeaveFour
	MOVE.W	LastThrow(A4),D5		;The last number we threw..
	BSET	D5,SpotBits(A4)			;Set that number as being blank
	BEQ.S	10$				;it was not blank
	MOVE.W	#RNDUMMY,D1			;they are off to dummy
	SCN	JumpHole		;They fall through exit..
	RET
10$
* Okay are they going through 2 holes?
	MOVE.W	D5,D6			;get the room they are going to
	LSL.W	#1,D6			;mulu by 2 for index
	LEA	ROOMHOLES(PC),A0	;a table of holes in each room
	MOVE.W	LastDirection(A4),D7	;The dir they are going in
	BMI.S	FUCKCUBE		;hmm a fuckup somewhere...
	MOVE.W	0(A0,D6.W),D5		;will contain the information about
					;Each room
	BTST	D7,D5			;NE => there is a hole there
	BEQ.S	20$
	SCN	Jump2Holes
	POPSP				;we are not changing rooms
	DOGO	DEAD			;sorry guys you are dead.
20$
* Fine now we are in the knew room
	LEA	DIEROOMS(PC),A0		;with the offsets
	MOVE.W	0(A0,D6.W),D1		;the new room
	SCN	JumpHole		;You jump through the hole
	RET

LeaveFour
* They are trying to leave the four room
* are they going by the correct exit? (ie joker one)
	CLR.W	D0
	MOVE.B	CardOut(A4),D0		;get the correct exit...
	LEA	FourExits(PC),A0	;the index table
	MOVE.B	0(A0,D0.W),D0		;the correct direction to go by
	CMP.W	LastDirection(A4),D0	;the right direction?
	BNE.S	80$			;nope.
	SCORE	OUTFOUR
	SCN	567			;you whiz ....
	MOVE.W	#RNDRM3,D1		;you end up in the white room
	BSET	#7,FGT_Thief(A4)	; and into boat description
	MOVE.W	#NTHIEF,D0		; Put the thief back into the boat
	GETINFO
	BCLR	#7,6(A0)		; put him back into the boat.
	MOVE.B	#1,ThiefHere(A4)	; Make him realise that he is back.
	RET				;okay chaps worked....
80$
	SCN	WrongExit		;The joker in the pack
	POPSP
	DOGO	DEAD

FUCKCUBE:
      
      IFNE	YCHEAT
	PRINT	<'Major Fuckup'>
      ENDC
	RET

DIEROOMS:
	DC.W	RNONEROOM
	DC.W	RNTWOROOM
	DC.W	RNTHREEROOM
	DC.W	RNFOURROOM
	DC.W	RNFIVEROOM
	DC.W	RNSIXROOM
ROOMHOLES:
	DC.W	%0000001000000000
	DC.W	%0000000010001000
	DC.W	%0000001010001000
	DC.W	%0000000010101010
	DC.W	%0000001010101010
	DC.W	%0000000011101110
FourExits
	DC.B	DIR_NW
	DC.B	DIR_SE
	DC.B	DIR_SW
	DC.B	DIR_NE

*-------------------------------

	XREF	CloseDoor,ShowOut

ManagerFollows

	MOVE.W	#NMANAGER,D0
	GETINFO
	BTST	#7,6(A0)
	BNE.S	90$
	SCN	561		;manager follows
	MOVE.B	#2,D7
	CALL	ShowOut
	MOVE.B	#2,TIMERB(A4)
	CLR.B	TIMER10(A4)	;shut him up
	MOVE.W	#RNINBANK,D1	;go to in the bank
90$
	RET

*---------------------------------

VaultDoor

	MOVE.W	#NVAULTDOOR,D0
	DO	VALID.NP		;has the vaultdoor been destroyed yet?
	BNE.S	90$
	SCN	563			;the vault door looks pretty solid
	POPSP
90$
	RET

*----------------------------------

	XREF	ExplodeBankFlag

QueueHassle

	TEST_B	ExplodeBankFlag(A4)	;queues here?
	BNE.S	10$
	SCN	251
	POPSP				;stay in the bank
10$
	RET

*-------------------------------

LevitateBankNotTill

	CMP.W	#RNTILL1,D1		;destination till?
	BNE.S	LevitateBankInside
	CLR.B	TIMERF(A4)		;short queue goes quiet
	RET				;do it normally

LevitateBankNotInbank

	CMP.W	#RNINBANK,D1		;destination in the bank?
	BNE.S	LevitateBankInside
	CLR.B	TIMER7(A4)		;teller goes quiet
	RET				;do it normally

LevitateBankInside

	TEST_B	FlagInAir(A4)		;levitating?
	BEQ.S	90$			;normal movement if not
	TEST_B	ExplodeBankFlag(A4)	;roof gone?
	BNE.S	10$
	SCN	33			;hit head on roof
	POPSP				;stay where we are
	RET
10$
	SCN	34			;move through the air
90$
	CLR.B	TIMER7(A4)		;teller goes quiet
	CLR.B	TIMERF(A4)		;short queue goes quiet
	RET

LevitateBank

	TEST_B	FlagInAir(A4)		;levitating?
	BEQ.S	90$			;normal movement if not
	TEST_B	ExplodeBankFlag(A4)	;roof gone?
	BNE.S	10$
	SCN	37			;catch feet on sign
	POPSP				;stay where we are
	RET
10$
	SCN	38			;move over bank
90$
	RET


LevitateMoat

	SCN	35			;move over the moat
	RET

	XREF	ThiefHere,FGT_Thief

RemoveThief
* when you have left the jetty the thief disappears for a while...
	TEST_B	ThiefHere(A4)		; Gone already?
	BNE.S	90$
	MOVE.B	#-1,ThiefHere(A4)	; 
	BCLR	#7,FGT_Thief(A4)	; get rid of the append.
	MOVE.W	#NTHIEF,D0		; get the thief
	DO	SETOUT
90$
	RET


OverLake

	SCN	229			;death
	POPSP
	DOGO	DEAD

	END


