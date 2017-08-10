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
*	@(#)wtable.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

************************************************************************
*
* Wtable (1-23)
*
************************************************************************

*
*  Due to the odd bytes between link modules all the wtable files must
*  be in one '.O' file. This way maybe the parser will understand 'north'
*

	include	"equates.asm"

wtablestart

	include	"wtable1.asm"     ;ignored,specials,conj,adverbs
	include "wtable4.asm"		;verbs
	include	"wtable5.asm"     ;adjectives
	include	"wtable6.asm"     ;nouns
	include	"wtable7.asm"     ;misc, including synonyms & multiples


    IFEQ	THE_PAWN

	XDEF	Section24

	DC.B	$81		;end of table to avoid clashes with scenery
				;nouns and normal nouns

Section24

	include	"wtable24.asm"

    ENDC


	DC.B	$81		;end of table

            end
