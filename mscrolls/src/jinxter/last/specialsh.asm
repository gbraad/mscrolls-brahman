******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SPECIALSH
*
*--------------------------------




	include	"equates.i"
*	include	"verbequ.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
*	include	"scoreref.i"
	include	"scenequ2.i"

;********************************************

	XDEF	SP.HOLD,SP.HOLD_UNDER,SP.HOLD_OVER

	XREF	ROOM,SEATED
	XREF	CONJ,FlagHoldFence,V.WARM

SP.HOLD
	CMP.W	#OVER,CONJ(A4)		; Hold x OVER y
	BEQ	SP.HOLD_OVER
	CMP.W	#UNDER,CONJ(A4)		; Hold x UNDER y
	BEQ	SP.HOLD_UNDER		
	CMP.W	#NFENCE,D0		; trying to hold the fence ?
	BNE.S	30$
	MOVE.W	#NGLOVES,D0		; ... and ...
	GETINFO
	TST_WORNBYPLAYER				; Wearing the gloves?
	BEQ.S	20$
	BSET	#7,FlagHoldFence(A4)	;Not ST coz we wanna set/clr it wiv
					;flags
	SCN	34			; 'you grasp a strand firmly.'
	BRA.S	80$
20$:
	SCN	35			; 'Nasty sharp barbs!'
	BRA.S	80$
30$
* trying to hold onto a weatherman...
	CMP.W	#NRAIN_WEATHERMAN,D0
	BEQ.S	40$
	CMP.W	#NSUN_WEATHERMAN,D0
	BNE.S	90$
40$
* Trying to hold onto a weatherman...?
	CMP.W	#RNON_PLATFORM,ROOM(A4)	; make sure we are in the right room
	BNE.S	90$			; naught we can do.
	MOVE.W	#NBEAM,SEATED(A4)	; sit him on it.
	SCN	291

80$:
	POPSP
90$:
	RET


	XREF	EXGNOUN

; Holding things over candle warms them.
SP.HOLD_OVER

	XREF	Denotation_MATCHES
	CMP.W	#NCANDLE,D5
	BEQ.S	MAKEWARM
	PUSH_L	D0/D5/A1
	MOVE.W	D5,D0			;MemberOfSet takes D0
	LEA	Denotation_MATCHES(A4),A1
	DO	MEMBEROFSET		;Is it a match?
	PULL_L	D0/D5/A1
	BEQ.S	MAKEWARM		;=> yep

	RET

; Ditto candle under things!
SP.HOLD_UNDER
	CMP.W	#NCANDLE,D0
	BEQ.S	95$
	MOVE.L	A1,-(A7)
	LEA	Denotation_MATCHES(A4),A1	;Is it a match?
	DO	MEMBEROFSET			;noun=D0
	PULL_L	A1
	BEQ.S	95$				;=> yep
	RET

95$:
	CALL	EXGNOUN			;Swap direct/indirect

MAKEWARM:
	MOVE.W	#WITH,CONJ(A4)		; Fake a 'with'!	
	POPSP
	BRA	V.WARM			; Warm D0 with D5

;*****************************************************
;
	XDEF	SP.HIT
		
	XREF	CONJ

SP.HIT
	CMP.W	#NHEAD,D0
	BNE.S	90$
	CMP.W	#ON,CONJ(A4)		;hit head on
	BNE.S	90$
	CMP.W	#NWALL,D5		;hit head on wall
	BNE.S	90$
	SCN	90
80$
	POPSP
90$
	RET


*--------------------------------------------------------------
*
* SP.HELP - so u can help random NPC's
*
*
	XDEF	SP.HELP

SP.HELP
	RET

*--------------------------------------
      END


