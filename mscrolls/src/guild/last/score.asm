******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.SCORE
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

     XDEF   INITG.E,ScoreList

MaxScore	SET	0

SDEF	MACRO	* points,noun
	XDEF	SC.\2
SC.\2	EQU	*-ScoreList
	DC.B	\1
MaxScore	SET	MaxScore+\1
	ENDM

ScoreList

	SDEF	20,CHALICE		;nouns stolen from castle
	SDEF	15,FOSSIL
	SDEF	5,PAINTING1
	SDEF	10,RUBY
	SDEF	5,POT
	SDEF	5,DRESS
	SDEF	13,RING
	SDEF	10,BAG2

	SDEF	5,RHINO
	SDEF	15,PLECTRUM	;nouns stolen from elsewhere
	SDEF	5,BURNER
	SDEF	10,STATUETTE
	SDEF	10,CHIPS
	SDEF	10,GEM
	SDEF	0,DIE5
	SDEF	10,BROOCH
	SDEF	0,CUBE2		; you don't actually score here for it.

	SDEF	5,JETTY		;for getting out the boat
	SDEF	5,MILL1		;getting into the mill
	SDEF	15,CELLAR	;Doing the rats!
	SDEF	17,SUMP		;Statue,etc...
	SDEF	7,LAB		; via the bed
	SDEF	7,SHOP2		;getting into the shop
	SDEF	15,WHITESQUARE	;getting across the rainbow room
	SDEF	5,LIBRARY2	;and getting past the door
	SDEF	10,STPS		;having crossed the coals
	SDEF	30,INBANK	;getting into the bank
	SDEF	15,MANOFFIS	;getting to see Mr. P.
	SDEF	5,TILL1		;getting to talk to tellar


	SDEF	5,HELPMAN	;for helping the man
	SDEF	5,BREAKCOAL	;sussing fossil
	SDEF	8,WINRATRACE	;for winning some money
	SDEF	5,FINDRING	;for finding the ring/opening ball
	SDEF	8,CATCHFISH	;for catching a fish
	SDEF	10,KILLSPIDER	;nobbling spider
	SDEF	5,FEEDMACAW	;giving coconut to macaw
	SDEF	5,CATCHGEM	;getting the gem from the wax
	SDEF	5,MOVEGEM	;for getting it to fall
	SDEF	5,MOVEJUNK	;moving the junk in the junk room
	SDEF	10,BREAKBARS	;sussing the polystyrene bars!
	SDEF	5,TEACHBIRD	;for teaching the bird things
	SDEF	10,KILLSNAKE	;not getting done by the snake
	SDEF	40,GETCUBE	;getting the ivory cube
	SDEF	30,GETDIE	;getting jewelled die
	SDEF	15,OUTMILL	;getting out of the mill without killing lute
	SDEF	15,OUTFOUR	;getting out of the four room
	SDEF	10,LADDER	;for dropping the ladder at waterfall
	SDEF	20,ANTECUBE	;for constructing the anticube
	SDEF	1,WINNING	;well you get one point for winning

             ALIGN

	XDEF	MaxScore,ScoringChart

ScoringChart
	DC.W	MaxScore/9
	DC.W	MaxScore/9*2
	DC.W	MaxScore/9*3
	DC.W	MaxScore/9*4
	DC.W	MaxScore/9*5
	DC.W	MaxScore/9*6
	DC.W	MaxScore/9*7
	DC.W	MaxScore/9*8
	DC.W	MaxScore
    
INITG.E

* end of initial state of game info

            END

