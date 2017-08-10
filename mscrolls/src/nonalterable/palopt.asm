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
*	@(#)palopt.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

******************************************************
*
* PALOPT.ASM   - Pallet options 
*
******************************************************

	include	"equates.asm"
	include	"macros.asm"


         XDEF     DO_FOREG,DO_BACKG,DO_CONTR,DO_SCROL

         XREF     BACK_PAL,FORE_PAL,GRAF_OPL,PRTTXT,SCRL_PAL,OK



*
* Misc pallet changing bits called via DEBUGBITS
*
* on arrival D0 = given parameter (in DEC!!!)
*


BAD_EXIT

         PRINT    <'^Invalid colour.'>
         RET



******************************************************
*
* DO_FOREG - do foreground
*
******************************************************

DO_FOREG

      CALL     CONV_TO_BCD
      BNE      BAD_EXIT
      CMP.W    BACK_PAL(A4),D0            ;cannot have same fore/back
      BEQ      BAD_EXIT
      MOVE.W   D0,FORE_PAL(A4)            ;copy for save/load game
      MOVE.W   D0,GRAF_OPL+2(A4)          ;copy to use
      BRA      OK


******************************************************
*
* DO_BACKG - do background
*
******************************************************

DO_BACKG

      CALL     CONV_TO_BCD
      BNE      BAD_EXIT
      CMP.W    FORE_PAL(A4),D0            ;cannot have same fore/back
      BEQ      BAD_EXIT
      MOVE.W   D0,BACK_PAL(A4)            ;for save/load
      MOVE.W   D0,GRAF_OPL(A4)            ;for use to update video
      MOVE.W   D0,GRAF_OPL+4(A4)
      BRA      OK

******************************************************
*
* DO_SCROL - do scroll colour
*
******************************************************

DO_SCROL

      CALL     CONV_TO_BCD
      BNE      BAD_EXIT
      CMP.W    BACK_PAL(A4),D0            ;cannot have same fore/back
      BEQ      BAD_EXIT
      MOVE.W   D0,SCRL_PAL(A4)            ;for save/load
      MOVE.W   D0,GRAF_OPL+6(A4)          ;for use to update video
      BRA      OK

******************************************************
*
* DO_CONTR - do contrast
*
*  this resets pallet to white (777) foreground,scrolls
*                    and black (000) background
*
******************************************************


DO_CONTR

      CLR.W    BACK_PAL(A4)
      CLR.W    GRAF_OPL(A4)
      MOVE.W   #$777,D0
      MOVE.W   D0,FORE_PAL(A4)
      MOVE.W   D0,GRAF_OPL+2(A4)
      MOVE.W   D0,GRAF_OPL+6(A4)          ;SCROLLS+MOUSE
      MOVE.W   D0,SCRL_PAL(A4)

      RET




* Convert to bcd - take D0 (dec) return 'bcd' version


CONV_TO_BCD

     IFEQ     YCHEAT                  ;if true then it's hex already


      CMP.W    #778,D0
      BPL.S    20$                    ;OUCH!!

      MOVEQ    #2,D1
10$
      EXT.L    D0
      DIVU     #10,D0
      SWAP     D0
      MOVE.W   D0,-(A7)
      SWAP     D0
      DBRA     D1,10$

      MOVEQ    #0,D0
      OR.W     (A7)+,D0
      LSL.W    #4,D0
      OR.W     (A7)+,D0
      LSL.W    #4,D0
      OR.W     (A7)+,D0
      MOVEQ    #0,D1                ;set equal
      RET
20$
      SETNE

     ENDC

      RET

      END
