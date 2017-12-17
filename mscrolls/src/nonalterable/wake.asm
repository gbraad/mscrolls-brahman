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
*	@(#)wake.asm	2.16
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.WAKE/RAPE/MEND/LEVER/SHAKE
*
*--------------------------------

	include	"equates.asm"
	include	"verbequ.asm"
	include "macros.asm"

      
      IFEQ	THE_PAWN
	XREF	SP.SHAKE,BSFULLSTOP,P.VERB,SETOUT
      ENDC

      XREF     DBS,GETNPC,P.BTN,ISAREN,W.ASLEEP,P.PRN,W.WAKES,P.SUBV
      XREF     W.UNABLE,W.NOVEL,W.NNA,RND.D1,P.MSG,W.HFUN,P.SUB
      XREF     W.FIDDLE,P.TN,W.GIVEUP,W.BROKEN,CHK4PT,PCOK,SP.LEVER
      XREF     W.NHAP,GET_CONT,CARRIED,P.MHAV,PRTTXT,W.SEPAWY
      XREF     GLOBAL.F,GETDATA,P.TNSTOP

	IfVerb	WAKE

	XDEF	V.WAKE
 
V.WAKE
 
	BTST    #6,4(A0)
	BEQ     DBS
	DO	GETNPC
	MOVE.B  2(A3),D1
	LSR.B   #4,D1               ;STATE
	CMP.B   #4,D1               ;ASLEEP
	BEQ.S   10$
	CMP.B   #5,D1              ;ALSO IF UNCONSCIOUS
	BEQ.S   20$
	DO	P.BTN
	DO	ISAREN
	MSGGO	ASLEEP
10$
	DO	P.PRN
	AND.B   #$0F,2(A3)   
	MSGGO	WAKES  
	
20$
	DO	P.SUBV
	MSGGO	UNABLE         
	
	ENDC

*--------------------------------

	IfVerb	RAPE

	XDEF	V.RAPE
 
V.RAPE
 
	BTST    #6,4(A0)
	BNE.S   10$
	MSGGO	NOVEL
10$
	BTST    #1,(A0)            ;DEAD?
	BEQ.S	15$
	MSGGO	NNA            ;NECROPHILIACS NOT ALLOWED
15$
	DO	GETNPC
	MOVE.B  2(A3),D1
	LSR.B   #4,D1
	CMP.B   #7,D1
	BEQ.S   20$
	MOVEQ   #1,D1
	RND_D1
    
	BeSure	M.CLDSHR		;incase not inc'ed
	
	ADD.W   #M.CLDSHR,D1
	DOGO	P.MSG
20$
	DO	P.SUB
	MSGGO	HFUN   


	ENDC
 
*--------------------------------

	IfVerb	MEND

	XDEF	V.MEND

V.MEND

      IFEQ	THE_PAWN

	XREF	SP.MEND

	CALL	SP.MEND

      ENDC
 
	BTST    #2,(A0)   	;BROKEN?
	BEQ.S   10$       	;NO
	DO	P.SUB
	MSG	FIDDLE
	DO	P.TN
	MSGGO	GIVEUP
10$
	BTST    #6,4(A0) 	;NPC?
	BNE     DBS
	DO	P.BTN
	DO	ISAREN
	MSGGO	BROKEN

	ENDC
 
*--------------------------------

	IfVerb	LEVER

	XDEF	V.LEVER
   
V.LEVER
 
	CLR.W   D1
	DO	CHK4PT
	CALL    PCOK
	BNE.S   90$                ;ERROR- EXIT
	CALL    SP.LEVER            ;*MUST* BE A CALL*NOT* BRA
	MSGGO	NHAP  
90$
	RET


	ENDC

*--------------------------------

	IfVerb	SHAKE

	IFEQ	THE_PAWN

	XDEF	V.SHAKE,ShakeIt
*
* Shaking objects:
*  Liquids spill out of open objects
*  Hard objects containing hard objects rattle
*  Liquids in containers slop around
*  Other things are shaken vigourously
*
V.SHAKE

	CALL	SP.SHAKE
	
*	DO	CARRIED			;must have it
*	BNE	P.MHAV			;not on inv

	MOVE.W	D0,D2			;save.
	MOVE.L	A0,A2
	MOVE.B	2(A0),D1		;size & weight
	AND.B	#$0F,D1			;zero weight?
	BEQ	DBS			;shake the guild of thieves etc!
	MOVE.B	1(A0),D1		
	LSR.B	#4,D1			;hardness -> d1
	CMP.B	#3,D1			;limit
	BLT	50$			;too soft (void,liquid, or qtty).
	MOVE.B	3(A0),D1		;shape & texture
	AND.B	#$0F,D1			;tex
	CMP.B	#8,D1			;soft?
	BEQ	50$			;yes.. too bad.
	ST	GLOBAL.F(A4)		;all! contents
	DO	GET_CONT		;ok D0 -> A1
	BCS	50$			; Carry set = not container
	CLR.W	GLOBAL.F(A4)		;back
10$
	MOVE.W	(A1)+,D0		;get element.
	BMI.S	10$			;MI => not just contained (rel maybe)
	BEQ	50$			;done. (zero term list)
	GETINFO				;of item.

* exclude 'out' objects

	BTST	#7,6(A0)		;may now include out items!
	BNE.S	10$			;yes, forget it.

	MOVE.B	3(A0),D1		;texture.
	AND.B	#$0F,D1			;texture
	CMP.B	#8,D1			;soft?
	BEQ.S	10$			;yes.
	MOVE.B	1(A0),D1		;pc
	LSR.B	#4,D1			;hardness.
	CMP.B	#1,D1			;liquid?
	BEQ.S	30$			;do liquids.
15$
	MOVE.W	D2,D0
	DO	P.TN
	MSGGO	CONTSOME		;<'contains something.'>
30$
	MOVE.B	(A2),D1			;state.
	BTST	#4,D1			;open?
	BNE.S	40$
	MSG	SOMESLOSH		;<'Something sloshes around in '>
	MOVE.W	D2,D0
	DO	P.TNSTOP
	BRA.S	90$
40$
	DO	P.TN
	MSG	SPILLAND		;'spills and '
	MSG	SEPAWY			;seeps away.
	DO	SETOUT
	BRA.S	90$			;exit
50$
	MOVE.W	D2,D0
        BRA.S   ShakeIt
90$
	RET

ShakeIt
        
	CLR.W	GLOBAL.F(A4)		;back
        DO	P.SUB
	MOVE.W	#VNSHAKE,D1
	DO	P.VERB
	DO	P.TN
	MSG	VIGOUR			;<'vigourously for a few moments.'>
        RET

	ENDC

	ENDC


*--------------------------------
      END

