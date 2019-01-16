*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   specialsm.asm
*
*--------------------------------


	include	"equates.i"
	include	"verbequ.i"
	include	"nounequ1.i"
*	include	"nounequ2.i"
	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
	include	"scoreref.i"


*--------------------------------
	XDEF	SP.MELT
	XREF	SP.WARM,DO.SCORE

SP.MELT
	BRA	SP.WARM			;For plastic key
* NB assumes SP.WARM only copes with key

;**********************************
	XDEF	SP.MOVE.

*	XREF	SP3.FLAG,TBL.MOVE 

SP.MOVE.
 
	CALL_S	Do_Dirt			;Cope with moving the dirt
	BNE.S	90$
	POPSP
90$
	RET


*----------------------------------------



************************************************************************
*	This stuff is unused in MAGIC				       *
************************************************************************
*	ST	SP3.FLAG(A4)        ;FLAG TO SUPPRESS SCAN'S MESSAGES
*	PUSH_L  D0/D1/D5/A0/A5/A6
*
*	LEA     TBL.MOVE(A4),A6
*10$
*	CMP.W   (A6)+,D0
*	BEQ.S   30$
*20$
*	TEST_L  (A6)+
*	BNE.S   10$
*	
*	SETNE
*	BRA.S   40$	            ;NOTHING REVEALED
* 
*30$    
*	BSET    #7,(A6)             ;TEST TO SEE IF ALREADY DONE+SET FLAG
*	BNE.S   20$
*      
*	MOVE.L  (A6),D7
*	AND.L   #$7FFFFFFF,D7           ;MASK OUT FLAG
*	JSR     0(A4,D7.L)          ;GOTO SPECIAL (A6 INTACT)
*40$:
*	PULL_L  D0/D1/D5/A0/A5/A6
*	BNE.S	SP3.RET			; Zero set = special DONE!
*	POPSP
*SP3.RET
*	CLR.W   SP3.FLAG(A4)
*	RET

*-----------------------

	XDEF	Do_Dirt

* Get here if we try to move the dirt mound

	XREF	DirtGone,ODT_DIRTMOUND,SEATED,SEATED_RELATOR
	XREF	DESITTABLE

Do_Dirt

	CMP.W	#NDIRTMOUND,D0		;Refering to dirt?
	BNE.S	90$
	BTST	#7,ODT_DIRTMOUND(A4)	;What state is dirtmound in?
	BEQ.S	10$			;=> worms been to work
	SCN	572
* It's too tightly packed for you to make an impression
	BRA.S	80$

10$
	SCN	571
* You clear the loose dirt away with little effort, revealing a passage to
* the east.

	DO	DESITTABLE		;In case summit is on it
	DO	SETOUT			;Lose the dirt
	CLR.W	SEATED(A4)		;In case he's sitting on it!
	CLR.W	SEATED_RELATOR(A4)	;Hmm, shouldn't desittable suss this?
	ST	DirtGone(A4)		;Tell the world
	Connect	STEPS1,CONNECTING_PASSAGE,E	;let em through
	SCORE	CLEARDIRT
80$
	SETEQ_RET			;Flag to caller

90$
*	SETNE				;NE if we get here anyhow
	RET


;**************************************
	XDEF	SP.MIX 
SP.MIX
	RET
;**************************************





;********************************************************************
;* The 'meddle' mechanism lets you have objects that can be examined
;* listened to, read,etc, but which you cannot remove from their position
;* or otherwise destroy. Each one can have a message printed when an attempt
;* is made to get it, or similar. The message gives a sort of 'cop-out' as
;* to why you shouldn't/couldn't get it. Essentially, 'super-scenery' nouns.
;*
;*

*	XDEF	SP.MEDDLE
*
*	XREF	P.SCN
*
*LastMessage	SET	0
*
;* This lays down the 'meddle' list
;*

*Meddle	MACRO		; NounName,MSGNUM
*
*	IFNE	LastMessage-\2
*LastMessage	SET	\2
*	DC.W	\2+HI	; New message type
*	ENDC
*
*	DC.W	N\1	; Noun number
*
*	ENDM
*
*
*	XREF	VERB
*

