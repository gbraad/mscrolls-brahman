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
*	@(#)eat.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.EAT/DRINK/TASTE/SMELL
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"

*
* This module contains things to do with EAT
*
* By devious means they all get built or none get built
*

WantEatBits	SET	FALSE

	IfVerb	EAT
WantEatBits	SET	TRUE
	ENDC

	IfVerb	DRINK
WantEatBits	SET	TRUE
	ENDC

	IfVerb	TASTE
WantEatBits	SET	TRUE
	ENDC

	IfVerb	SMELL
WantEatBits	SET	TRUE
	ENDC


	IFNE	WantEatBits



      XREF     DBS,P.FLOO,C.HP,C.STATUS,TIMERA
      XREF     GETNPC,BSFULLSTOP
      XREF     FIXCONT,SETOUT,P.MSG,SCAN,W.TASTES,P.TN,W.SML,W.LKE,P.QN


	XDEF	V.SMELL,V.TASTE,V.DRINK,V.EAT

*
* a randomly usefull bit that gets the edibility of a noun
*

 
EDTS.FNDED
 
	CLR.L   D1
	MOVE.B  11(A0),D1          ;GET EDIBILITY BYTE
	LSR.B   #5,D1              ;GET TOP THREE BITS
	RET

*--------------------------------
	
EDTS.OK

	MOVE.L  (A7)+,D3           ;SAVE RET
	
	BTST    #6,6(A0)           ;ROOM?
	BNE     DBS                ;FAIL
	MOVE.B  2(A0),D1           ;SIZE & WEIGHT
	AND.B   #$0F,D1            ;WEIGHT
	BEQ     DBS                ;DOES NOT APLY 
	BTST    #6,4(A0)           ;an NPC?
	BEQ.S   10$                ;Nope...
	BTST    #1,(A0)            ;you can taste a DEAD npc!
	BNE.S   10$
	                           ;I am assuming d0 has noun number
	DO	GETNPC	            ;returns a3 = data d0 = noun number (needed)
	MOVE.B  2(A3),D1           ;extra NPC data
	AND.B   #$F0,D1
	CMP.B   #$50,D1            ;NPC = unconcious? (no funny looks...)
	BNE     P.FLOO             ;GIVES U A FUNNY LOOK
10$
	MOVE.L  D3,-(A7)           ;OK
	RET

*--------------------------------

	XREF	SP.EAT

V.EAT
	
	CALL    SP.EAT

**** DO NOT INSERT ANYTHING HERE!

	CALL_S  EDTS.OK            ;CHK D0
	CALL_S  EDTS.FNDED
	BEQ     DBS
	BTST	#4,3(A0)		;moveable?
	BEQ	DBS			;no - u cannot eat it!
	MOVE.B  1(A0),D1           ;PHYS CHAR
	AND.B   #$F0,D1            ;GET HARDNESS ONLY
	CMP.B   #$10,D1
	BNE.S   EDTS.CONSUME
	DOGO	DBS	               ;IF LIQUID PRINT DBR


	XREF	SP.DRINK
   
V.DRINK
  
	CALL    SP.DRINK
	CALL    EDTS.OK
	CALL    EDTS.FNDED
	BEQ     DBS
	MOVE.B  1(A0),D1           ;PHYS CHAR        
	AND.B   #$F0,D1            ;GET HARDNESS ONLY     
	CMP.B   #$10,D1    
	BNE     DBS                ;IF NOT LIQUID PRINT DBR
    
   
EDTS.CONSUME

        XREF    CANTSAVE
        
	MOVE.W  D0,-(A7)
	CALL_S  V.TASTE
	MOVE.W  (A7)+,D0
	TEST_B  D1
	BEQ.S   30$
10$
	CMP.B   #4,D1              ;POISON ETC ?
	BMI.S   20$                ;BRANCH IF BAD NEWS
   
	SUBQ.W  #4,D1              ;VALUE OF FOOD
	ADD.W   D1,C.HP(A4)
	BRA.S   30$
20$
	TEST_B  TIMERA(A4)         ;ALREADY POISONED ?
	BNE.S   30$                ;NE= YES, SO NO MORE
	MOVE.W  D1,C.STATUS(A4)    ;D1 = 1,2,3
	MOVE.B  #3,TIMERA(A4)
        BSET    #0,CANTSAVE(A4)    ;bit 0 reserved for fatals
30$
					;IN CASE IT WAS IN CONTAINER 
	DOGO	SETOUT	            	;OUT OF ADVENTURE + ABOVE
 
 
EDTS.DO
 
	MOVE.B  11(A0),D2 
	LSR.B   #5,D2              ;GET EDIBILITY   
	AND.W   #$000F,D2

	BeSure	M.NEDBL			;if not def XREF if

	MOVE.W  #M.NEDBL,D1        ;EDIBILITY MSGS START AT NOT EDIBLE
	ADD.W   D2,D1
	MOVE.W  D2,-(A7)           ;SAVE EDIBILITY
	DO	P.MSG
	CALL    SCAN
	MOVE.W  (A7)+,D1
	RET


V.TASTE
	
	CALL    EDTS.OK
	DO	P.TN
	MSG	TASTES
 
V.TASTE2
 
	BRA.S   EDTS.DO
	
	
V.SMELL

	CALL    EDTS.OK
	DO	P.TN
	MSG	SML   
	CALL    EDTS.FNDED
	BNE.S   V.TASTE2
	MSG	LKE
	DO	P.QN
	DOGO	BSFULLSTOP


	ENDC

      END

