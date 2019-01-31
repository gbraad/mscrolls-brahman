******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SPECIALSE
*
*--------------------------------




	include	"equates.i"
*	include	"verbequ.i"
	include	"nounequ1.i"
*	include	"nounequ2.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
	include	"scoreref.i"
	
	XDEF	SP.EAT,SP.EMPTY,SP.EXIT

	XREF	ROOM,SETOUT,SCRPNT,SEATED
*	XREF	SP.MEDDLE

 
SP.EXIT
	RET

	XREF	CharmInCake

SP.EAT

    IFNE	YCHEAT              ; eat ticket with hands debug mode
	XREF	DBUG.ON 

	CMP.W	#NTICKET,D0
	BNE.S	10$
	CMP.W	#NHAND,D5
	BNE.S	10$
	ST      DBUG.ON(A4)
	POPSP_RET
    ENDC
    
10$
	CMP.W	#NPUDDING,D0		;bread?
	BEQ.S	20$
	CMP.W	#NPUDDING2,D0		;burnt bread?
	BNE.S	90$
20$
	TEST_B	CharmInCake(A4)
	BEQ.S	90$			;charm not in bread
	SCN	779			;you hurt your teeth
	PUSH_L	D0/A0
	MOVE.W	#NCHARM4,D0
	GETINFO
	CLR.L	6(A0)			;I suppose you might as well..
    SCORE   DOBAKERY
	PULL_L	D0/A0			;be carrying it now.
90$:
	RET
 
	XREF	SP.CanoeHole,SP.RowdyInPub
SP.EMPTY
	CALL	SP.CanoeHole		; something into the hole?
	CMP.W	#NFLOUR_SACK,D0		; emptying the sack?
	BNE.S	90$
	SCN	541			; You'd make a terrible mess!
; not sure if it is worth the space needed
;	BRA.S	80$
;10$
;	CMP.W	#NDRINK,D0		; pouring drink on floor in Pub
;	BNE.S	90$
;	CMP.W	#NPUB,ROOM(A4)
;	BNE.S	90$
;	POPSP
;	BRA	SP.RowdyInPub
;80$
	POPSP
90$
	RET

      END


