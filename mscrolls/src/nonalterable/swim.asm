**************************************************************
*
*	%W%
*	%G%
*
**************************************************************


	SECTION	"ADV",CODE

	include	"equates.i"
	include	"macros.i"
	include	"verbequ.i"
	include	"scenequ.i"

*------------------------------------------
*
* V.SWIM
*
*------------------------------------------

	IfVerb	SWIM


    IFND	Version2_V.SWIM			;old version

	XDEF	V.SWIM
	XREF	SP.SWIM,DBS

V.SWIM


	CALL	SP.SWIM
	DOGO	DBS			;no


    ENDC	;!Version2_V.SWIM

*------------------------------------------

    IFD	Version2_V.SWIM			;new vers (10/87)

	XDEF	V.SWIM
	XREF	SP.SWIM,R.NOUN,P.SUBV,DBS,ADVERB
	XREF	GO.N
        XREF    REMASTER

V.SWIM


	CALL	SP.SWIM
        TEST_B  REMASTER(A4)
        BNE.S   01$
        DOGO    DBS                     ; original logic when not remaster
01$
	CALL_S	IsRoomWet
	BEQ.S	10$			;cant swim here
	TEST_W	D0			;dobj specified?
	BPL	GO.N			;yes, try going there
	MOVE.W	ADVERB(A4),D1		;fetch modifier
	BPL.S	20$
	CMP.W	#-1,D1			;null?
	BEQ.S	20$
	BCLR	#15,D1			;fetch direction
	MOVE.W	D1,D5
	SUBQ.L	#4,A7			;go will pop this
	BRA	GOTOGO.OK
10$
	DO	P.SUBV
	MSG	UNABLE
	MSGGO	HERE			;fullstop
20$
	DO	P.SUB
	MSG	SWIMAROUND		;swim around a bit.
90$
	RET

*------------------------------------------

	XDEF	IsRoomWet
	XREF	R.NOUN,SEATED,SEATED_RELATOR

IsRoomWet
	
* Exit	NE<=>wet

	TEST_W	SEATED(A4)
	BEQ.S	10$		;not so
*	CMP.W	#IN,SEATED_RELATOR(A4)	;sitting in something?
*	BEQ.S	90$			;yes, exit as not wet
        SETEQ
        BRA.S   90$                     ; not wet
10$
	PUSH_L	D0/D2/A0
	DO	R.NOUN		; noun# of room ->d0
	GETINFO
	TST_WET				;
	PULL_L	D0/D2/A0
90$
	RET

*------------------------------------------

        XDEF GOTOGO.OK
        XREF GO.OK

GOTOGO.OK
        BRA    GO.OK
        
    ENDC	;Version2_V.SWIM

    ENDC	;V.SWIM



	END
