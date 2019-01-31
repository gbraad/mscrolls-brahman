******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.SCORECODE
*
*--------------------------------

	include	"equates.i"
	include	"macros.i"
	include	"scen.i"

	XREF	MaxScore,FULLSTOP,DO.SCN,P.SCN
	XREF	PRTNUM,SCORE_,LUCK_
	XDEF	ScoreCode


ScoreCode:

	FIXED_SCN	Score1		;'Your score_ is..'
	MOVE.W  SCORE_(A4),D6

	CMP.W   #MaxScore,D6		;Score_ too much?
	BLS.S   10$			;Nope - OK
	MOVE.W  #MaxScore-1,D6		;Yes - make it MAX -1!
10$
	CALL	PRTNUM			;Print score
	FIXED_SCN	Score2		;Unfortunately you need another..
	MOVE.W	#MaxScore,D6
	SUB.W	SCORE_(A4),D6		;remaining ponts
	CALL	PRTNUM		
	DO	FULLSTOP
	MSG	LINE
	SCN	721
* You feel

; You're a Loony !
;	MOVE.W	#721,D1			;base scn # for luck msg's
;	MOVE.W	LUCK_(A4),D2
;30$
;	ADDQ.W	#1,D1
;	SUBQ.W	#1,D2
;	BPL.S	30$
;	DOGO	P.SCN

	MOVE.W	#722,D1			; really the base msg #
	ADD.W	LUCK_(A4),D1		; 0 <= luck <= 4
	DOGO	P.SCN

	END
