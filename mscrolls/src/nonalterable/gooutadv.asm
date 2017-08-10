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


*****************************************************************
*
*	@(#)gooutadv.asm	1.3
*	12/12/86
*
*****************************************************************

	SECTION	"ADV",code



* this is the dc.b statements of the outward end of adv!
PUTDATA		MACRO
	XDEF	\1
\1	DS.B	4+4		;LEA AND JSR X(A4)
	ENDM

	PUTDATA		MAKEFREE
	PUTDATA		DO_BACKG
	PUTDATA		MAKESTIP
	PUTDATA		GRA_EXIT
	PUTDATA		DO_CONTR
	PUTDATA		READFILE
	PUTDATA		MAKEDITH
	PUTDATA		DO_SCROL
	PUTDATA		MS_END
	PUTDATA		WRITEFIL
	PUTDATA		GETCHAR
	PUTDATA		SPOOLOUT
	PUTDATA		LOAD_PIC
	PUTDATA		GETLINE
	PUTDATA		MS_BUTT
	PUTDATA		MAKEFEE
	PUTDATA		DO_FOREG
	PUTDATA		SWHEIGHT	
	PUTDATA		COUT


	END
