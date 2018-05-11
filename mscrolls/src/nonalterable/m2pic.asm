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

        ;; A0 = code address of name of picture
        ;; D0 = picture number
        ;; D1 = operation

        PUSH_L  D2/A0
        LEA     NAMETAB(PC),A0
        MOVE.W  D0,D2
        ADD.W   D2,D2
        MOVE.W  0(A0,D2.W),D2           ; name offset
        EXT.L   D2
        ADD.L   D2,A0
        SUB.L   A4,A0
        DC.W    $A0F0
        PULL_L  D2/A0
        RET
        ENDC

*--------------------------------
        END
