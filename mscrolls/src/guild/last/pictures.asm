******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

************************************************
*
*  PICTURES  module to deal with all the code
*            needed to suss whether a picture
*            is required in this room
*
*
***********************************************



* Guild Of Thieves version beware !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	include "roomequ.i"
	include	"equates.asm"
	include	"macros.asm"

Pict	MACRO	* pictno., room identifier
	DC.B	\1,RN\2
	ENDM


      XDEF     PICTURE_INDEX


PICTURE_INDEX
	
	Pict 	4,BOAT
	Pict	16,SCRUB1
	Pict	16,SCRUB3       ; castle picture
	Pict	15,FOREST4      ; temple picture
	Pict	11,FIELD
	Pict	19,HGROUND      ; mountain picture
	Pict	1,YARD
	Pict	5,LOUNGE
	Pict	2,CORRIDOR1
	Pict	6,ROOM3		;BILLIARD
	Pict	3,ROOM4		;MUSIC
	Pict	10,ACHAM
	Pict	9,TEMPLE
	Pict	14,STAIR2
	Pict	14,STAIR3
	Pict	14,STAIR4
	Pict	7,GDN2		;HOT HOUSE
	Pict	20,SHOP2	
	Pict	18,CEMTR
	Pict	8,RRM           ; skeleton
	Pict	13,FNRM         ; coalroom
	Pict	12,BHSE         ; boathouse
	Pict	17,BLACKSQUARE  ; rainbowroom
	Pict	21,INBANK       ; bank1
	Pict	22,KBANK        ; outside bank
	Pict	23,MANOFFIS     ; bank man
	Pict	24,CRYPT
	Pict	25,KITCHEN
	Pict	26,SUMP
	Pict	27,SWAY3	;homeless picture! tempc
	Pict	28,MILL1
	Pict	29,ORGANROOM

;;  4 pictures from collection
        Pict    31,AVRE         ; macaw
        Pict    30,FOREST3      ; hill picture
        Pict    32,SHRINE
        Pict    33,STABLE
        
	DC.W	-1		; the end.

	END
