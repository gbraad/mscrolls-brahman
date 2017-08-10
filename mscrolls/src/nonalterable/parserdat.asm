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


*---------------------------------------------
*  
*	5/16/87
*	@(#)parserdat.asm	2.14
*
*---------------------------------------------

	SECTION		"ADV",CODE

* 	parserdat.asm
* 	temporary data used by the parser
*

	include		"equates.asm"

	XDEF		CMDPNT,OLDWRDNUM,WHICHFIX,WDEPTH,QUIETWH,RELTXT
	XDEF		PADV,PVERB,FNOUN,MKLSaved,LastDirection,OLDADVERB
	XDEF		ISNPC

ISNPC	DC.W	0	;flag used by Anita's npc stuff (and makelocal)

CMDPNT	DC.L	0	;command point (text ptr) [ parser3.asm.getadjph ]
WDEPTH	DC.W	0			;level of nested which questions
QUIETWH	DC.W	0			;used to prevent PICKONE
OLDWRDNUM
		DC.L	0		;to hold posn b4 relresolve
PVERB   DC.W	0			;store pending verb.
PADV	DC.W	0			;pending adverb
WHICHFIX
		DC.W	0,0,0,0,0,0
		DC.W	0,0,0,0,0,0
		DC.W	0,0,0,0,0,0
		DC.W	0,0,0,0,0,0
		DC.W	0,0,0,0,0,0
		DC.L	0		;end
RELTXT
		DS.B	BUFFLEN

FNOUN		DC.W	0		;flag to force noun
MKLSaved	DC.W	0		;somewhere for makelocal to look
LastDirection	DC.W	-1		;DIRECT puts dir here before SETMODE
OLDADVERB	DC.W	0

*---------------------------------------------
		END
