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


**************************************************************
*
*	4/10/89
*	@(#)undo.elt	1.1
*
**************************************************************


	SECTION	"ADV",CODE

	include	"equates.asm"
	include	"macros.asm"


	XDEF	UndoDiff,MakeUndoDiff
	XREF	CLOCK,PRTNUM,ForceRefresh,SC70,INVENT

UndoDiff

      IFNE	M68000
	NOP
      ENDC

      IFEQ	M68000
	DC.W	$A0E2
      ENDC

	CALL	ForceRefresh
	LEA	INVENT,A0
	ADD.L	A4,A0
	JSR	(A0)
	MOVE.L	(A7)+,D0
	BRA	SC70

MakeUndoDiff


	MOVE.W	CLOCK(A4),D0

      IFNE	M68000
	NOP
      ENDC

      IFEQ	M68000
	DC.W	$A0E3
      ENDC

	RET					;back to screen

	END
