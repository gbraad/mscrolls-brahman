*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSI
*
*--------------------------------


	include	"equates.i"
*	include	"verbequ.i"
*	include	"nounequ1.i"
*	include	"nounequ2.i"
*	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
*	include	"scoreref.i"

;**************************************
	XDEF	SP.INVENT,SPA.INVENT

	XREF	BoozedUp

BaseBoozeMsg	EQU	408

SP.INVENT
	RET

SPA.INVENT
	CLR.W	D1
	MOVE.B	BoozedUp(A4),D1		; How pissed is he?
	BEQ.S	90$
	ADD.W	#BaseBoozeMsg-1,D1
	DO	P.SCN			; Tell him his booze level
90$:
	RET

      END


