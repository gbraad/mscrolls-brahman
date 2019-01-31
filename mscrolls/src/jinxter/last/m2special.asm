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
* M2 SPECIAL
*
*--------------------------------

  
	SECTION  "ADV",CODE

	include  "equates.i"
	include  "macros.i"

    
    XDEF SP.DEATH,SP.PICTURES

SP.DEATH
        RET

    XREF  LodgePicture
    XREF  FloodPos,OfficeFlooded
    
SP.PICTURES

    ;;  D0 = picture# (can be zero)
    ;;  D1 = version

    ;; FP = 4 => chest, 5=>neck, 6=>nose

    MOVE.L  D1,-(A7)
    CMP.B   #LodgePicture,D0
    BNE.S   90$
    TEST_B  OfficeFlooded(A4)
    BNE.S   15$
    MOVE.W  FloodPos(A4),D1
    BEQ.S   90$
    SUB.W   #4,D1
    BGT.S   10$                 ; 5,6
    ADDQ.B  #1,D0               ; lodge+1 => low height
    BRA.S   90$
10$    
    ;; D1=1,2
    ADDQ.B  #1,D1               ;+2 etc
    CMP.B   #4,D1               ;max
    BLE.S   20$                 ;ok
15$    
    MOVE.B  #4,D1               ;set max
20$    
    ADD.B   D1,D0               ;new picture
90$    
    MOVE.L  (A7)+,D1
    RET
        
*--------------------------------
        END
