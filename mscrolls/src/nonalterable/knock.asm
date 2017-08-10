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
*	@(#)knock.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.KNOCK
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include	"verbequ.asm"


      XREF     CONJ,W.SQLCH,W.THUD,V.BREAK,DBS

	
	IfVerb	KNOCK

	XDEF	V.KNOCK
	XREF	SP.KNOCK
 
V.KNOCK
 
	TEST_W  CONJ(A4)
	BPL     V.BREAK
	TEST_W  D0                 ;ANY?
	BMI.S   10$
	MOVE.B  1(A0),D1           ;GET PHYS CHAR
	LSR.B   #4,D1              ;HARDNESS
	CMP.B   #5,D1              ;SOFT?
	BLT.S	20$
	CMP.B   #12,D1
	BEQ     DBS
10$
	CALL    SP.KNOCK
	MSGGO	THUD
20$
	MSGGO	SQLCH


	ENDC

      END

