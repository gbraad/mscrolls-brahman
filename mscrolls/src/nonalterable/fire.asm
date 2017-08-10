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
*	@(#)fire.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.FIRE/B/S/TWIST/TURN
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include "verbequ.asm"


      XREF     P.SUBV,W.UNABLE,ADVERB,CONJ,W.IDFY,SP.TURN,P.SUB,W.YCNDTT
      XREF     P.TN,W.IFONLY,W.YBLUKY,P.TNSTOP

	IfVerb	FIRE

	XDEF	V.FIRE

V.FIRE
 
	DO	P.SUBV
	MSGGO	UNABLE         ;SILLY EH?

	ENDC

TwistOrTurn	SET	FALSE
 
	IfVerb	TWIST
	XDEF	V.TWIST
TwistOrTurn	SET	TRUE
	ENDC
	
	IfVerb	TURN
	XDEF	V.TURN
TwistOrTurn	SET	TRUE
	ENDC
	
	IFNE	TwistOrTurn

V.TWIST
 
V.TURN
 
	CMP.W   #-1,ADVERB(A4)
	BEQ.S   05$  
	MOVE.W  ADVERB(A4),CONJ(A4)
	AND.W   #$7FFF,CONJ(A4)
05$
	CALL    SP.TURN
	CMP.W   #ON,CONJ(A4)        ;ON
	BEQ.S   10$
	CMP.W   #OFF,CONJ(A4)      ;OFF
	BEQ.S	10$
	MSGGO	IDFY
10$
	BTST    #6,4(A0)   
	BNE.S   30$
	DO	P.SUB
	MSG	YCNDTT
	DOGO	P.TNSTOP
 
30$
	CMP.W   #ON,CONJ(A4)        ;ON
	BEQ.S	40$
	MSGGO	IFONLY
40$
	MSGGO	YBLUKY

	ENDC

      END

