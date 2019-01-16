*****************************************************
*
* (C) 1987 Magnetic Scrolls LTD
*
* ALL PARTS OF THIS DOCUMENT ARE COPYRIGHT AND 
* CONFIDENTIAL
*
*****************************************************

******************************************************
*
*	@(#)gmverbs.asm	0.0
*	2/6/87
*
******************************************************

		SECTION		"ADV",CODE

; Extra verbs for Green Magic

	include	"equates.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"macros.i"
	include	"verbequ.i"

      XREF     CONJ,W.IDFY,YCANT,W.BECAUS,P.PRN,ISARE,W.CLOSED
      XREF     P.SCN,P.DESC,DO.SCN,MessageFromTable

      IfVerb	DIAL

	XDEF	V.DIAL

	XREF	SP.DIAL
	
V.DIAL
	CALL	SP.DIAL
	RET

      ENDC

      IfVerb	STAY

	XDEF	V.STAY

	XREF	SP.STAY
	
V.STAY
	CALL	SP.STAY
	RET

      ENDC

      IfVerb	FOLLOW

	XDEF	V.FOLLOW

	XREF	SP.FOLLOW
	
V.FOLLOW
	CALL	SP.FOLLOW
	RET

      ENDC

      IfVerb	STRIKE

	XDEF	V.STRIKE

	XREF	SP.STRIKE,HIT
	
V.STRIKE
	CALL	SP.STRIKE
	BRA	HIT

      ENDC
 
      IfVerb	WARM

	XDEF	V.WARM

	XREF	SP.WARM,P.TN,P.BTN,W.LIT,CHK4PT,ISAREN,PRTTXT
	
V.WARM

	MOVE.W	#WITH,D1		; Warm x with y
	DO	CHK4PT
	BTST	#7,(A5)			; 'alight'?
	BEQ.S	90$
	BTST	#7,4(A5)		; and 'lightable'?
	BEQ.S	90$

	CALL	SP.WARM			; check specials.

	AND.B	#$F0,3(A0)		; Lose texture
	OR.B	#10,3(A0)		; make it 'warm'!
	DO	P.TN
	DO	ISARE
	PRINT	<'now warm.^'>
	RET
90$:
	EXG	D0,D5
	EXG	A0,A5
	DO	P.BTN
	DO	ISAREN
	MSGGO	LIT

      ENDC

      IfVerb	DRACMAZZA
	XDEF	V.DRACMAZZA
	XREF	SP.Dracmazza

V.DRACMAZZA
	CALL	SP.Dracmazza
	MSG	PARDON
	RET
      ENDC

      IfVerb	OIL

	XDEF	V.OIL
	XREF	SP.OIL,YCANTWSTOP

V.OIL

	MOVEQ	#WITH,D1
	DO	CHK4PT

	CALL	SP.OIL
	MOVE.W	D5,D0		; so it whinges about the ind. object
	DOGO	YCANTWSTOP

      ENDC

      END
