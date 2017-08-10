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
*	@(#)spool.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*******************************************************************
*
*  SPOOL - Adventure printer spooler
*
*******************************************************************

	include	"equates.asm"

      XDEF     SPOOLTXT,SPOOLOUT

	IFNE	SpoolLen

      XREF     SpoolPos,SpoolBuf,SpoolOff,PrtThere,PRINTER
      XREF     PRINT_CHAR,CHK_PRINTER,CHANNEL

      ENDC



* SPOOLTXT, Char to be 'printed' is in D2.B, always hi bit low

* N.B. A6 points at start of prog A4 = ???
*
* this bit called from cout to place characters in the buffer


SPOOLTXT

     IFNE     SpoolLen


      TEST_B   CHANNEL(A6)                   ;Hmmm due to Amiga hassle!
      BNE.S    99$


      PUSH_L   D0-D7/A0-A6


      TEST_B   PRINTER(A6)          ;printer on = -1
      BEQ.S    90$

      CMP.B    #8,D1                ;backspace?
      BEQ.S    80$                  ;no, do a simple insert

      LEA      SpoolBuf,A0          ;Start of buffer
      ADD.L    SpoolPos(A6),A0      ;current pos in buffer
      MOVE.B   D1,0(A6,A0.L)        ;save byte

      ADDQ.L   #1,SpoolPos(A6)         ;update pos. in buffer
      CMP.L    #SpoolLen,SpoolPos(A6)  ;run out of buffer?
      BNE.S    10$
      CLR.L    SpoolPos(A6)         ;yes, start at begining again
10$
      MOVE.L   SpoolOff(A6),D0      ;find how far we've got in printing the
*                                   ;stuff
      SUBQ.L   #1,D0                ;last minute fudge (cake?)
      CMP.L    SpoolPos(A6),D0
      BNE.S    90$                  ;ok, we didn't catch up

      TEST_B   PrtThere(A6)         ;have we found a printer at all?
                                    ;NE => yes, so the printer is slow
      SNE      PRINTER(A6)          ;Cannot find a printer guys..
      BEQ.S    90$                  ;So kill printer flag and leave

      MOVE.L   A6,A4                ;urk!!!
      CALL     SPOOLOUT             ;print some of the chars in buffer
      BRA.S    10$

* BS comes here

80$
      SUBQ.L   #1,SpoolPos(A6)      ;loose char prior to BS
      BPL.S    90$                  ;no, hassle due to BS over start/end

      MOVE.L   #SpoolLen-1,SpoolPos(A6)   ;point at last char in buffer
90$
      MOVE.L   A6,A4
      CALL_S   SPOOLOUT

     IFNE     AMIGA512

   XREF     DoSpeech

      CALL     DoSpeech

     ENDC                              ;of Amiga bit 

      PULL_L   D0-D7/A0-A6

     ENDC                              ;of IFNE SPOOLEN

99$

      RET



*
* SPOOLOUT, This tries to empty the spool buffer at the printer
*
* if it cannot send characters it returns immediately, otherwise it tries
* to send chars until a line break (LF = 10), thus avoiding hassle with a 
* BS trying to remove a char that's already been printed
*
* NB expects A4!!!!
*


SPOOLOUT

	IFNE	SpoolLen

      PUSH_L   D0-D7/A0-A6

      MOVE.L   SpoolPos(A4),D0      ;Get pos where chars going in
      CMP.L    SpoolOff(A4),D0      ;is buffer empty?
      BEQ.S    90$                  ;Yes, forget it

      CALL     CHK_PRINTER          ;is the printer on-line?
      BNE.S    90$                  ;nope, forget it

      LEA      SpoolBuf,A0          ;Addr (less A4) of buffer
      ADD.L    A4,A0
      MOVE.L   SpoolOff(A4),D2      ;how far we've printed

10$
      CMP.B    #10,0(A0,D2.L)       ;a line feed?
      BEQ.S    30$                  ;yes, print the chars up to it
      ADDQ.L   #1,D2
      CMP.L    #SpoolLen,D2         ;end of buffer?
      BNE.S    20$
      MOVEQ    #0,D2                ;Yes, wrap around
20$
      CMP.L    D2,D0                ;run out of chars to print?
      BNE.S    10$                  ;nope, keep looking for LF
      BRA.S    90$                  ;Yes, leave it till we get a LF

* ok, now we have a no. of chars terminated with a LF, print them

30$
      MOVE.L   SpoolOff(A4),D2
40$
      CALL     CHK_PRINTER          ;Is printer ready?
      BNE.S    90$                  ;NE => nope, time to leave
      MOVE.B   0(A0,D2.L),D1        ;get char to print
      CALL     PRINT_CHAR
      ADDQ.L   #1,D2                ;Next char addr
      CMP.L    #SpoolLen,D2         ;time to deal with wrap around?
      BNE.S    50$
      MOVEQ    #0,D2
50$
      MOVE.L   D2,SpoolOff(A4)      ;update output stream pointer
      CMP.B    #10,D1               ;was that our line feed?
      BNE.S    40$                  ;nope, get more
90$
      PULL_L   D0-D7/A0-A6       ;time to leave


      ENDC

      RET


      END
