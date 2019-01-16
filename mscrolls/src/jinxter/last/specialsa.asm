*****************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSA
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


	XDEF	SP.ATTAC,SP.ASK,SP.ASK2

	XREF	TBL.ATTA,P.SCN,DEAD,PRTTXT,DO.SCN

*--------------------------------
*
* sp.attack - catch attempts to murder the population
*
* TBL.ATTA consists of 2 words/entry - noun#,msg#
* if the hi bit of the msg# is set they get to live 
*


SP.ATTAC
	CMP.W	#NGUARDIAN,D0
	BNE.S	90$
	SCN	127
	POPSP
90$
	RET
 
* Any tweeking of the table goes here - EG changing hi (death) bit of MSG#
* or the msg itself (like HJ in The Pawn, kills u second time)
*
*	RET



	XDEF	SP.ASKFOR
	XREF	PostItem,TIMERAFTERE

SP.ASKFOR
	CMP.W	#NBAKER,D0	; Asking baker for a job??
	BNE.S	10$
	CMP.W	#NJOB,D5
	BNE.S	90$
	CALL	AskAboutJob	; Do special code...
	BRA.S	80$
10$:
	CMP.W	#NPOSTMISTRESS,D0	; Trying to get back an object...
	BNE.S	30$			; ... which he's trying to post?
	TEST_B	TIMERAFTERE(A4)		; in the process of posting something?
	BEQ.S	20$
	SCN	380			; "I'm busy!"
	BRA.S	80$
20$
	CMP.W	PostItem(A4),D5		;
	BNE.S	90$
	CLR.W	PostItem(A4)		; Let him have it, but clear the flag
	BRA.S	90$
30$
	CMP.W	#NPUBLICAN,D0		; HMMMM ask publican for handpump
	BNE.S	90$
	CMP.W	#NDRINK,D5		; DRINK,BEER,HANDPUMP effectively same
	BEQ.S	40$
	CMP.W	#NBEER,D5
	BEQ.S	40$
	CMP.W	#NHANDPUMP,D5
	BNE.S	90$
40$
	PUSH_L	D0/A0
	MOVE.W	#NBEER,D0		; is the beer IN (i.e. not bought yet)
	GETINFO
	TST_OUT
	PULL_L	D0/A0
	BEQ.S	90$			; IN, so let ASKFOR handle it
	DO	P.TN			; 'the publican '
	SCN	382			; 'says, "It ain't free you know!"'
80$
	SETNE_RET
90$
	SETEQ_RET

	XREF	AppliedForJob
SP.ASK
	CMP.W	#NBAKER,D0	; Asking baker...?
	BNE.S	90$
	CMP.W	#NJOB,D5	; about job...
	BEQ.S	10$
	CMP.W	#NBAKERYNOTICE,D5 ; or about job ad.
	BNE.S	90$
10$:
	POPSP
	BRA.S	AskAboutJob		; handles ask about job twice
90$:
	RET

	XDEF	AskAboutJob
	XREF	WeirdAttire

AskAboutJob:
	TEST_B	AppliedForJob(A4) 	; Second time, do other message
	BNE.S	10$
	CALL	WeirdAttire		; Is he wearing flippers etc.
	BEQ.S	5$			; clothes ok so...
	SCN	359
* I'm not giving a job to some1 dressed like that!
	BRA.S	90$
5$
	ST	AppliedForJob(A4)	; He's applied for it now
	SCN	341			; Tell player about it...
	RET
10$
	SCN	543			; i've told u about that!
90$
	RET

; Dunno what this special does!
SP.ASK2
	RET                  ;LEAVE IT IN!!!!

*--------------------------


	XDEF	SP.DOODAH
	XREF	TIMER16,CloudAround,StartCloud,ROOM,FirstCloud

SP.DOODAH

	CALL_S	Do_Aqua			;Coz CAST needs to call it too
	BEQ.S	90$			;Nothing done, back to verb

*********************
*
* Anita, moved this into Do_Aqua so that it works with CAST too - Paul
*
* okay we have got some rain....
*	MOVE.W	ROOM(A4),D1
*	MOVE.B	#1,CloudAround(A4)	; the cloud is around
*	ST	FirstCloud(A4)
*	MOVE.B	#5,TIMER16(A4)		; expiry date (for cloud)
*	POPSP
*	BRA	StartCloud		; go and put it there..
********************

	POPSP
90$
	RET



*---------------------

	XDEF	Do_Aqua
	XREF	TypeOfWeather,WT_WRAIN,StartRain,SpellWorks

* This has been factored from SP.AQUA coz of CAST hassles

Do_Aqua

	MOVE.W	#NCHARM5,D0
	CLR.W	D5				;Hmm, this is a singv
	CALL	SpellWorks			;got it?
	BNE.S	80$				;No chance matey!
	BSET	#WT_WRAIN,TypeOfWeather(A4)	;Make it rain!
	BNE.S	90$				;=> already raining
	MOVE.W	#NWRAIN,D0
	GETINFO					;StartRain expects this
	CALL	StartRain
	TEST_W	D6				;Did we see the effect?
	BNE.S	10$				;=> yes
	SCN	373				;Else this msg...
* Nothing obvious happens.

10$
* okay we have got some rain....
	MOVE.W	ROOM(A4),D1
	MOVE.B	#1,CloudAround(A4)	; the cloud is around
	ST	FirstCloud(A4)
	MOVE.B	#5,TIMER16(A4)		; expiry date (for cloud)
	CALL	StartCloud		; go and put it there..

80$
	SETNE_RET
90$
	SETEQ_RET

*-------------------------


	XREF	SP.LiftPhone

	XDEF	SP.ANSWER

SP.ANSWER
	CALL	SP.LiftPhone
	BEQ.S	90$
	POPSP
90$
	RET


*------------------------------


	END
