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
*	@(#)wear.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.WEAR
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include	"verbequ.asm"
   


      XREF     W.BUT,P.SUBV,W.WEARNG,P.TN,GETDATA,P.SUB,W.PUTON,SP.WEAR
      XREF     W.ICNDTW,ISARE,P.TNSTOP,SP.SCORE.NOUN,P.BTN,FIXCHK,YCANTNOW

	IfVerb	WEAR

	XDEF	WEAR

WEAR
 
	DO	FIXCHK			;tied/has things tied to it?
	BNE.S   30$
	BTST    #2,6(A0)           ;ALREADY?
	BEQ.S   10$                ;OK
	MSG	BUT
	DO	P.SUBV
	MSG	WEARNG
	DOGO	P.TNSTOP
10$
	BTST    #0,11(A0)
	BEQ.S   20$                ;NOT WORN
	MOVE.W  8(A0),D1
	EXG     D0,D1
	DO	P.BTN
	CALL    GETDATA
	DO	ISARE
	MSG	WEARNG
	EXG     D1,D0
	DOGO	P.TNSTOP
20$
	MOVE.B  11(A0),D1
	AND.B   #6,D1
	BEQ.S   30$                ;NOT POS
	DO	P.SUB
	MSG	PUTON
	DO	P.TNSTOP
	MOVE.L	#$04000000,6(A0)	;make worn only
	BRA     SP.WEAR
30$
	DOGO	YCANTNOW

	ENDC

      END

