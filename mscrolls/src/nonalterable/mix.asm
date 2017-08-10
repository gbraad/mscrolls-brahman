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
*	@(#)mix.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.MIX/UNMIX
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include	"verbequ.asm"



      XREF     SP.MIX,SP.SPLIT,CHK4PT,YCANT,W.MAGFOR,UNTIE

	IfVerb	MIX

	XDEF	V.MIX
 
V.MIX
 
	CALL    SP.MIX
	 
	MOVEQ   #0,D1              ;'WITH'
	DO	CHK4PT
	MOVE.B  1(A0),D1           ;PHYS CHAR   
	LSR.B   #4,D1  
	CMP.B   #1,D1
	BEQ.S   10$          
	MOVE.B  1(A5),D1
	LSR.B   #4,D1
	CMP.B   #1,D1
	BNE     YCANT
10$
	MSGGO	MAGFOR


	ENDC
 
*--------------------------------

	IfVerb	SPLIT

	XDEF	V.SPLIT
 
V.SPLIT
 
	CALL    SP.SPLIT
	 
	BRA     UNTIE

	ENDC

      END

