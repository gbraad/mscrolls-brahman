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

	include	"equates.i"
	include	"macros.i"

     XDEF   INITG.E,ScoreList

MaxScore	SET	0

SDEF	MACRO	* points,noun
	XDEF	SC.\2
    XDEF    SCVAL.\2
SC.\2	EQU	*-ScoreList
SCVAL.\2
	DC.B	\1
MaxScore	SET	MaxScore+\1
	ENDM

ScoreList

*	SDEF	1,NULL

	SDEF	5,LEAVEBUS			;leaving bus at right place
	SDEF	5,DEFEATBULL			;Nobbling bull/cutting wire
	SDEF	10,GETOIL			;Separating oil from water
	SDEF	10,OPENMBX			;Open Xam's mbx
	SDEF	5,GETSPECS			;Listening to magpie
	SDEF	10,OPENBHOUSE			;Surprising Leprechaun
	SDEF	10,LAUNCHCANOE			;Launching canoe
	SDEF	10,CLEARDIRT			;Clearing dirt mound
*	SDEF	10,CHARM1			;
*	SDEF	10,CHARM2			;
*	SDEF	10,CHARM3			;May be covered by others
*	SDEF	10,CHARM4			;
*	SDEF	10,CHARM5			;
	SDEF	10,POSTSADDLE			;Posting saddle to station
	SDEF	5,MUFFLEBELL			;Silencing post office bell or scaring PM
	SDEF	10,OPENSAFE			;Opening post office safe
	SDEF	10,ONGIRDER			;Getting to girder
	SDEF	10,GOCLOUD			;Successfully boarding cloud
	SDEF	10,GETTICKET			;Getting train ticket
	SDEF	10,CATCHTRAIN			;Catching the train
	SDEF	10,ARRIVECASTLE			;Landing in castle wi unicorn
	SDEF	10,RELEASEXAM			;wakeing/untying Xam
	SDEF	10,ENTERKITCHEN			;Entering castle kitchen
	SDEF	10,OPENDOME			;Solving sliding block puzzle
	SDEF	5,ASSEMBLEBRACELET		;Assembling the bracelet
	SDEF	10,SEEWITCH			;For getting to witch chamb
	SDEF	20,WINNING			;Lots for winning???
  	SDEF	10,DOBAKERY         ;getting the charm from the bread

             ALIGN

	XDEF	MaxScore

    
INITG.E

* end of initial state of game info

            END

