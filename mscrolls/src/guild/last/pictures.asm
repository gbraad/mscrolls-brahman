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

Pict	MACRO	* pictno., room identifier, version
	DC.B	\1,RN\2,\3
	ENDM


      XDEF     PICTURE_INDEX


PICTURE_INDEX
	Pict 	4,BOAT,0
	Pict	16,SCRUB1,0
	Pict	16,SCRUB3,0       ; castle picture
	Pict	15,FOREST4,0      ; temple picture
	Pict	11,FIELD,0
	Pict	19,HGROUND,0      ; mountain picture
	Pict	1,YARD,0
	Pict	5,LOUNGE,0
	Pict	2,CORRIDOR1,0
	Pict	6,ROOM3,0		;BILLIARD
	Pict	3,ROOM4,0		;MUSIC
	Pict	10,ACHAM,0
	Pict	9,TEMPLE,0
	Pict	14,STAIR2,0
	Pict	14,STAIR3,0
	Pict	14,STAIR4,0
	Pict	7,GDN2,0		;HOT HOUSE
	Pict	20,SHOP2,0	
	Pict	18,CEMTR,0
	Pict	8,RRM,0           ; skeleton
	Pict	13,FNRM,0         ; coalroom
	Pict	12,BHSE,0         ; boathouse
	Pict	17,BLACKSQUARE,0  ; rainbowroom
	Pict	21,INBANK,0       ; bank1
	Pict	22,KBANK,0        ; outside bank
	Pict	23,MANOFFIS,0     ; bank man
	Pict	24,CRYPT,0
	Pict	25,KITCHEN,0
	Pict	26,SUMP,0
	Pict	27,SWAY3,0	;homeless picture! tempc
	Pict	28,MILL1,0
	Pict	29,ORGANROOM,0

;;  4 pictures from collection
        Pict    31,AVRE,0         ; macaw
        Pict    30,FOREST3,0      ; hill picture
        Pict    32,SHRINE,0
        Pict    33,STABLE,0
        
	DC.W	-1		; the end.

	END
