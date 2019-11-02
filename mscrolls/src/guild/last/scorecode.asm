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

	include	"equates.asm"
	include	"macros.asm"
	include	"scen.i"
	XREF	MaxScore,FULLSTOP,DO.SCN,P.SCN,ScoringChart
	XREF	PRTNUM,SCORE_
	XDEF	ScoreCode


ScoreCode:

	FIXED_SCN	Score1		;'Your score_ is..'
	MOVE.W  SCORE_(A4),D6

	CMP.W   #MaxScore,D6		;Score_ too much?
	BLS.S   10$			;Nope - OK
	MOVE.W  #MaxScore-1,D6		;Yes - make it MAX -1!
10$
	CALL    PRTNUM			;Print score
	FIXED_SCN	Score2		;'From 
	LEA	ScoringChart(A4),A0
    	MOVEQ	#-1,D2
20$
	ADDQ.W	#1,D2
	CMP.W	(A0)+,D6		;LE => try again
	BGT.S	20$
	MOVE.W	#574,D1
	ADD.W	D2,D1
	CALL	P.SCN			; print possible etc..
	MOVE.W	#MaxScore,D6 
	CALL	PRTNUM
	BRA	FULLSTOP

	END
