***************************************************************
*
*	%W%
*	%G%
*
***************************************************************
*
*
* scenrw.asm - scenario read/write bits
*

* included by data1.asm

	IFND	VNLOOK
	include	"verbequ.asm"
	ENDC


* Guild of Thieves !!!!!!!!!!!!!!!!!!!!!


	XDEF	TBL.ATTA,TBL.MOVE
	XDEF	FlagGK1,MoorFlag,FlagLeftBoat,GumFlag,FlagVoodoo,FlagFell
	XDEF	FlagMillStopped,FlagDBlowered,Weigh_Flag
	XDEF	TypeOfWeather,WeatherTime,FlagRaceDue
	XDEF	FlagSnake,FlagBear,FlagSpider,ThiefHere
	XDEF	OtherDescTable,FirstGlanceTable
	XDEF	TimeInGDN1,FlagTollGate,Lucky,Brokemirror
	XDEF	FlagRatRace,Bet_Stake,Bet_Rat,Bet_RatList,FGT_Cheque
	XDEF	FGT_Gem,FGT_Lute,FlagLute
	XDEF	FGT_Die1,FGT_Die2,FGT_Die3,FGT_Die4
	XDEF	FlagBeam,FlagMynah,SuccLife,FlagSucc
	XDEF	FlagBedup,FlagPiconwall
	XDEF	FlagGem,UWClock
	XDEF	FGT_Bed2,FlagGotBoots,FGT_Cage,FGT_System1,FGT_System2
	XDEF	FlagRuby
	XDEF	Flooded,Flooding,FloodClock,FGT_Steps,FGT_Junk
	XDEF	FlagChampers,BombRoom,FGT_Bars
	XDEF	BooksFlags,FlagFish
	XDEF	FlagChewing,SpotBits,LastThrow
	XDEF	OldScore,CardOut,FlagRRAnnounced
	XDEF	BirdText1,BirdText2,BirdText3,BirdKnowsHooray,BirdTextNo
	XDEF	BankedTreasure,TellerMessage,FlagInAir,FlagRoomDone
	XDEF	ManagerMessage,GoOfficeFlag,ExplodeBankFlag
	XDEF	FGT_Kbank,RoomMod,RoomDirStore,BankVisitCount
	XDEF	FGT_Thief,FGT_WaterFall,FeedMacawFlag,RubbishFlag
	XDEF	FGT_TopFall

*--------------------------------
*
* TBL.READ - table of objects which may be read
*
* format:
*
*	DC.W	noun#,scn msg#		;scenario msg# is used
* OR
*	DC.W	noun#,noun2#+HI		;so desc of second noun# is used
*
*	DC.L	0			;end of table
*
*

	XDEF	BankNotice
	XDEF	TBL.READ

TBL.READ
*	MFT_Scen	DETAILS,229
*	MFT_Scen	ALTAR,229	; poem on altar
* The above no longer relevant since statue praying puzzle gone -Paul

	MFT_Scen	BOARD,164
	MFT_Scen	BOOK3,88	;book about fossils
	MFT_Scen	BOOK2,155	;potion book
	MFT_Scen	SHEET,472
	MFT_Scen	PLAQUE1,349	;About the Ice snake
	MFT_Noun	PLAQUE2
	MFT_Noun	LUNCHSIGN
	MFT_Noun	BOARD2

	MFT_Noun	HEADSTONE1
	MFT_Noun	HEADSTONE2
	MFT_Noun	HEADSTONE3

	MFT_OtherNoun	GRAVE1,HEADSTONE1
	MFT_OtherNoun	GRAVE2,HEADSTONE2
	MFT_OtherNoun	GRAVE3,HEADSTONE3

