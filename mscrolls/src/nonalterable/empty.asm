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
*	@(#)empty.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.EMPTY/UNTIE	
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include "verbequ.asm"

      XDEF     F.APART

      XREF     CHK4PT,P.BTN,ISAREN,W.NCONT,TOPLOC,W.CLOSED,V.PUT,SETOUT,LINE
      XREF     GET_CONT,GETDATA,P.TN,DBS,FIXCONT,W.SEPAWY,RETURN,ISARE
      XREF     W.DCONTA,W.NEMPTY,W.WNTF,W.IN,CONJ,FIXCHK,W.CON,W.ANYT,W.FXTO
      XREF     P.FROM,FIXSYS,P.LIST2,W.FAPART,OK,LISWRD,P.TNSTOP,SP.SCORE.NOUN


	IfVerb	EMPTY

	XDEF	EMPTY
	XREF	SP.EMPTY

 
EMPTY
 
	CALL    SP.EMPTY           ;STOP EMPTY BALLOT BOX THINGS
 
	MOVE.W  D0,D6              ;SAVE 1ST CONAINER #
	TEST_W  D5                 ;ADDNOUN?
	BEQ.S   30$                ;NONE
	MOVEQ   #INTO,D1           ;INTO?
	DO	CHK4PT	            ;OK?

	ifeq	1
	MOVE.B  10(A5),D1          ;VOL
	AND.B   #7,D1
	BNE.S   20$
	MOVE.W  D5,D0
	BRA.S	10$
	endc

20$
	BTST	#5,(A5)			;is the target open?
	BEQ.S	40$			;NE => nope - complain
	MOVE.W	D5,D0			;complain about I.O.
	BRA.S	42$
30$
	PUSH_L  D0/A0
	DO	TOPLOC
	MOVE.W  D0,D5
	MOVEQ   #0,D3              ;ON INV OR IN ROOM
	PULL_L  D0/A0
	BTST    #5,6(A0)           ;CONTAINER CONTAINED?
	BEQ.S   40$                ;NOPE
	MOVE.W  8(A0),D5           ;WHERE TO PUT IT
40$
	MOVE.L	6(A0),D3		;in case in room/on inv
	BTST    #5,(A0)            ;CLOSED?
	BEQ.S   45$                ;EQ= NO
42$
	DO	P.BTN	             ;BUT THE XYZZY,.
	GETINFO
	DO	ISARE	             ;'IS
	MSGGO	CLSE               ;'CLOSED.
45$
	DO	GET_CONT
	BCC.S   46$                ;CS = NOT A CONTAINER
	DO	P.BTN
	DO	ISAREN
	MSGGO	NCONT
46$
	MOVEQ   #0,D4              ;COUNT NO. OF OBJS
EMP50
	MSG	LINE			;'cos lots of info produced
	MOVE.W  (A1)+,D0           ;GET OBJ
	BMI.S   EMP50              ;FLAGED OUT
	BEQ.S   70$                ;FINITO
	GETINFO        
 
	MOVE.B  1(A0),D1           ;GET PHYS CHAR
	LSR.B   #4,D1              ;JUST WANT HARDNESS
	CMP.B   #1,D1              ;LIQUID?
	BEQ.S	57$

	TEST_W	D5			;is there a target?
	BEQ.S	10$			;EQ => no, put in room/on inv

	PUSH_L	D0-D7/A0-A6		;I don't trust V.PUT!!

	CALL	V.PUT			;so SP.PUT etc get called

	PULL_L	D0-D7/A0-A6
	BRA.S	60$

10$
	BTST	#4,3(A0)		;moveable?
	BEQ.S	20$
	BTST	#4,6(A0)		;inseperable?
	BEQ.S	30$
20$
	DO	P.TN
	MSG	STEADF
	MOVEQ	#-$7F,D4		;set neg. count
	BRA.S	EMP50
30$
	DO	FIXCONT
	MOVE.L	D3,6(A0)		;use original location of container
	BRA.S	60$

57$
	DO	P.TN	              ;YES - THE XYZ SEEPS AWAY..
	MSG	SEPAWY        
	DO	SETOUT
60$
	ADDQ.W  #1,D4              ;COUNT NOUNS REMOVED
	BRA.S   EMP50              ;NEXT
70$
	MOVE.W  D6,D0              ;RECOVER NOUN # OF 1ST CONTAINER
	GETINFO        
	TEST_W  D4                 ;WAS IT EMPTY?
	BNE.S	80$
	DO	P.BTN	             ;BUT THE XYZ DIDNT CONTAIN ANYTHING
	DO	ISARE
	MSGGO	DCONTA           ;'EMPTY'
80$
	BMI.S	90$			;cos we had a steadfast..
	TEST_W	D5			;did we put it into something?
	BNE.S	90$
	DO	P.TN			;the xyzzy is now empty
	DO	ISARE
	MSG	NOW
	MSG	DCONTA
90$
	RET

 
	ENDC
  
*--------------------------------


	IfVerb	UNTIE

	XDEF	UNTIE
   
UNTIE
	TEST_W  CONJ(A4)
	BMI.S   10$
	MOVEQ   #2,D1
	DO	CHK4PT
10$
      IFEQ	THE_PAWN

	XREF	SP.UNTIE

	CALL	SP.UNTIE

      ENDC

	DO	FIXCHK
	BNE.S   20$
	DO	P.BTN
	DO	ISAREN
	MSG	CON
	MSGGO	ANYT
60$
90$
	BRA     F.APART
20$
	CLR.W   D4
	TEST_W  CONJ(A4)
	BMI.S   70$
	MOVE.W  12(A0),D1
	AND.W   #$3FFF,D1
	BEQ.S   40$
	CMP.W   D5,D1
	BNE.S   40$
	CLR.W   12(A0)
	ADDQ    #1,D4
40$
	MOVE.W  D0,D2
	MOVE.L  A0,A2
41$
	MOVE.W  (A1)+,D0
	BEQ.S   50$
	CMP.W   D5,D0
	BNE.S   41$
	GETINFO        
	CLR.W   12(A0)
	ADDQ.B  #1,D4
	BRA.S   41$
50$
	TEST_B  D4
	BNE.S   60$
	MOVE.W  D2,D0
	MOVE.L  A2,A0
	DO	P.BTN
	DO	ISAREN
	MSG	FXTO
	MOVE.W  D5,D0
	DOGO	P.TNSTOP
70$
	MOVE.W  12(A0),D2
	AND.W   #$3FFF,D2
	BEQ.S   73$
	DO	P.FROM
	CLR.W   12(A0)
	MOVE.W  D2,D5
71$
	MOVE.W  D0,D3
72$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	GETINFO        
	MOVE.W  12(A0),D1            ;THERE IS A BUG HERE (17/3/86)
	AND.W   #$3FFF,D1
	BEQ.S   72$
	CMP.W   D3,D1
	BNE.S   72$
	MOVE.W  D0,D2
	DO	P.FROM
	CLR.W   12(A0)
	BRA.S   72$
73$
	MOVE.W  D0,D5
	BRA.S   71$

	ENDC

*--------------------------------
   
F.APART
  
	MOVE.W  D5,D0
	GETINFO        
	CLR.W   D5
	DO	FIXSYS
	CLR.W   D4
	MOVE.L  A3,A5
	TEST_W  2(A3)              ;ONE?
	BEQ.S   90$
10$
	MOVE.W  (A3)+,D0
	BEQ.S   20$
	GETINFO        
	MOVE.B  1(A0),D0
	LSR.B   #4,D0
	CMP.B   #12,D0             ;FLEX?
	BNE.S   10$
	ADDQ.W  #1,D4
	BRA.S   10$
20$
	TEST_W  D4
	BNE.S   90$
	MOVE.L  A5,A3
30$
	MOVE.W  (A3)+,D0
	BEQ.S   40$
	GETINFO        
	CLR.W   12(A0)
	BRA.S   30$
40$
	MOVE.L  A5,A0
	MOVEQ   #1,D5              ;FLAG
	TEST_W  (A0)
	BEQ.S   90$

	BeSure	M.AND

	MOVE.W  #M.AND,LISWRD(A4)
	CALL	P.LIST2
	MSG	FAPART
90$
	RET

      END

