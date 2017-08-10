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
*	@(#)strangle.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.STRANGLE/PLANT/KISS
*
*--------------------------------
   
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"

      XREF     CONJ,W.IDFY,YCANTW,FIGHT,SP.PLANT,YCANT,DBS,CHKNPC,GETNPC
      XREF     W.SQLCH,P.SUB,W.GIVE,P.OBJPRN,W.ABSKIS

	IfVerb	STRANGLE

	XDEF	V.STRANG

V.STRANG
 
	TEST_W  CONJ(A4)
	BMI.S   60$
	BEQ.S	10$
	MSGGO	IDFY           ;0=WITH= OK
10$
	CMP.W   #NHAND,D5        ;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	BEQ.S   60$
	EXG     D0,D5
	EXG     A0,A5
	DOGO	YCANTW
60$
	BRA     FIGHT

	ENDC
  
*--------------------------------

	IfVerb	PLANT

	XDEF	V.PLANT
   
V.PLANT
 
	CALL    SP.PLANT
	DOGO	YCANT

	ENDC
 
*--------------------------------

	IfVerb	KISS

	XDEF	V.KISS
	
V.KISS
	
	BTST    #6,4(A0)   	;ALIVE?
	BEQ     DBS
	
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	DO	CHKNPC
	DO	GETNPC	   	;GET DATA TO A3
	BTST    #2,2(A3)  	;ANIMAL?
	BEQ.S   10$       	;NO
	MSG	SQLCH  
	MOVE.B  2(A3),D1
	AND.B   #$0F,D1
	OR.B    #$80,D1
	MOVE.B  D1,2(A3)  	;BACK
	BRA.S   99$       	;EXIT
10$
	DO	P.SUB
	MSG	GIVE
	DO	P.OBJPRN
	MSG	ABSKIS		;a sloppy kiss
99$
	RET

	ENDC

      END