*	MFT_Noun	MESSAGE
	MFT_Noun	SIGNPOST
	MFT_Noun	SASH
	MFT_Noun	NOTE		; 5 ferg note.
	MFT_Noun	MAGAZINE2
	MFT_Noun	PAMPHLET
	MFT_Noun	FISHINGSIGN	; No fishing!
	MFT_Scen	BLACKBOARD,471
	MFT_Noun	BLACKBOOKS
	MFT_Noun	CHEQUE
	MFT_Scen	SIGNPOST3,501	;The signpost in four dice room
BankNotice
	MFT_Scen	BNOTICE,517	;bank closed message
	MFT_Scen	BSIGN,526	;'bank of kerovnia

	MFT_End


*********************************************************
*
* TBL.MOVE - info for SP.MOVE. (was SP.MOVE.GET)
*
* table of noun#,addr. of code to cope
*
* format:
*
*	DC.W	noun#
*	DC.L	addr. of code (hassles due to Metacomco/Vax/Amiga assem)
*
* 	..
*
* 	DC.W	0,0,0		;to end...
*

	XREF	SP.MOVE.CHALICE,SP.MOVE.JUNK,SP.MOVE.BOOKS,SP.MOVE.ABSTRACT
	XREF	SP.MOVE.PIPE,SP.MOVE.CHEST,SP.MOVE.STATUE

	ALIGN

TBL.MOVE


	DC.W	NSTATUE
	DC.L	SP.MOVE.STATUE

	DC.W	NJUNK
	DC.L	SP.MOVE.JUNK

	DC.W	NBOOK1
	DC.L	SP.MOVE.BOOKS

	DC.W	NABSTRACT
	DC.L	SP.MOVE.ABSTRACT

	DC.W	NFINGER			#skeleton finger moved ch skel desc
	DC.L	SP.MOVE.CHEST

	DC.W	NCHEST
	DC.L	SP.MOVE.CHEST

	DC.W	0,0,0

*****************************************************************
*
* UntilDisturbedTable - things which have scn msg's in room desc
*

	XDEF	UntilDisturbedTable

UntilDisturbedTable

	DC.W	NLUTE,NMILLER,451		; Don't drop
	DC.W	NGUEST,NSCRUB2,395		; Don't drop in room
	DC.W	NBUCKET,NLOUNGE,432		; Don't drop     
	DC.W	NSTOOL,NROOM4,458		; Dont drop
	DC.W	NMILLER,NMILL1,468		; Dont DROP IN ROOM
	DC.W	NWHITE,NFOURROOM,97		; the white is lighting it..
	DC.W	NGATEKEEPER,NGATEHOUSE

	XDEF	UDT_GKeeper
UDT_GKeeper
	DC.W	30				;will be 29 sometimes

	DC.W	0		;terminator

*********************************************************
*
* FirstGlanceTable - used by LOOK to add bits to the end
*                    of descriptions of nouns the first
*                    time u look at them.
*
*                    if the scn msg# is >$8000 then it is
*                    ALWAYS appended, not just first time
*
* Format:
*		DC.W	Scenario msg#[+HI],noun#
*		 ..
*
*		DC.L	0		;end of table
*
*********************************************************

FirstGlanceTable

FGT_TopFall
	DC.W	HI+173,NTPWFL	;ladder desc at to of waterfall
FGT_WaterFall
	DC.W	HI+168,0	;to be NCAVE1, bottom of waterfall
FGT_Gem
	DC.W	HI+174,NRMUD	; muddy room - about gem over pool
FGT_Thief
	DC.W	HI+441,NBOAT	; Thief in boat.
FGT_Junk
	DC.W	HI+414,0	; fod in second junk room exit.

FGT_Steps
	DC.W	HI+162,NSWAY5	;steps to wine cellar (rats msg)
FGT_Cage
	DC.W	HI+232,NCAGE3
	DC.W	HI+254,NMILL2

FGT_Cheque
	DC.W	HI+255,NCHEQUE	;to be tweeked by SP.BET
FGT_Lute
	DC.W	HI+263,NLUTE	;to be tweeked by setmode/fix(repair)
