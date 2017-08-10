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
*	@(#)cross.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.CROSS/SLIDE/BLOW/PRESS
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include "verbequ.asm"


	XREF     CONJ,GO.N,CHK4PT,W.ICNDTW,P.TN,SP.SLIDE
	XREF     SP.MOVE.,V.MOVE,W.WNTMVE,W.AHEM,W.PUFF,SP.PRESS
	XREF     W.NHAP


	IfVerb	CROSS

	XDEF	V.CROSS

V.CROSS

	TEST_W  CONJ(A4)
	BMI     GO.N
	CLR.W   D1
	DO	CHK4PT
	MSG	ICNDTW
	MOVE.W  D5,D0
	DOGO	P.TN


	ENDC

*--------------------------------

	IfVerb	SLIDE

	XDEF	V.SLIDE

V.SLIDE

	CALL    SP.SLIDE            ;LIFT DOOR?
	CALL    SP.MOVE.
	BTST    #4,3(A0)
	BNE     V.MOVE
	DO	P.TN
	MSGGO	WNTMVE
	
	ENDC

*--------------------------------

	IfVerb	BLOW

	XDEF	V.BLOW

V.BLOW

	BTST    #6,4(A0)
	BEQ.S	10$
	MSGGO	AHEM
10$
	MSGGO	PUFF

	ENDC

*--------------------------------

	IfVerb	PRESS

	XDEF	V.PRESS

V.PRESS

	CALL    SP.PRESS
	MSGGO	NHAP

	ENDC


	END