*SP.MEDDLE
*
*	BTST	#4,6(A0)		; Only inseparable objects can
*					; be 'meddle' protected
*	BEQ.S	99$			;=> Not inseperable - Exit

* Suss if the verb used to reference an object is a 'MeddleVerb'.
* Note - if the 'MeddleList' ends up shorter than the 'MeddleVerbs'
* list then re-arrange the order in which we decide whether we're
* going to do anything or not, ie test the 'MeddleList' before
* the relevant verbs - Paul.

*	LEA	MeddleVerbs.LST(PC),A1
*	MOVE.W	VERB(A4),D1
*5$
*	MOVE.W	(A1)+,D2
*	BMI.S	99$			;=> end of list, no match - Exit
*	CMP.W	D2,D1			;A verb we're interested in?
*	BNE.S	5$
*

* Roddy's original code from here on down.....

*	LEA	MeddleList(PC),A1
*10$:
*	MOVE.W	(A1)+,D2		; Get Word
*	BPL.S	20$
*	AND.W	#$7FFF,D2		; Lose high bit...
*	BEQ.S	99$			; End of Table
*	MOVE.W	D2,D1
*	BRA.S	10$
*20$:
*	CMP.W	D2,D0
*	BNE.S	10$
*
*	DO	P.SCN
*	SETNE
*	RET

* Not meddle-protected or verb not valid gets here
*99$:
*	SETEQ
*	RET

*
* List of verbs which meddle is interested in - be careful which ones go
* here because it would be silly for this code to cope with things like
* 'listen to the roses'
* Basically, if a noun in the MeddleList table is referenced using ANY
* of the following verbs then the SCN # in MeddleList gets printed.
* Paul.

*DefMeddleVerb	MACRO		*verb
*
*	DC.W	VN\1
*
*	ENDM


*MeddleVerbs.LST
*
*	DefMeddleVerb	BUY
*	DefMeddleVerb	PURCHASE
*	DefMeddleVerb	BREAK
*	DefMeddleVerb	SMASH
*	DefMeddleVerb	DAMAGE

*	DefMeddleVerb	CATCH		;This game doesn't have catch (yet)

*	DefMeddleVerb	DRINK
*	DefMeddleVerb	EAT
*	DefMeddleVerb	GIVE
*	DefMeddleVerb	GET
*	DefMeddleVerb	TAKE
*	DefMeddleVerb	CARRY
*	DefMeddleVerb	JOIN
*	DefMeddleVerb	FIX
*	DefMeddleVerb	TIE
*	DefMeddleVerb	UNTIE
*	DefMeddleVerb	LIFT
*	DefMeddleVerb	RAISE
*	DefMeddleVerb	LOWER
*	DefMeddleVerb	LEAVE
*	DefMeddleVerb	MELT
*	DefMeddleVerb	MOVE
*	DefMeddleVerb	PUSH
*	DefMeddleVerb	PULL
*	DefMeddleVerb	SHIFT
*	DefMeddleVerb	PUT
*	DefMeddleVerb	INSERT
*	DefMeddleVerb	FIT
*	DefMeddleVerb	SLIDE
*	DefMeddleVerb	TEAR
*	DefMeddleVerb	RIP
*	DefMeddleVerb	CUT
*	DefMeddleVerb	WEAR
*
*	DC.W	-1			;-1 Word terminator


*
* NB ONLY inseperable items are caught - so there's no point in putting
*    any entries for nouns in this table unless they are inseperable.
*    Paul
*

*MeddleList
*	Meddle	ROSES,80
*	Meddle	FRUIT,80
*	Meddle	WICKER_BASKETS,80
*	Meddle	PLANTS,80
*	Meddle	MAGAZINES,80
*	Meddle	TOMATO_CANS,173
*
*	DC.W	$8000			; List terminator

***********************************************************************



*--------------------------------
*
* SP.MEND
*
;	XDEF	SP.MEND
;SP.MEND

;	RET

	END
