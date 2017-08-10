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
*	@(#)fight.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.FIGHT
*
*--------------------------------
   
	include	"equates.asm"

	include	"macros.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"verbequ.asm"



      XREF     P.BTNI,SP.ATTAC,CHK4PT,YCANT,SUBJECT,P.MHAV,PCOK
      XREF     GETNPC,FNDWEAPO,P.TN,ISARE,W.DFLESS,W.DFND,P.PRN,W.BEATRB
      XREF     W.DEFET,P.OBJPRN,VERB,W.AETARG,OK,DEAD,W.AYKILL,SUBJECT2,P.SUB
      XREF     GETDATA


      IfVerb	FIGHT

	XDEF	FIGHT

FIGHT
 
	BTST    #1,(A0)
	BEQ.S   20$

	BeSure	M.DEAD

	MOVE.W  #M.DEAD,D1
	BRA     P.BTNI      ;IS ALREADY DEAD
20$
	CALL    SP.ATTAC
 
	CLR.W   D1
	DO	CHK4PT	            ;VALID?
	BTST    #6,4(A0)           ;LIVE?
	BEQ     YCANT
	BTST	#3,6(A5)		;intrinsic?
	BNE.S	30$			;yes, can use it as a weapon	
	MOVE.L  6(A5),D1
	CMP.W   SUBJECT(A4),D1   
	BNE.S   29$
	SWAP    D1
	CMP.W   SUBJECT2(A4),D1   
	BEQ.S   30$		  ;OK
29$
	MOVE.W  D5,D0
	BRA     P.MHAV
30$

      IFEQ	THE_PAWN

	MSGGO	NOMURDER

      ENDC

      IFNE	THE_PAWN

	CALL    PCOK
	BNE     FI90
	DO	GETNPC
	MOVE.B  2(A3),D1
	AND.B   #$F0,D1
	CMP.B   #$50,D1
	BEQ     FI51
	CALL    FNDWEAPO
	DO	P.TN
	MOVE.W  D0,D6              ;SAVE NOUN
	TEST_W  D3
	BNE.S   40$
	DO	ISARE
	MSG	DFLESS
	MOVE.B  #4,D3
	BRA.S   50$
40$
	MOVE.W  D3,D0
	MSG	DFND
	DO	P.TN
50$
	BTST    #7,3(A5)
	BNE.S   53$
	MOVE.B  1(A5),D1 
	AND.B   #$0F,D1
54$
	CMP.B   D1,D2
	BLS.S   60$
	MOVE.W  D6,D0
	DO	P.PRN
	MSGGO	BEATRB
53$
	MOVE.B  2(A3),D1
	LSR.B   #4,D1
	BRA.S   54$
60$
	DO	P.SUB
	MSG	DEFET
	MOVE.W  D6,D0
	GETINFO
	DO	P.OBJPRN
	DO	GETNPC
	MOVE.B  2(A3),D1
	AND.B   #$0F,D1
	OR.B    #$50,D1            ;UNCON
	MOVE.B  D1,2(A3)
	CMP.W   #VNKILL,VERB(A4)
	BNE.S   FI90
FI52
	MSG	AYKILL
	DO	P.OBJPRN
	BSET    #1,(A0)
FI90
	RET
FI51
	DO	P.TN
	DO	ISARE
	MSG	AETARG
	BRA.S   FI52

      ENDC

*--------------------------------
	
	XDEF	V.KILL

V.KILL
	
	CMP.W   #NPLAYER,D0     	;KILL ME/MYSELF?
	BNE     FIGHT			;NO
	CALL    OK
	DOGO	DEAD


	ENDC
	
*-------------------------------

      END

