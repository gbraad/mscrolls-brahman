*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSJ
*
*--------------------------------


	include	"equates.i"
*	include	"verbequ.i"
	include	"nounequ1.i"
*	include	"nounequ2.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
*	include	"scoreref.i"

*--------------------------------
*
* SP.JUMP (verb will NOT be VAULT)
*
	XDEF	SP.JUMP

	XREF	SP.GO.N,SCRPNT,VERB,ROOM,DoGuardian,JumpedD0
	XREF	ROOM,TIMERAFTER8,MovePlayer

SP.JUMP
;	CMP.W	#NGARDENGATE,D0		;Jump gate?
;	BNE.S	20$
;	TST_OPEN				;Open?
;	BNE.S	12$			;If so just a 'you succeed'
;	CALL	DoGuardian		;Guardian may not want you to leave!
;	BNE.S	80$
;	MOVE.W	#1,SCRPNT(A4)
;	CMP.W	#RNFRONTGARDEN1,ROOM(A4)	;From the front garden?
;	BNE.S	10$
;	MOVE.W	#RNCOUNTRYLANE1,ROOM(A4)
;	BRA.S	12$
;10$
;	MOVE.W	#RNFRONTGARDEN1,D1
;	DO	MovePlayer
;12$
;	CALL	JumpedD0		;You succeed in jumping the gate
;	BRA.S	80$

20$
	CMP.W	#RNTRAIN_ROOF,ROOM(A4)	;jump from train roof?
	BNE.S	30$
	SCN	520			;you daren't
	BRA.S	80$			;and exit
30$
	CMP.W	#RNPLATFORM_BRIDGE,ROOM(A4)
	BNE.S	35$			;nope
	TEST_W	D0			;nounalready?
	BPL.S	31$			;yes
	MOVE.W	#NLOCOMOTIVE,D0		;set as go there
	BRA.S	35$			;ok
31$
	CMP.W	#NPLATFORM_BRIDGE,D0	;jump off bridge
	BNE.S	35$			;no
	MOVE.W	#NLOCOMOTIVE,D0		;set as go there
35$
	CMP.W	#RNTHEATRE,ROOM(A4)
	BNE.S	40$
	SCN	448
	BRA.S	80$

40$
	BRA	SP.GO.N			;Now check jump down hole etc.

80$
	POPSP
90$
	RET

;***************************************

	XDEF	SP.JOIN

*	XREF	SP.MEDDLE

SP.JOIN
	RET

      END
