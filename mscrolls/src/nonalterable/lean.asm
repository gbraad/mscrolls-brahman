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
*	@(#)lean.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.LEAN/STAND
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include	"verbequ.asm"


      XREF     CONJ,ADVERB,W.IDFY,YCANT,CHK4PT,P.TN,W.ANOY
      XREF     SEATED,P.BYAN,P.SUBV,RIDING,W.ONFEET

      IfVerb	LEAN

      XDEF	V.LEAN
      XREF	SP.LEAN
   
V.LEAN
 
	BTST    #6,4(A0)            ;ALIVE
	BNE.S   10$
	BTST    #2,6(A0)            ;WORN  ?
	BNE.S   10$  
	TEST_W  CONJ(A4)
	BPL.S   50$
	CMP.W   #AGAINST,ADVERB(A4)     ;AGAINST 
	BEQ.S   05$
	CMP.W   #ON,ADVERB(A4)      ;ON
	BEQ.S	05$
	MSGGO	IDFY
05$
	CALL    SP.LEAN
10$
	DOGO	YCANT
50$
	MOVE.W  #$11,D1
	CMP.W   #$3,CONJ(A4)
	BEQ.S   51$
	MOVE.W  D1,CONJ(A4)
51$
	DO	CHK4PT
	DO	P.TN
	MSGGO	ANOY

	ENDC
  
*--------------------------------

	IfVerb	STAND

	XDEF	V.STAND
	        
V.STAND
	        
	TEST_W  D0                  ;A NOUN?
	BMI.S	10$
	MSGGO	IDFY
10$	
	TEST_W  SEATED(A4)
	BEQ     P.BYAN            ;BUT YOU ARE NOT SEATED
	DO	P.SUBV
	CLR.W   SEATED(A4)
	CLR.W   RIDING(A4)
	MSGGO	ONFEET

	ENDC

*--------------------------------

      END

