*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSK
*
*--------------------------------


	include	"equates.i"
*	include	"verbequ.i"
	include	"nounequ1.i"
*	include	"nounequ2.i"
*	include	"roomequ.i"
	include	"macros.i"
	include	"scenmacros.i"
*	include	"scoreref.i"
	
 
;*******************************************
	XDEF	SP.KNOCK
	XREF	TIMERAFTER19,ODT_CMAKERS_WINDOW

SP.KNOCK

	CMP.W	#NPUBDOOR,D0		; Knock on pub door...
	BNE.S	10$
	SCN	338			; We're open!
	BRA.S	80$
10$
	CMP.W	#NCMAKERS_DOOR,D0	;Knock on clockamkers door?
	BNE.S	90$
	MOVE.W	D0,-(A7)
	MOVE.W	#NCLOCKMAKER,D0
	GETINFO
	BSET	#7,6(A0)		;set him out for a bit
	PULL_W	D0			;With flags
	BNE.S	90$			;=> Already out, 'Thud!'
	SCN	786
* "Alright, already," moans a voice from inside the shop. "I'm coming,
* I'm coming." Through the window you see the clockmaker leave his
* workshop and shuffle out into the main shop area.

	MOVE.B	#2,TIMERAFTER19(A4)	;Prime him opening door
	BCLR	#7,ODT_CMAKERS_WINDOW(A4)	;One ODT desc

    ;; there's a fake clockers object that serves to allow look
    ;; outside, but now the door is open, the real shop is local for look/go
    MOVE.W  #NFAKECMAKERS,D0
    DO      SETOUT
    
80$:
	POPSP
90$:    
	RET


*-------------------------

      END


