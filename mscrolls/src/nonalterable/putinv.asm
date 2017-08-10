* 
* 
*     _    __        _      __                           
*    | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
*    | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
*    | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
*    |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
*                                                        
*  Copyright (c) 2016-2017 Voidware Ltd.  All Rights Reserved.
* 
*  This file contains Original Code and/or Modifications of Original Code as
*  defined in and that are subject to the Voidware Public Source Licence version
*  1.0 (the 'Licence'). You may not use this file except in compliance with the
*  Licence or with expressly written permission from Voidware.  Please obtain a
*  copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
*  it before using this file.
* 
*  THIS IS NOT FREE SOFTWARE.
*  
*  The Original Code and all software distributed under the Licence are
*  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
*  OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
*  WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
*  PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* 
*  Please see the Licence for the specific language governing rights and 
*  limitations under the Licence.
* 
*  contact@strandgames.com
* 
*  This software was originally written and owned By Magnetic Scrolls Ltd.
*  It is being released under a Public Source Licence for historical reference
*  and educational purposes.
*


******************************************************
*
*	@(#)putinv.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
* 
*   SRC.ADV.PUT/INV
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"



      XREF     CONJ,WHOOPS,W.IDFY,CHK4PT,GETDATA,YCANTW,P.BTN,ISAREN,W.NCONT
      XREF     ISARE,W.CLOSED,W.BUT,P.TN,W.AINSID,W.WNTF,YCANT,W.ATMOMT,FULLSTOP
      XREF     FIXCHK,DBS,FIXCONT,W.NINSID,SP.PUT,CARRIED,P.MHAV,W.SEPAWY
      XREF     W.NOW,W.ON,P.SUBV,W.CAR,GETRELN,SUBJECT2,LINE
      XREF     W.WEARNG,SUBJECT,BSFULLSTOP,P.TNSTOP,P.LISTSTOP,YCANTNOW
      XREF     P.NORCH,CHK.OWN

* PUT/ INSERT

	IfVerb	PUT

	XDEF	V.PUT
 
V.PUT

12$
	MOVE.W  CONJ(A4),D1
	BMI.S   15$
	CMP.B   #INTO,D1              ;INTO
	BEQ.S   10$       
	CMP.B   #IN,D1              ;IN
	BEQ.S   10$                ;OK
	CMP.B   #ON,D1              ;ON
	BEQ     V.PUTON
	CMP.B   #ONTO,D1            ;ONTO
	BEQ     V.PUTON
	ST      WHOOPS(A4)
	MSGGO	IDFY
15$
	MOVEQ   #4,D1              ;INTO WHAT?
	DO	CHK4PT	            ;WILL ALWAYS ERROR!   
10$
	TEST_W  D5                 ;SECOND NOUN?
	BEQ.S   15$                ;WHOOPS!

      IFEQ	THE_PAWN

	XREF	SP.PutBefore

* This special gets called before anything gets printed
* to cope with put x in beam 
* and dr anything in pool!

	CALL	SP.PutBefore

      ENDC

	EXG     D0,D5              ;D5=#, D0=ADD
	EXG     A0,A5              ;TOO
	MOVE.B  10(A0),D2          ;DAT
	AND.B   #7,D2              ;CONTAINER?
	BNE.S   20$                ;YES
	ST      WHOOPS(A4)         ;TELL PROCESS TO GIVE UP
	BTST    #2,(A0)            ;BROKEN?
	BNE     YCANTW             ;CAN DO THAT WI IT
	DO	P.BTN
	DO	ISAREN
	MSGGO	NCONT
20$
	MOVE.B  (A0),D1            ;GET ATSTUS
	AND.B   #$60,D1            ;LOCKED OR CLOSED?
	BEQ.S   25$
	ST      WHOOPS(A4)         ;TELL PROCESS TO GIVE UP
	DO	P.BTN	             ;SORRY, CONTAINER IS LOCKED/CLOSED
	DO	ISARE
	MSG	CLOSED           ;BYE
	DOGO	BSFULLSTOP
 
25$
	BTST    #5,6(A5)
	BEQ.S   35$
	CMP.W   8(A5),D0           ;CONTAINED HERE
	BNE.S   35$
	EXG     D0,D5
	MSG	BUT
	DO	P.TN
	DO	ISARE
	MSG	AINSID
	
	EXG     D5,D0
	DOGO	P.TNSTOP
30$
	EXG     D0,D5              ;D0=#
	DO	P.TN
	MSG	WNTF
	DOGO	BSFULLSTOP
35$
	EXG     D0,D5
	MOVE.B  6(A5),D1
	AND.B   #$1E,D1
	BNE.S   38$
	BTST    #4,3(A5)	  ;check movability
	BEQ.S   38$
	DO	FIXCHK             ;IS OBJECT TIED?
	BEQ.S   40$                ;NO, GOTO 40$
38$
	DOGO	YCANTNOW            ;U CAN'T DO THAT TO THE ..
40$
	EXG     D0,D5
	CALL    NESTLOOP	  ;check that d5 in not alread in d0
	BNE	DBS		  ;it may be prefered to say, "not possible"
	CMP.W   8(A0),D5	  ;is the target related to source?
	BNE.S   41$
	BTST	#4,6(A0)	  ;is it part of it?
	BNE     DBS		  ;yes, dissallow (put jeans in pocket)
41$
	MOVE.B  2(A5),D3           ;SIZE&W
	LSR.B   #4,D3              ;SIZE
	BEQ     DBS                ;IF VOID


	SUB.B   D3,D2              ;SPACE LEFT
	BMI.S   30$                ;WON'T FIT
	BEQ.S   30$                ;WON'T FIT
	MOVE.B  10(A0),D3          ;CAP
	AND.B   #$F8,D3            ;REMOVE CAP
	OR.B    D2,D3              ;WITH NEW CAP
	MOVE.B  D3,10(A0)          ;UPDATE
	EXG     A0,A5
	EXG     D0,D5
	DO	FIXCONT            ;FIX VOLUME OF CONTAINER REMOVED FR
	MOVE.W  D5,8(A0)           ;POINT TO CONTAINER  
	BSET    #5,6(A0)           ;SET BIT
	BCLR    #0,5(A0)           ;NOT NOW  A PSEUDO (SNOW)   
	DO	P.TN
	GETINFO
	DO	ISARE
	MSG	NINSID
	CALL    SP.PUT
	EXG     D0,D5
	EXG     A0,A5
	DO	P.TNSTOP

      IFEQ	THE_PAWN

	XREF	SP.PUT2

	CALL	SP.PUT2		;'loud' specials

      ENDC

	RET
 
*--------------------------------

	XDEF	V.PUTON
	XREF	SP.PUTON
	
V.PUTON
	
      IFEQ	THE_PAWN

	CALL	SP.PUTON

      ENDC
	
	TEST_W	D5
	BGT.S	05$
	MOVEQ	#ON,D1
	DO	CHK4PT
05$
	BTST    #3,5(A5)           ;CAN PUT x ON y?
	BNE     10$
	MOVE.L  A5,A0
	MOVE.W  D5,D0
	DOGO	YCANT
10$
	DO	CARRIED	           ;MUST HAVE
	BNE     P.MHAV

	BTST    #4,6(A0)           ;inseperable?
	BNE.S   15$		;OK, stop 'em putting the band on things!
	BTST    #4,3(A0)	;moveable?
	BEQ	YCANT		;sorry

	DO	FIXCHK             ;IS TIED?
	BEQ.S   20$                ;NO, OK
15$
	DOGO	YCANTNOW              ;YOU CAN'T 
20$
	MOVE.B  1(A0),D1
	LSR.B   #4,D1              ;GET HARDNESS
	CMP.B   #1,D1              ;LIQUID?
	BNE.S   21$                ;NOPE, OK
	DO	P.TN
	BSET    #7,6(A0)           ;OUT
	BSET    #5,3(A5)           ;TARGET BECOMES WET.
	MSGGO	SEPAWY
21$
	DO	FIXCONT			;in case it's in something
	MOVE.W  #$0100,6(A0)       ;ZAP, IN
	MOVE.W  D5,8(A0)
	DO	P.TN
	DO	ISARE
	MSG	NOW
	MSG	ON
	MOVE.W  D5,D0
	DOGO	P.TNSTOP


	ENDC

*--------------------------------

	IfVerb	INVENTORY

	XDEF	INVENT
	
INVENT

      IFEQ	THE_PAWN

	XREF	SP.INVENT,SPA.INVENT

	CALL	SP.INVENT

      ENDC

	ST      WHOOPS(A4)         ;'I ALL'
	DO	P.SUBV
	MSG	CAR
	MOVE.W  SUBJECT(A4),D2     ;CLR.W  D2   WLOG
	DO	GETRELN	           ;TO A1
	MOVE.L  A1,A2              ;SAVE
10$
	MOVE.W  (A1)+,D0           ;NOUN$
	BEQ.S   30$                ;DONE
	GETINFO
	MOVE.B  6(A0),D3
	CMP.B   SUBJECT2(A4),D3
	BNE.S   20$                ;NO
	BTST    #0,5(A0)           ;PSEUDO
	BEQ.S   10$
20$
	BSET    #7,-2(A1)          ;VOID PREVIOUS NOUN
	BRA.S   10$
30$
	MOVE.L  A2,A0              ;START
	DO	P.LISTSTOP
	MSG	LINE
	DO	P.SUBV
	MSG	WEARNG
	MOVE.L  A2,A1              ;START
40$
	MOVE.W  (A1),D0            ;NOUN#
	BEQ.S   70$
	GETINFO
	BTST    #2,6(A0)           ;WORN?
	BEQ.S   50$
	BCLR    #7,(A1)            ;VALID
	BRA.S   60$
50$
	BSET    #7,(A1)            ;VOID
60$
	ADDQ.L  #2,A1              ;NEXT
	BRA.S   40$
70$
	MOVE.L  A2,A0
	DO	P.LISTSTOP

      IFEQ	THE_PAWN

	CALL	SPA.INVENT

      ENDC

	RET


	ENDC

*------------------------------------------------
*
*  NESTLOOP
*
* checks that a container cannot be accidently nested within
* itself.
*
* entry:
*   put D0 in d5
*
* exit:
*	NE => fail
*
*------------------------------------------------- 

NESTLOOP

	PUSH_L 	D0/D1/D7/A0
	MOVEQ	#0,D7			;set result to ok
	BRA.S	20$
10$
	GETINFO				;to A0
	BTST	#5,6(A0)		;contained?
	BEQ.S	90$			;no, return
	MOVE.W	8(A0),D0		;get container.
20$
	CMP.W	D5,D0			;in itself!!?
	BNE.S	10$			;no, try next
	ST	D7			;non-zero
90$
	TEST_W	D7			;set ne/z flag	
	PULL_L 	D0/D1/D7/A0  		;fetch vars back

	RET

*------------------------------------------------- 

		END
