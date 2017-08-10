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
*	@(#)movefix.asm	2.12
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.MOVE/FIX
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"verbequ.asm"


      XREF     SP.MOVE.,W.BUT,W.GOT,P.TN,YCANT,DBS,W.STEADF,GET_,FIXCHK
      XREF     DO.FIX,P.SUB,W.YSI,W.MVG,UTIL.BS,SCAN,W.NHAP,W.BUT
      XREF     CONJ,CHK4PT,P.FLOO,YCANTW,ADDNOUN,GETDATA,W.ATMOMT,P.BTN
      XREF     ISARE,W.ALRDF,W.YCNDTT,ROOM,P.TNIN,W.MSTBE,PRTTXT
	XREF	P.TNSTOP,FULLSTOP,BSFULLSTOP,YCANTWSTOP,YCANTNOW
      IfVerb	MOVE
 
      XDEF     V.MOVE,MVE.PNT

V.MOVE
 
	CALL    SP.MOVE.
	TEST_W  8(A0)              ;ON INVENT ?
	BNE.S   05$
	BTST    #0,5(A0)           ;PSEUDO ?
	BNE.S   05$
 
	MSG	BUT              ;BUT YOU HAVE
	CALL    PRTTXT   
	DC.W    $8100              ;YOU HAVE/HE HAS
	MSG	GOT
	DOGO	P.TNSTOP
	
05$
	MOVE.B  1(A0),D1
	AND.B   #$F0,D1            ;HARDNESS
	CMP.B   #$10,D1            ;LIQUID ?
	BLS     YCANT
	MOVE.B  2(A0),D1
	AND.B   #$0F,D1
	BEQ     DBS
	CMP.B   #$0F,D1
	BEQ     GET_  
	DO	FIXCHK
	BEQ.S   MVE.PNT
	LEA     MVE.PNT(PC),A6
	BRA     DO.FIX

MVE.PNT

	BTST    #4,3(A0)           ;SH&TEX  MOVEABLE?
	BNE.S   10$
	DO	P.TN
	MSGGO	STEADF
10$
	DO	P.SUB
	MSG	YSI              ;'SUCCEED IN'   
	MSG	MVG
	DO	P.TN
	DO	UTIL.BS
	PRINT   <', '>
	CALL    SCAN
	TEST_B  D7
	BEQ.S   20$
	RET
20$
	MSG	BUT
	MSGGO	NHAP

	ENDC
 
*--------------------------------

	IfVerb	FIX

	XDEF	FIX,V.JOIN

V.JOIN

      IFEQ	THE_PAWN

	XREF	SP.JOIN

	CALL	SP.JOIN

      ENDC

*
* WARNING!! this falls through!
*
  
FIX

      IFEQ	THE_PAWN

	XREF	SP.FIX

	CALL	SP.FIX

      ENDC
 
	MOVEQ	#0,D6			;use as a flag

	BTST    #6,4(A0)           ;NO NPCS MAY BE TIED!!
	BNE     DBS                ;(NOT EVEN THE PRINCESS!!)
	MOVEQ   #AROUND,D1             ;AROUND
	CMP.W   CONJ(A4),D1
	
	BEQ.S   04$

        IfVerb	MEND

      IFEQ	THE_PAWN        

	XREF	SP.MEND

	TEST_W	CONJ(A4)		;with?
	BNE.S	010$
	CALL	SP.MEND			;fix x with y == mend x wi y
        
      ENDC

010$
      ENDC

	MOVEQ   #TO,D1		    ;TO
04$
	DO	CHK4PT
	BTST    #6,4(A5)           ;NPC?
	BEQ.S   01$                ;NOPE
	BTST    #1,(A5)            ;IF DEAD U CAN TIE TO    
	BNE.S   01$                ;OK - SINCE DEAD
	MOVE.W  D5,D0
	MOVE.L  A5,A0              ;BEWARE OF PLURAL PROBS
	BRA     P.FLOO             ;LIVE NPCS GIVE U A FUNNY LOOK
01$
	MOVE.B  1(A0),D1           ;H&S
	LSR.B   #4,D1              ;HI 4
	CMP.B   #12,D1             ;FLEX?
	BNE.S   09$
	TEST_W  6(A0)
	BEQ	10$
	BTST    #0,5(A0)
	BNE	10$
	BRA.S   02$                ;YOU CANNOT DO THAT WI THAT AT THE MOM
 
09$
	MOVE.W  CONJ+2(A4),D2
	BMI     YCANTWSTOP
	MOVE.W  ADDNOUN+2(A4),D2
	EXG     D2,D0
	MOVE.L  A0,A2
	GETINFO
	MOVE.B  1(A0),D3
	LSR.B   #4,D3
	CMP.B   #12,D3
	BNE     YCANTWSTOP
	MOVE.W  8(A0),D3
	CMP.W   8(A2),D3           ;X,Y,Z ALL IN SAME LOC?
	BNE.S   02$
	MOVE.W  12(A0),D3
	AND.W   #$3FFF,D3
	BNE.S   07$
	BTST    #2,6(A0)           ;WORN?
	BEQ.S   03$
02$
	DO	YCANTW
	MSGGO	ATMOMT
03$
	EXG     D0,D2
	MOVE.W  6(A5),D1
	CMP.W	#$1400,D1          ;INSEPERABLE AND WORN ALLOWED!
	BEQ.S	0310$
	AND.W   #$EFFF,D1

	BEQ.S   0310$
	MOVE.W  6(A5),D1     
	AND.W   #$3000,D1          ;CONTAINED AND INSEP ALLOWED
	CMP.W   #$3000,D1          ;MUST BE BOTH
	BNE	FX13
0310$
	MOVE.W  D5,12(A0)
	EXG     A2,A0
	ST	D6
	BRA     FX50
10$
	MOVE.W  12(A0),D1
	AND.W   #$3FFF,D1
	BEQ.S   11$
07$
	DO	P.BTN
	DO	ISARE
	MSG	ALRDF
	DOGO	FULLSTOP
11$
	BTST    #0,5(A5)           ;PSEUDO?
	BEQ.S   FX50               ;YES = FALL THROUGH
	MOVE.B  6(A5),D7
	AND.B   #$FB,D7            ;WORN ALLOWED
	BEQ.S   FX12
FX13
	MOVE.W  D5,D0
	DOGO	YCANTNOW
FX12
	MOVE.W  ROOM(A4),6(A0)
	BSET    #0,6(A0)
	MOVE.W  D5,8(A0)
FX20
	BeSure	M.FXTO

	MOVEQ   #M.FXTO,D1
	DO	P.TNIN             ;THE NOUN IS NOW FIXED TO..
	MOVE.W  D5,D0
	DOGO	P.TNSTOP
	 
FX50
	MOVE.W  6(A5),D1
	CMP.W	#$1400,D1          ;INSEPERABLE AND WORN ALLOWED!
	BEQ.S	FX52
	AND.W   #$EFFF,D1

	BEQ.S   FX52
	MOVE.W  6(A5),D1     
	AND.W   #$3000,D1          ;CONTAINED AND INSEP ALLOWED
	CMP.W   #$3000,D1          ;MUST BE BOTH
	BEQ.S   FX51
	BRA	FX13
FX52
	MOVE.W  8(A5),D1           ;TEST TO SEE IF THEY ARE
	CMP.W   8(A0),D1           ;TOGETHER (SAME LOC)
	BEQ.S   FX51

*--------------------------------
*--------------------------------

	IFNE	THE_PAWN
	
	CMP.W   #NBED,D0
	BEQ.S   FX51               ;!!!!!!!!!!!!!!!!!!!!!!!!!!
	CMP.W   #NBED,D5
	BEQ.S   FX51

	ENDC
     
*--------------------------------
	
	TEST_B	D6
	BEQ.S	10$
	CLR.W	12(A2)

10$
	DO	P.TN
	MSG	MSTBE		;must be
	MOVE.W  D5,D0
	DOGO	P.TNSTOP
FX51
	MOVE.W  D5,12(A2)
	MOVE.W  D5,12(A0)
	BRA.S   FX20


	ENDC

      END
