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
*	@(#)melt.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.MELT
*
*--------------------------------

      XREF     CHK4PT,YCANT,W.ICNDTW,P.TN,SP.MELT,W.MELTS,W.SEPAWY,SETOUT

      include	"equates.asm"
      include	"macros.asm"
      include	"verbequ.asm"

      IfVerb	MELT

      XDEF     V.MELT
 
V.MELT
 
	CLR.W   D1
	DO	CHK4PT
	MOVE.B  3(A0),D1
	AND.B   #$0F,D1
	CMP.B   #$B,D1             ;ICE COLD?
	BNE     YCANT
	MOVE.B  3(A5),D1
	AND.B   #$0F,D1
	EXG     D0,D5
	CMP.B   #3,D1              ;HOT?
	BEQ.S   10$
	CMP.B   #$A,D1             ;WARM?
	BEQ.S   10$
	MSG	ICNDTW
	DOGO	P.TN
10$
	EXG     D0,D5
	CALL    SP.MELT
	DO	P.TN
	MSG	MELTS
	AND.B   #$F0,3(A0)
	MOVE.B  1(A0),D1
	AND.B   #$F,D1
	OR.B    #$10,D1
	MOVE.B  D1,1(A0)           ;LIQUID
	BTST    #5,6(A0)           ;CONTAINED?
	BEQ.S   99$
	DO	P.TN
	MSG	SEPAWY
	DO	SETOUT
99$
	RET

	ENDC

      END
