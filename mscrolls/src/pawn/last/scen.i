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


**********************************************************************
*
*	%W%
*	%G%
*
**********************************************************************

* scen.i - macro and equates for fixed scenario messages
*          such as protection msgs+score/copyright/credits

FIXED_SCN	MACRO	* use
		SCN	FS_\1
		ENDM

F_DEF		MACRO	* label,value
FS_\1		EQU	\2
		ENDM


		F_DEF	Copyright,384
		F_DEF	Credits,385
		F_DEF	Score1,294
		F_DEF	Score2,382
		F_DEF	Removedisk,391
		F_DEF	Tryagain,393
		F_DEF	Thanks,388
		F_DEF	Enter,392
		F_DEF	Prot1,386
		F_DEF	Prot2,387
		F_DEF	Prot3,390
		F_DEF	Prot4,389

