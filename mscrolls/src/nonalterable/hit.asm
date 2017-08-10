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
*	@(#)hit.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.HIT/GIVE
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include "verbequ.asm"

	  
      XDEF     NOVERB

      XREF     CONJ,FIGHT,V.BREAK,CHK4PT,IFLIQUID,WHOOPS,P.TN,W.VPSYC,CHKNPC
      XREF     CARRIED,P.MHAV,FIXCHK,YCANT,W.ATMOMT,SP.GIVE,P.PRN,W.THANK
      XREF     P.SUBOB,W.STEADF,PRTTXT,FIXCONT,YCANTNOW,BSFULLSTOP

      IfVerb	HIT

      XDEF	HIT
  
HIT
      IFEQ	THE_PAWN
	
	XREF	SP.HIT

	CALL	SP.HIT

      ENDC
   
	LEA     CONJ(A4),A1
	TEST_W  2(A1)
	BPL     HAMMER
	CMP.W   #INTO,(A1)
	BEQ.S   10$
	CMP.W   #IN,(A1)
	BEQ.S   10$
	BTST    #6,4(A0)			;npc?
	BNE     FIGHT
	BRA     V.BREAK
10$
	CLR.W   D1
	MOVE.W  #-1,(A1)
	DO	CHK4PT				;will never come back


	ENDC
 
  
*--------------------------------
 
	
*--------------------------------
* 
* GIVE
*
*--------------------------------


	IfVerb	GIVE

	XDEF	GIVE
   
GIVE
 
	CALL    IFLIQUID           ;FLOAT UP TO CONTAINER IF A LIQUID
	MOVEQ   #1,D1
	DO	CHK4PT	
	BTST    #6,4(A5)		;give TO a non-npc?
	BNE.S   10$
	MOVE.W  D5,D0
	ST      WHOOPS(A4)         ;NO MORE
	DO	P.TN
	MSGGO	VPSYC            ;GO SEE UR SHRINK
10$
	BTST    #4,3(A0)           ;1=MOVEABLE
	BEQ.S   20$        
 
	DO	CHKNPC          ;ABLE?
	DO	CARRIED	        ;MUST HAVE IT
	BNE     P.MHAV          ;NO

	DO	FIXCHK          ;NOT IF TIED
	BNE.S   11$             ;not OK

* let's consider whether the NPC is smaller than the object 
* this will stop us giving the tub to the maggot (crash) etc

	MOVE.B	2(A0),D1	;get size of object we're giving away
	LSR.B	#4,D1
	MOVE.B	2(A5),D2	;and size of the NPC
	LSR.B	#4,D2
	CMP.B	D2,D1		;if NPC > object then OK
	BMI.S	12$		;MI => ok
11$
	DOGO	YCANTNOW 	;NOT AT THE MOMENT
12$
	CALL    SP.GIVE
	DO	FIXCONT		;if contained!
	MOVE.W  D5,8(A0)
	MOVE.B  #1,6(A0)
	BSET    #6,5(A0)  	;SET AS GIVEN
	MOVE.W  D5,D0
	MOVE.L  A5,A0
	DO	P.PRN
	MSG	THANK
	CALL	P.SUBOB
	DOGO	BSFULLSTOP
  
20$
	DO	P.TN
	MSGGO	STEADF

	ENDC


*********************************
*
*   NOVERB
*
*  VERBS AS YET N/A COME HERE
*
*********************************

NOVERB

      PRINT    <'NO CAN DO!'>
      RET


	IfVerb	HIT

	XDEF	HAMMER


HAMMER   EQU NOVERB


	ENDC




      END
