*
* scoreref.asm - included by things than need to use scores
*

SREF	MACRO	* noun/room
	XREF	SC.\1
	ENDM

	SREF	CHALICE		;nouns stolen from castle
	SREF	FOSSIL
	SREF	PAINTING1
	SREF	RUBY
	SREF	POT
	SREF	DRESS
	SREF	RING
	SREF	BAG2

	SREF	RHINO
	SREF	PLECTRUM	;nouns stolen from elsewhere
	SREF	BURNER
	SREF	STATUETTE
	SREF	CHIPS
	SREF	GEM
	SREF	DIE5
	SREF	BROOCH
	SREF	CUBE2		; no points for putting it in the night safe

	SREF	JETTY		;for getting out the boat
	SREF	MILL1		;getting into the mill
	SREF	CELLAR		;Doing the rats!
	SREF	SUMP		;Statue,etc...
	SREF	LAB		; via the bed
	SREF	SHOP2		;getting into the shop
	SREF	WHITESQUARE	;getting across the rainbow room
	SREF	LIBRARY2	;and getting past the door
	SREF	STPS		;having crossed the coals
	SREF	INBANK		;getting into the bank
	SREF	MANOFFIS	;getting to see Mr. P.
	SREF	TILL1		;getting to talk to tellar


	SREF	HELPMAN		;for helping the man
	SREF	BREAKCOAL	;sussing fossil
	SREF	WINRATRACE	;for winning some money
	SREF	FINDRING	;for finding the ring/opening ball
	SREF	CATCHFISH	;for catching a fish
	SREF	KILLSPIDER	;nobbling spider
	SREF	FEEDMACAW	;giving coconut to macaw
	SREF	CATCHGEM	;getting the gem from the wax
	SREF	MOVEGEM		;for getting it to fall
	SREF	MOVEJUNK	;moving the junk in the junk room
	SREF	BREAKBARS	;sussing the polystyrene bars!
	SREF	TEACHBIRD	;for teaching the bird things
	SREF	KILLSNAKE	;not getting done by the snake
	SREF	GETCUBE		;getting the ivory cube
	SREF	GETDIE		;getting jewelled die
	SREF	OUTMILL		;getting out of the mill without killing lute
	SREF	OUTFOUR		;getting out of the four room
	SREF	LADDER		;for dropping the ladder at waterfall
	SREF	ANTECUBE	;for making the anticube
	SREF	WINNING		; for winning the game


             ALIGN
