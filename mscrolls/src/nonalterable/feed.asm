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
*	@(#)feed.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.FEED/BITE
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"
	include "verbequ.asm"


      XREF     CONJ,CHK4PT,W.NMOUTH,P.TN,ISAREN,W.INTER,GETNPC,W.MUNCH
      XREF     CHKNPC,DBS,W.THANK,P.SUB,SETOUT,V.TASTE,W.CRUNCH
      XREF	P.MHAV,FIXCHK,YCANT,W.ATMOMT,CARRIED,ISARE,BSFULLSTOP

      IfVerb	FEED

	XDEF	V.FEED
   
V.FEED
 
	MOVEQ   #0,D1		;'WITH' for CHK4PT
	CMP.W   #1,CONJ(A4)      ;TO? 
	BNE.S   01$		;No - so it MUST be 'with' then.

	EXG     D0,D5		;swap nouns and data
	EXG     A0,A5
	MOVEQ   #1,D1		;tell CHK4PT that 'TO' is ok

01$
	DO	CHK4PT
	BTST    #6,4(A0)
	BNE.S   10$
	MSGGO	NMOUTH
10$
	EXG     D0,D5		;CHKNPC takes D5/A5
	EXG     A0,A5
	DO	CHKNPC

	DO	CARRIED	           ;MUST HAVE IT
	BNE     P.MHAV          ;NO
	DO	FIXCHK             ;NOT IF TIED
	BEQ.S   11$                ;OK
	DO	YCANT
	MSGGO	ATMOMT         ;NOT AT THE MOMENT
11$
	EXG     D0,D5
	EXG     A0,A5

      IFEQ	THE_PAWN

	XREF	SP.FEED

	CALL	SP.FEED		;D0 = pc, D5 = food

      ENDC

	MOVE.B  11(A5),D1
	AND.B   #$E0,D1             ;EDIBLE?
	BNE.S   20$
15$
	DO	P.TN
	DO	ISARE
	MSGGO	NOTINTERESTED
20$
	DO	GETNPC
	BTST    #2,2(A3)
	BEQ.S   30$
	CMP.B   #3,D1
	BLS.S   15$
	MSG	MUNCH
	 
	BRA.S   40$
30$
	DO	P.TN
	MSG	THANK
	DO	P.SUB
	CALL	BSFULLSTOP
40$
	MOVE.L  D5,D0
	DOGO	SETOUT

	ENDC
 
*--------------------------------

	IfVerb	BITE
   
	XDEF	V.BITE

V.BITE
 
	MOVE.B  11(A0),D1
	AND.B   #$E0,D1
	BNE     V.TASTE
	MOVE.B  2(A0),D1
	AND.B   #$0F,D1             ;WEIGHT?
	BEQ     DBS
	DO	CARRIED
	BNE	P.MHAV
	MSGGO	CRUNCH

	ENDC
 
*--------------------------------

      END

