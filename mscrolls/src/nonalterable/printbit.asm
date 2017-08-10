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
*	@(#)printbit.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.PRINTBITS
*
*--------------------------------


      XREF     CHAROUT,CHAR.SUB,POS

      XDEF     LINE,UTIL.BS,UTIL.SP,UTIL.DEL

      include	"equates.asm"
      include	"macros.asm"
 
*--------------------------------
*
* UTIL.DEL  DELETES CHAR TO LEFT
*           OF CURRENT CURS POS
*           (MOVES CURSOR LEFT)
*
*--------------------------------
 
 
UTIL.DEL    
 
       MOVE.L  D1,-(A7)           ;ELSE STOMPED
       CALL_S  UTIL.BS            ;MOVE CURSOR LEFT
       MOVEQ   #$20,D1            ;SPACE
       DO	CHAR.SUB           ;PRINT - IGNORING LEFT JUSTIFY
       ADDQ.W  #1,POS(A4)         ;UPDATE SCREEN POSITION
       MOVE.L  (A7)+,D1
 
* FALLS THROUGH TO UTIL.BS
 
*--------------------------------
*
* UTIL.BS   JUST MOVES CURSOR
*           LEFT ONE CHAR
*
*--------------------------------
 
UTIL.BS
 
       MOVE.L  D1,-(A7)
       MOVEQ   #8,D1
       DO	CHAR.SUB
       SUBQ.W  #1,POS(A4)
       MOVE.L  (A7)+,D1
       RET
 
*--------------------------------
*
* LINE - DOES A LINE FEED
*    
* N.B. LF DOES *NOT* CLEAR TO
*      RIGHT OF CURSOR
*
*--------------------------------
 
LINE
   
       MOVE.L  D1,-(A7)
       MOVEQ   #$0A,D1            ;LF
       DO	CHAROUT	           ;HAS TO COPE WITH LEFT JUST ETC
       MOVE.L  (A7)+,D1
       RET
   
*--------------------------------
*
* UTIL.SP - FORCES A SPACE!
*
* USES CHAR.SUB TO AVOID NASTY
* RULE APPLICATION IN CHAROUT
* (DECOM'S FAULT)
*
*--------------------------------
 
UTIL.SP
 
       MOVE.L  D1,-(A7)
       MOVEQ   #$20,D1
       ADDQ.W  #1,POS(A4)         ;KEEP LEFT JUST HAPPY
       DO	CHAR.SUB           ;DO THE SPACE (FORCED)
       MOVE.L  (A7)+,D1
       RET

*-------------------------------

       END
