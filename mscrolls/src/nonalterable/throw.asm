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
*	@(#)throw.asm	2.9
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.THROW/REM
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"

      XDEF     BBRIT

      XREF	TOPLOC
      XREF     CHK4PT,P.TN,W.STEADF,FIXCHK,SP.THROW,CARRIED,P.MHAVSTOP,SETOUT
      XREF     ROOM,W.HITS,W.BUT,W.NHAP,SP.MELT,P.NOUN,W.OUCH,FIXCONT
      XREF     GETDATA,BRKD0,SP.GET,CONJ,GET_,P.BTN,ISAREN,P.MSG
      XREF     VERB,SUBJECT,P.FROM,W.FXPERM,W.TAKOF,P.SUB,SP.UNWEA,P.CNTO
      XREF     W.REMOVE,DO.FIX,ISARE,UNTIE,P.TNSTOP


	IfVerb	THROW

	XDEF	THROW


THROW
	MOVE.W	CONJ(A4),D1		;prep?
	CMP.W	#IN,D1			;throw x in y?
	BEQ.S	10$
	CMP.W	#INTO,D1		;throw x into y
	BEQ.S	10$
	MOVE.W  #11,D1			;at?
10$
	DO	CHK4PT
	BTST    #4,3(A0)           ;MOVEABLE?
	BNE.S   20$
	DO	P.TN
	MSGGO	STEADF
20$
	DO	FIXCHK
	BNE     THR.MANY
 
THR.PNT
 
	CALL    SP.THROW
	DO	CARRIED
	BNE	P.MHAVSTOP
	DO	SETOUT	              ;FIX TIE& SO ON
	CLR.W   6(A0)                ;HB
	MOVE.W  ROOM(A4),8(A0)       ;IN ROOM
	BTST    #6,4(A5)             ;NPC?
	BEQ.S   10$
	BTST    #1,(A5)              ;DEAD
	BEQ.S   60$                  ;OUCH
10$
	CLR.W   D7
	DO	P.TN
	MSG	HITS
	EXG     D5,D0
	DO	P.TN
	CALL    BBRIT               ;BREAK IF BRITTLE
	EXG     D5,D0
	CALL    BBRIT               ;BREAK IF BRITTLE
	TEST_W  D7                  ;ANY BROKEN?
	BNE.S   90$                 ;YES, EXIT
    
	MSG	BUT
	MSGGO	NHAP 
60$
	EXG     D0,D5
	CALL    SP.MELT             ;SNOWMAN
	DO	P.NOUN
	MSG	OUCH
90$
	RET
 
THR.MANY
 
	LEA     THR.PNT(PC),A6
	BRA     DO.FIX


	ENDC

*--------------------------------
	
BBRIT
	
*BREAK D0 IF BRITTLE
	
*EXIT: D7 !=0 IF BRITLE
	
	PUSH_L  D1/A0
	GETINFO
	MOVE.B  1(A0),D1        ;GET H&S
	LSR.B   #4,D1           ;HARDNESS
	CMP.B   #$A,D1          ;BRITTLE?
	BNE.S   90$             ;NO, EXIT
	CALL    BRKD0           ;BREAK IT
	MOVEQ   #1,D7           ;ERROR
90$
	PULL_L  D1/A0
	RET

*--------------------------------
	
	
	
	IfVerb	REMOVE

	XDEF	REMOVE
  
REMOVE
 
	CALL    SP.GET                   ;STOP SNOW,G F BALLOT BOX ETC..
 
	BTST    #4,3(A0)                 ;MOVEABLE?
	BNE.S   10$
	DO	P.TN
	MSGGO	STEADF
10$
	MOVE.W  D0,D2                    ;SAVE
	MOVE.L  A0,A2                    ;& ADDR
	MOVE.W  D5,D0
	MOVE.L  A5,A0
LB2
	TEST_W  CONJ(A4)  
	BMI     50$
	MOVEQ   #2,D1
	DO	CHK4PT
11$
	TEST_B  6(A2)                    ;AN IS LOC?
	BNE.S   20$                      ;NO
	MOVE.W  12(A2),D1                ;SEE IF TIED
	AND.W   #$3FFF,D1                ;SUPERFLOUS?
	BNE     GOTO_UNTIE               ;YES ITS TIED - UNTIE
14$
	MOVE.W  #-1,CONJ(A4)         ;KILL 'FROM' OTHERWISE GET CALLS REMOVE!
	MOVEQ   #0,D5                    ;NO ADD NOUN
	MOVE.W  D2,D0                    ;GET BACK PRIMARY NOUN
	GETINFO                          ;AND DATA
	BRA     GET_                     ;USE GET
20$
	CMP.W   8(A2),D0                 ;ON SPEC NOUN
	BEQ.S   30$                      ;YES
	BTST    #0,6(A2)                 ;REL TO ANOTHER
	BEQ.S   21$

	BeSure	M.ON

	MOVE.W  #M.ON,D3
22$
	EXG     D2,D0
	DO	P.BTN
	MOVE.L  A2,A0
	DO	ISAREN
	MOVE.W  D3,D1
	DO	P.MSG
	EXG     D2,D0
	DOGO	P.TNSTOP
21$
	BTST    #5,6(A2)                 ;IN ANOTHER
	BEQ.S   27$                      ;HUM!

	BeSure	M.IN

	MOVE.W  #M.IN,D3
	BRA.S   22$
 
27$
	BeSure	M.CONTO

	MOVE.W  #M.CONTO,D3
	BRA.S   22$
30$    
	CALL    RMFIX1
32$
	BTST    #4,6(A2)                 ;INSEP
	BNE     REM1
	MOVE.L  A2,-(A7)
	DO	FIXCONT
	MOVE.L  (A7)+,A2
	DO	TOPLOC
	MOVE.W  D0,8(A2)
	CLR.W   6(A2)
	CMP.W   #VNGET,VERB(A4)          ;GET FROM..
	BNE.S   33$
	MOVE.W  SUBJECT(A4),8(A2)
33$
	RET
50$
	MOVE.W  12(A2),D1
	AND.W   #$3FFF,D1
	BNE     GOTO_UNTIE
*       BEQ.S   51$
*       MOVE.W  D2,D0
*       MOVE.W  D1,D2
*       DO	P.FROM
*       MOVE.W  D0,D2
*       BRA     RMFIX1

51$
	MOVE.W  D2,D0                    ;FOR GET
	MOVE.B  6(A2),D6
	BTST    #2,6(A2)                 ;WORN?
	BNE.S   REMCLTH
	AND.B   #$3F,D6                  ;NOT ROOMS
	BEQ     14$  
	MOVE.W  8(A2),D2
	DO	P.FROM
	MOVE.W  D0,D2
	BRA.S   32$

REM1

	DO	P.TN
	MOVE.L  A2,A0
	DO	ISARE
	MSGGO	FXPERM

REMCLTH

	MOVE.L  A2,A0
	DO	P.SUB
	MSG	TAKOF
	DO	P.TNSTOP
	CLR.W   6(A2)
	MOVE.W  SUBJECT(A4),8(A2)
	BRA     SP.UNWEA

RMFIX

	CMP.W   D0,D1
	BEQ.S   RMFIX1
	MOVE.W  D1,D0
	BRA     P.CNTO

RMFIX1

	CLR.W   12(A2)
	DO	P.SUB
	MSG	REMOVE
	MOVE.W  D2,D0
	DOGO	P.TNSTOP


********************************
*
* Put back DO/A0 and D5/A5
* so untie does work for us
*
********************************


GOTO_UNTIE

	MOVE.W   D0,D5
	MOVE.L   A0,A5
	MOVE.W   D2,D0
	MOVE.L   A2,A0

	BRA      UNTIE          ;off to verb routine

	ENDC

 
*--------------------------------
 
 

      END