FGT_Bars
	DC.W	HI+439,NROOM9
*FGT_Case
*	DC.W	HI+473,NCASE	; Jewelled die in clear plastic case

*	DC.W	654,NDRAWBRIDGE
*************************************************************************
*
* DO NOT SPLIT THESE DICE ENTRIES, OR JUGGLE THEM!
* Failure to observe this warning will result in excessive aggro.
*
* The dice each have one spot on intially.
*
FGT_Die1
	DC.W	HI+124,NDIE1
FGT_Die2
	DC.W	HI+124,NDIE2
FGT_Die3
	DC.W	HI+124,NDIE3
FGT_Die4
	DC.W	HI+124,NDIE4
*
*************************************************************************
*
* Picture description is changed to hole desc.
*
FGT_Bed2
	DC.W	HI+374,NBED2	; Painting over head of bed..
FGT_System1
	DC.W	HI+398		; Vandalised drainage system (may gush water)
FGT_System2
	DC.W	0		; Fod #NSYSTEM into second word.

* black+white squares

*	DC.W	HI+427,NBLACKSQUARE
*	DC.W	HI+427,NWHITESQUARE

	DC.W	HI+429,NBLACKSQUAREP
	DC.W	HI+429,NWHITESQUAREP
* rest
	DC.W	HI+583,NREDSQUARE2
	DC.W	HI+429,NREDSQUAREP

	DC.W	HI+585,NYELLOWSQUARE1
	DC.W	HI+428,NYELLOWSQUARE2
	DC.W	HI+429,NYELLOWSQUAREP

	DC.W	HI+585,NORANGESQUARE1
	DC.W	HI+428,NORANGESQUARE2
	DC.W	HI+429,NORANGESQUAREP

	DC.W	HI+427,NGREENSQUARE1
	DC.W	HI+429,NGREENSQUAREP

	DC.W	HI+427,NBLUESQUARE1
	DC.W	HI+428,NBLUESQUARE3
	DC.W	HI+429,NBLUESQUAREP

	DC.W	HI+427,NINDIGOSQUARE1
	DC.W	HI+428,NINDIGOSQUARE2
	DC.W	HI+584,NINDIGOSQUARE3
	DC.W	HI+429,NINDIGOSQUAREP

	DC.W	HI+428,NVIOLETSQUARE1
	DC.W	HI+428,NVIOLETSQUARE2
	DC.W	HI+429,NVIOLETSQUAREP

FGT_Kbank
	DC.W	HI+145,0		;exploding changes to NKBANK

	DC.L	0		;end of table
 
*********************************************************
*
* OtherDescTable -   used by LOOK to potentially replace
*		     the normal description with the 
*		     Scenario message number in this one.
*
*
* Format:
*		DC.W	Scenario msg#,noun#
*		 ..
*
*		Zero Terminated!
*
**********************************************************

ODTMacro	MACRO	*msg#,noun#
		DC.W	\1
ODT_\2		DC.W	N\2+HI
		XDEF	ODT_\2
		ENDM

OtherDescTable

	ODTMacro	252,DOOR7
	ODTMacro	253,MILL2
	ODTMacro	169,RATS
	ODTMacro	416,SKELETON

	ODTMacro	453,INVENTORY	; Noun used is irrelevant - 
					; As it's patched to be the room
					; that gets vaporised.

	XDEF	TILL1Desc,INBANKDesc,FINBANKDesc

TILL1Desc	ODTMacro	143,TILL1
INBANKDesc	ODTMacro	144,INBANK
FINBANKDesc	ODTMacro	144,FINBANK

	ODTMacro	478,BEAR	; Desc. of ex-bear

	XDEF	ODT_Explode
ODT_Explode	EQU	ODT_INVENTORY


	DC.L	0


