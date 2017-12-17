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

	include  "equates.asm"
	include  "macros.asm"

        XDEF SP.DEATH,SP.PICTURES

SP.DEATH
        RET

SP.PICTURES

        ;; D0 = requested picture#
        ;; can change it here or set to 0 to reject pic

        XREF    ExplodeBankFlag

        TEST_B  ExplodeBankFlag(A4)
        BEQ.S   20$

        ;; pictures 0x14=BANK1 and 0x16=BANKMAN are not there anymore
        ;; NB: D0 = pic+1, since d0 mean no picture insofar as index
        ;; into picnames
        CMP.B   #$15,D0
        BEQ.S   10$
        CMP.B   #$17,D0
        BNE.S   20$
10$        
        CLR.W   D0              ; cancel
20$        

        RET
        
*--------------------------------
        END
