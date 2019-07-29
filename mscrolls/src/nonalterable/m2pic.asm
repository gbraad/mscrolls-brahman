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
*	%W%
*	%G%
*
******************************************************

*--------------------------------
*
* M2 PIC
*
*--------------------------------

  
	SECTION  "ADV",CODE

	include  "equates.asm"
	include  "macros.asm"

        XDEF    LOAD_PIC
		XREF    ROOM

        IFEQ    NAMEDPICS
LOAD_PIC

        ;; picture number in D0
        ;; operation in D1 (1=>display)

        DC.W    $A0F0
        RET
        ENDC

        IFNE    NAMEDPICS
        include  "pictnames.asm"
        ALIGN
        
LOAD_PIC

    ;; D0 = picture number
    ;; D1 = operation (1 = show, 0 = hide)
    ;; D2.B = picture version
    ;; sets D3.W = current room

    ;; set up A0 to point to picture name

    PUSH_L  D3/A0
    LEA     NAMETAB(PC),A0
    MOVE.W  D0,D3
    ADD.W   D3,D3
    MOVE.W  0(A0,D3.W),D3           ; name offset
    EXT.L   D3
    ADD.L   D3,A0
    SUB.L   A4,A0
    MOVE.W  ROOM(A4),D3
    DC.W    $A0F0                   ; graphic trap
    PULL_L  D3/A0
    RET
    ENDC

*--------------------------------
        END