*************************************************************
*
* ReachTable
*
* table of objects which may be reached when seated
* (must also be valid) 
*
* format    Reach   object noun#,seat noun# (or verb # if hi bit set)
*            ..
*           DC.L    -1		;long word -1 terminator
*
	XDEF	ReachTable

Reach	MACRO
	XDEF	RT_\1
RT_\1	DC.W	N\1,N\2
	ENDM

VReach	MACRO
	DC.W	N\1,HI+VN\2
	ENDM

ReachTable

	Reach	UPBUTTON,BED2
	Reach	DOWNBUTTON,BED2
	Reach	NOTE,COUCH		; So you can get it after 
					; finding it, if seated!
	Reach	PIANO,STOOL
	VReach	BEAR,GIVE
	VReach	BEAR,FEED
	VReach	PLAQUE2,READ		;can read plaque in cabinet while
					;sitting on bed
	VReach	MACAW,FEED

	DC.L	-1			;terminator


**************************************************************
*
* NoReachTable
*
* table of objects which u cannot reach (above table, ReachTable, overrides this
*
* verb is one which allows u to reach object
*
* format     NoReach	noun#,verb#
*              ..
*            DC.L       0		;ZERO terminator

	XDEF	NoReachTable

NoReach	MACRO
      IFNC	'\2','-1'
	DC.W	VN\2
	XDEF	NRT_\1\2
NRT_\1\2
      ENDC
      IFC	'\2','-1'
	DC.W	-1
	XDEF	NRT_\1
NRT_\1
      ENDC
	DC.W	N\1
	ENDM

NoReachTable

	NoReach FAKEMILLER,LOOK
	NoReach FAKEMILLER,EXAMINE
	NoReach COCONUT,LOOK
	NoReach COCONUT,EXAMINE
	NoReach	WAX,LOOK
	NoReach	WAX,EXAMINE
	NoReach	WAX,MELT
	NoReach WAX,ILLUMINATE
	NoReach SKY,LOOK
	NoReach SKY,EXAMINE

*
* The rats get fodded from this table when player enters cellar,and restored
* on leaving

	XDEF	NoReachRats

NoReachRats
	NoReach RATS,LOOK
	NoReach RATS,EXAMINE
	NoReach RATS,FEED

* Please Make sure that the GEM is ALWAYS at the end of this table
* as a long word of zero gets fodded in!

	XDEF	GemReachStart

GemReachStart

	NoReach GEM,LOOK
	NoReach	GEM,EXAMINE
	NoReach	GEM,MELT

GemReachEnd

	DC.L	0

*--------------------------------
*
*  TBL.ATTA
*
* FORMAT:     /NOUN#,/MSG#
*
* IF MSG# HIGH BIT IS LOW U DIE!
*
*--------------------------------
  
 
TBL.ATTA

	DC.W	NTHIEF,2		;thief is not too patient
	DC.W	NGRAVEDIGGER,18		;nobbled with a spade
	DC.W	NMILLER,42+HI		;+code to throw u out of mill
	DC.W	NSPIDER,114		;Spider kills you
	DC.W	NGUEST,399		;frail old ex-beserker nasty
	DC.W	NGATEKEEPER,14		;chucked in moat
					; Morphy Richards strike again
	DC.W	NBEES,493+HI		; Bees in temple garden
	DC.W	NFLIES,493+HI		; Live flies, not dead ones!
* snake
	DC.W	NSNAKE,113		;last msg from timed death
	DC.W	NMANAGER,40		;manager's logic defeats you
	DC.W	NBIRD3,91+HI		;bird eats itself
	DC.L    0
 

* misc flags and 'bits'

MoorFlag	DC.B	0	;bit 0=1 => boat close to jetty, else not
FlagLeftBoat	DC.B	0	;NE => have left boat
GumFlag		DC.B	0	;bit 0=1 => have stuck gum to door in maze
				;bit 1=1 => have been given gum

FlagVoodoo	DC.B	0	;bit 0=1 => have knobbled gatekeeper wi effigy
				;bit 1=1 => have seen the body
				;bit 2 used to give a free move with gatekeeper

* Flags for the folding bed special.

FlagBedup	DC.B	0	;NE => folding bed is UP

FlagGK1		DC.B	0	;bit 0=1 => have met gatekeeper
FlagMillStopped	DC.B	0	;bit 0=1 => windmill stopped
FlagDBlowered	DC.B	0	;bit 0=1 => drawbridge lowered
TypeOfWeather	DC.B	0	;up to 8 types of weather
WeatherTime	DC.B	0	;only consider weather every 8 moves
FlagRaceDue	DC.B	0	;NE => race may happen soon (been announced)
FlagBear	DC.B	0	;NE=> bear passive
TimeInGDN1	DC.B	0	;count of moves spent in Temple Gardens
FlagTollGate	DC.B	0	;NE=> gate is open

FlagLute	DC.B	0	;bit 0=1 => lute damaged
				;bit 1=1 => lute fixed again (now N/A)
				;bit 2=1 => lute already offered by miller
				;bit 3=1 => was played in last 2 moves
				;bit 4=1 => sang urfanore in last 2 moves

FlagSnake	DC.B	0	;NE=> snake is trying to kill you!
FlagSpider	DC.B	0	;NE=> The spider is dead!
FlagSucc	DC.B	0	;NE=> feet are numb with succulents
SuccLife	DC.B	0	;0-6 you can use them
FlagMynah	DC.B	0	;-1 => bird was put in cage now dead
				;NE && NOT -1 => bird in cage
FlagBeam	DC.B	0	;-1 = done
				;1 = mirror in beam
FlagGem		DC.B	0	;NE=> Gem is falling

UWClock		DC.B	0	; Clocks as we stay underwater..
Lucky		DC.B	0	; Rubbing horse shoes give luck
Brokemirror	DC.B	0	;Breaking mirror loses luck
FlagRuby	DC.B	0	;bit 0 => have found ruby

FlagGotBoots	DC.B	0	;True when he finds the boots
FlagRatRace	DC.B	0	;bit 0=1 => race is happening in the yard
Bet_Stake	DC.W	0	;noun# of stake money
Bet_Rat		DC.W	0	;noun# of rat u betted on

* Bet_RatList - nouns you can bet on (used to put rats in/out room)


Flooded		DC.B	0	; NE = Cellar completely flooded 
Flooding	DC.B	0	; NE = Flooding in progress
FloodClock	DC.B	0	; Incremented as flooding occurs
FlagChampers	DC.B	0	; Bit 0 = Shaken,Bit 1 = Opened (Can't explode)

BombRoom	DC.W	0	; Room# that's been destroyed!
				; Zero means no room (or room has been visited)
BooksFlags	DC.L	0	;bit n (of LONG word) set when a book is read
CardOut		DC.B	0	; the joker!
FlagFish	DC.B	0	;bit 0 => have caught fish
FlagChewing	DC.B	0	;NE if chewing the gum.
OldScore	DC.W	0	; Score on previous move

BirdKnowsHooray	DC.B	0	;bits 0-2 => text contains 'hooray'
BirdTextNo	DC.B	0	;which text gets taught next

        ;; You can teach mynah bird up to 3 things
BirdText1       DC.L    0       
                DC.L    0
                DC.L    0
                DC.L    0
                DC.L    0

BirdText2       DC.L    0       
                DC.L    0
                DC.L    0
                DC.L    0
                DC.L    0

BirdText3       DC.L    0       
                DC.L    0
                DC.L    0
                DC.L    0
                DC.L    0

                DS.B	8	;hacked 4/9/87 
        
        
BankedTreasure	DC.B	0	;number of treasures banked
FlagFell	DC.B	0	;bit 0 => have fallen out of windmill
TellerMessage	DC.W	0	;which message the teller tells you

Weigh_Flag	DC.B	0	;NE => they have the machine set up.
FlagInAir	DC.B	0	;NE => Is floating about

FlagRoomDone	DC.B	0	;All bits used by 'RandThieves'
FlagRRAnnounced	DC.B	0	;flag set when race announced

ManagerMessage	DC.W	0	;what the manager says

GoOfficeFlag	DC.B	0	;going into the manager's office?
ExplodeBankFlag	DC.B	0	;will levitation work?

RoomMod		DC.W	0	;room modified by levitating
RoomDirStore	DC.L	0	;room data of room
		DC.L	0	;with altered exits
		DC.L	0	;on levitating

BankVisitCount	DC.B	0	;number of visits to manager
ThiefHere	DC.B	0	; -1 => Thief gone walkies

FeedMacawFlag	DC.B	0	;fed coconut to macaw or not

RubbishFlag	DC.B	0	;rubbish due to be thrown out

FlagPiconwall	DC.B	-1	;NE => picture still on wall
Bet_RatList	DC.W	NRAT1,NRAT4,NRAT2,NRAT3,0 ;order matters!!
LastThrow	DC.W	2	; Last number thrown (three)
SpotBits	DC.B	4	; bitmap of blank faces (three is blank)
	ALIGN

        ;; these are to be used one at a time and refined into game
        ;; flags. They exist to keep the save game size constant

        XDEF    MillerWineFlag
        
MillerWineFlag  DC.B    0
SpareFlag2      DC.B    0
SpareFlag3      DC.B    0
SpareFlag4      DC.B    0
SpareFlag5      DC.B    0
SpareFlag6      DC.B    0
SpareFlag7      DC.B    0
SpareFlag8      DC.B    0
SpareFlag9      DC.B    0
SpareFlagA      DC.B    0
SpareFlagB      DC.B    0
SpareFlagC      DC.B    0
SpareFlagD      DC.B    0
SpareFlagE      DC.B    0
SpareFlagF      DC.B    0

      	ALIGN

	include	"msgequ2.asm"
	include	"msgequ4.asm"


	XDEF	GREETCHART
	
	BeSure	M.LASTMSG
	BeSure	M.NT
	BeSure	M.HELLO
	BeSure	MsgSCNBASE

GREETCHART
	
	 DC.W    0,M.LASTMSG+27,MsgSCNBASE+85			;THIEF
	 DC.W    0,M.LASTMSG+28,MsgSCNBASE+450			;MILLER
	 DC.W    0,M.HELLO,M.HELLO				;GATEKEEPER
	 DC.W    0,M.NT,M.NT					;BEAR !?!
	 DC.W    0,MsgSCNBASE+449,MsgSCNBASE+452		;GUEST
	 DC.W    0,M.NT,M.NT					;SPIDER
	 DC.W    0,M.NT,M.NT					;GRAVEDIGGER
	 DC.W    0,M.NT,M.NT					;SNAKE
	 DC.W    0,M.NT,M.NT					;RAT1
	 DC.W    0,M.NT,M.NT					;RAT2
	 DC.W    0,M.NT,M.NT					;RAT3
	 DC.W    0,M.NT,M.NT					;RAT4
	 DC.W    0,M.NT,M.NT					;RATS
	 DC.W    0,M.LASTMSG+28,MsgSCNBASE+450			;FAKEMILLER
	 DC.W    0,M.NT,M.NT					;MACAW
	 DC.W    0,M.NT,M.NT					;BIRD3 ??
	 DC.W    0,M.NT,M.NT					;HOORAY
	 DC.W    0,M.NT,M.NT					;QUEUE1
	 DC.W    0,M.NT,M.NT					;QUEUE2
	 DC.W    0,M.NT,M.NT					;TELLER
	 DC.W    0,M.NT,M.NT					;PEOPLE
	 DC.W    0,M.NT,M.NT					;MANAGER
