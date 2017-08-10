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
*	@(#)feel.asm	2.11
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.FEEL/TOUCH
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"
	include "verbequ.asm"


      XREF     BIZARE,W.NNA,ADVERB,W.OYSB,NPC.OK,P.PRN,W.SOFT
      XREF     P.TN,ISARE,W.WET,W.AND,W.VSHP,W.SHP,P.MSG,DBS,BSFULLSTOP

	IfVerb	TOUCH
 
	XDEF	V.TOUCH
	XREF	SP.TOUCH

V.TOUCH

      IFEQ	THE_PAWN

      XREF	SP.FEEL

	CALL	SP.FEEL			

      ENDC
 
	BTST    #6,6(A0)           ;ROOM?
	BNE     BIZARE
	BTST    #1,(A0)            ;DEAD?
	BEQ.S	10$
	MSGGO	NNA              ;NECROPHILIACS..  
10$
	CMP.W   #-1,ADVERB(A4)
	BPL.S   20$
	BTST    #6,4(A0)
	BEQ     BIZARE
	 
 
	CMP.W   #$8008,ADVERB(A4)
	BNE     BIZARE
	CALL    SP.TOUCH
	MSGGO	OYSB
 
20$
	 
	BTST    #6,4(A0)           ;NPC?
	BEQ.S   25$                ;NOPE
	CALL    NPC.OK             ;CONSIOUS?
	BEQ.S   25$
	DO	P.PRN
	DO	ISARE
	MSG	SOFT
	DOGO	BSFULLSTOP
25$
	CLR.L   D1
	MOVE.B  3(A0),D1
	MOVE.B  D1,D2
	AND.B   #$0F,D1            ;TEXTURE IN D1
	BEQ     DBS
	LSR.B   #6,D2              ;SHARPNESS
	DO	P.TN
	DO	ISARE
	BTST    #5,3(A0)           ;WET ? 
	BEQ.S   30$  
	MOVE.B  D1,D6
	MSG	WET
	MSG	AND
	CLR.L   D1
	MOVE.B  D6,D1
30$
	TEST_B  D2
	BEQ.S   50$
	CMP.B   #3,D2
	BNE.S   40$
	MSG	VSHP
	DOGO	BSFULLSTOP
40$    
	MSG	SHP
	DOGO	BSFULLSTOP
 
50$
	TEST_B  D1
	BEQ.S   60$

	BeSure	M.TEXTURE

	ADD.W   #M.TEXTURE,D1
	DO	P.MSG
	DOGO	BSFULLSTOP
60$
	RET


	ENDC

      END

