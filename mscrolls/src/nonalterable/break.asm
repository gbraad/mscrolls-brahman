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
*	@(#)break.asm	2.9
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.BREAK/SMASH/DAMAGE
*
*--------------------------------


	include "equates.asm"
	include	"macros.asm"
	include "verbequ.asm"


      XDEF     BRKSUB,BRKD0,BRKQ

      XREF     CONJ,CHK4PT,CARRIED,P.MHAV,P.FLOO,FIGHT,P.MHAVSTOP
      XREF     DBS,P.SUBV,W.UNABLE,PCOK,P.TN,GETDATA,W.BRKS,GET_CONT
      XREF     LINE,P.SEPA,FULLSTOP

	IfVerb	BREAK

	XDEF	V.BREAK
	XREF	SP.BREAK
        XREF    REMASTER

V.BREAK

	CALL    SP.BREAK
	MOVE.W  CONJ(A4),D1
	CMP.W   #AGAINST,D1            ;AGAINST
	BEQ.S   05$
	CMP.W   #OVER,D1             ;OVER
	BEQ.S   05$
	MOVEQ   #0,D1
	DO	CHK4PT
05$
	EXG     D0,D5
	DO	CARRIED
	BNE     P.MHAVSTOP         ;MUST HAVE ITEM
	EXG     D0,D5
	BTST    #6,4(A5)           ;WITH A PERSON??
	BNE     P.FLOO             ;STRANGE
	BTST    #6,4(A0)           ;ATTACKIG A PERSON?
	BEQ.S   10$                ;NO
	CLR.W   CONJ(A4)           ;INCASE
	BRA     FIGHT              ;ATTACK X W Y
10$
	CALL    BRKSUB
	EXG     D0,D5
	EXG     A0,A5              ;TO CHECK MAIN NOUN
	CALL    BRKSUB             ;PC OF MAIN

        TEST_B  REMASTER(A4)
        BNE.S   90$

        ;; BRKSUB breaks brittle things, but here the strongest
        ;; wins. but we no longer break ordinary things, onlt breakables
        
	MOVE.B  1(A5),D1           ;PC OF X
	AND.B   #$F,D1             ;STR OF X
	MOVE.B  1(A0),D2           ;PC OF Y
	AND.B   #$F,D2
	CMP.B   D2,D1              ;COMPARE STRENGTHS
	BHI     BRKD0              ;Y BREAKS (HUH)
	EXG     D0,D5
	BRA     BRKD0              ;ELSE X BREAKS
90$        
	MSGGO	NHAP               ;nothing happens
        

	ENDC

*--------------------------------
	
BRKSUB
	
*CHECK OUT BREAKING POTENTIAL OF D5/A5
	
        ;;  brittle things break

	MOVE.L  (A7)+,A2           ;RTS, MIGHT NOT RETURN?
	MOVE.B  1(A5),D1           ;GET PC
	LSR.B   #4,D1              ;GET HARDNESS
	BNE.S   08$
	MOVE.W  D5,D0
	DOGO	DBS                ;hardness void
08$
	CMP.B   #1,D1              ;LIQUID?
	BEQ.S   09$                ;YES  
	CMP.B   #$F,D1             ;UNBREAKABLE MEANS NOTHING BREAKS
	BNE.S   10$                ;NO
09$
	DO	P.SUBV
	MSGGO	UNABLE
10$
	CALL    PCOK               ;TOO SOFT or already broken?
	BNE.S   99$                ;YES, ERROR PRINTED
	MOVE.B  1(A5),D1
      	LSR.B   #4,D1              ;get hardness
	CMP.B   #$0A,D1            ;BRITTLE?
	BNE.S   20$
	EXG     D0,D5
	CALL    BRKD0              ;D0 BREAKS (PROPERLY THIS TIME!)
	EXG     D0,D5              ;BAK
        BRA.S   99$                ;do not return
20$ 
	MOVE.L  A2,-(A7)           ;RTS BAK
99$
	RET
	
*--------------------------------
	
BRKD0
	
*BREAK D0
	
	PUSH_L  D1/A0-A2
	GETINFO
	BTST    #$2,(A0)
	BNE.S   20$                ;ALREADY
    
      IFEQ	THE_PAWN
	XREF	SP.BRKD0
	CALL	SP.BRKD0
	BEQ.S	10$		;EQ => we've said something, be quiet
      ENDC

	DO	P.TN
	MSG	BRKS
	DO	FULLSTOP
10$
	CALL    BRKQ
20$
	PULL_L  D1/a0-a2
	RET

BRKQ
	
	PUSH_L  D1/A0-A2
	GETINFO
	BSET    #$2,(A0)
	BNE.S   11$                ;ALREADY
	BCLR    #7,(A0)            ;GOES OUT IF LIT
	BCLR    #7,4(A0)           ;AND NO LONGER LIGHTABLE
	AND.B   #$8F,(A0)          ;ZAP LOCKED/OPEN/CLOSED
	DO	GET_CONT
	BCS.S   11$                ;NOT A CONTAINER
	AND.B   #$F8,10(A0)        ;NO LONGER A  CONTAINER
20$
	MOVE.W  (A1)+,D0           ;GET CONTENTS
	BMI.S   20$                ;N/A
	BEQ.S   11$                ;END OF LIST
	GETINFO
	MOVE.B  1(A0),D1           ;PHYS CHAR
	LSR.B   #4,D1              ;HARDNESS
	CMP.B   #1,D1              ;1=LIQUID
	BNE.S   30$                ;IF SOLID= FALL OUT
	MSG    LINE
	CALL    P.SEPA
	BRA.S   20$
30$
	MOVE.L  A0,A2              ;SAVE DATA ON CUR NOUN
	MOVE.W  8(A0),D0    
	GETINFO                    ;ON D0
	MOVE.L  6(A0),6(A2)        ;SHIFT ENTIRE LOCATION OVER
	BRA.S   20$                ;NEXT
11$
	PULL_L  D1/A0-A2
	RET

*--------------------------------

      END

