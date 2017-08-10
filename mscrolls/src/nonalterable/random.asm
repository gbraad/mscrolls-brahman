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
*	@(#)random.asm	2.5
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.RANDOM
*
*--------------------------------

	include	"equates.asm"

      XREF  RND.CHIN               ;RND.CHARIN

      XDEF  INITRND,INITRND3,RND.D1,RNDGEN,RNDNUM,RND.DATA

*--------------------------------
*
* INITRND - INITIALISE RANDOM NO.  !!! CALL AT START !!!
*           SEQUENCE, CALL AT START
*           OF GAME, USES SEED TO
*           PRODUCE 'PACK' TO BE
*           USED DURING SHUFFLE
*           WHEN GENERATING NO.S
*
*--------------------------------

      IFEQ     M68000

RNDTRAP    EQU   $A0F7
RNDTRAP2   EQU   $A0F6

      ENDC

INITRND
      IFNE     M68000

       LEA     RND.DATA(PC),A6    ;POINT AT DATA AREA
 
       MOVE.B  SEED(A6),RND(A6)   ;GRAB SEED AND START 'PACK'
       MOVE.B  SEED+1(A6),D0      ;LOW BYTE OF SEED
       MOVE.B  D0,RND+2(A6)       ;GENERALLY MESS AROUND WITH SEED
       MOVE.B  RND(A6),D1         ;TO PRODUCE INITIAL 'PACK'
       ADDX.B  D1,D0              ;ADC (HONEST!)
       MOVE.B  D0,RND+1(A6)
       LSL.B   #1,D0              ;*2I
       MOVE.B  D0,RND+3(A6)
       MOVE.B  SEED+1(A6),D1
       ADDX.B  D1,D0              ;6502 IS MUCH MORE COMPACT!!
       MOVE.B  D0,RND+4(A6)
       LSR.B   #1,D0
       MOVE.B  D0,RND+5(A6)

      ENDC

       RET

INITRND3 EQU INITRND-32           ;USED BY GAME EN/DECRYPT
 
*--------------------------------
*
*  RNDGEN - RETURNS A PSEUDO      !!! THIS IS THE ONE TO CALL !!!
*           RANDOM NO. IN D0.W
*
*  SPEEDWISE - CAN DO ABOUT $800
*              WORDS A SECOND
*
*--------------------------------
 
 
 
RNDGEN
      IFNE     M68000

       PUSH_L  D1/D7/A0/A6  ;SAVE SCRATCH
       LEA     RND.DATA(PC),A6    ;GET POINTER TO DATA
       MOVE.W  RND.CHIN(A4),D0    ;INTRODUCE A BIT OF REAL WORLD
       ADD.W   D0,RND(A6)         ;TO AVOID PREDICTABLE SEQUENCE
       CALL_S  RNDNUM             ;SHUFFLE AND GET A BYTE
       MOVE.B  D0,RND.ACC(A6)     ;SAVE IT
       CALL_S  RNDNUM             ;SHUFFLE SOME MORE, ANOTHER BYTE
       MOVE.B  D0,RND.ACC+1(A6)   ;ZZZZzzzz..
       MOVE.W  RND.ACC(A6),D0     ;GRAB RANDOM WORD
       PULL_L  D1/D7/A0/A6  ;GET BACK SAVED STUFF

      ENDC

      IFEQ  M68000

       DC.W  RNDTRAP
       ASL.W #8,d0
       DC.W  RNDTRAP

      ENDC

       RET                        ;BYE... 
 
 
*--------------------------------
*
* RNDNUM - SHUFFLE PACK, RETURN A 
*          RANDOM NO. IN D0.B
*
*--------------------------------
 
 
RNDNUM
      IFNE     M68000

       MOVE.B  RND(A6),D0         ;THIS IS REALLY JUST TO DEMO
       MOVE.B  RND+4(A6),D1       ;HOW YUK A 68K IS WHEN USING
       ADDX.B  D1,D0              ;BYTEWISE DATA (HONEST)
       MOVE.B  RND+5(A6),D1
       ADDX.B  D1,D0
       MOVE.B  D0,RND(A6)
 
       MOVEQ   #4,D7
       LEA     RND.DATA+RND(PC),A0
10$
       MOVE.B  0(A0,D7),D0
       MOVE.B  D0,1(A0,D7)
       SUBQ.L  #1,D7
       BPL.S   10$

      ENDC
      IFEQ     M68000

      DC.W     RNDTRAP

      ENDC

       RET
 
*--------------------------------
*
*  DATA
*
*  THIS *MUST* NOT BE PUT IN THE 
*  NORMAL DATA AREA SINCE THE 
*  ENCRYPTION ON SAVE/LOAD GAMES
*  USES IT 
*
 
RND.DATA       DS.B  10
RND.ACC        EQU  0
SEED           EQU  2
RND            EQU  4
 
*--------------------------------
*
* RND.D1 - THIS IS HOW THE ADVENTURE
*          USUALLY GETS A RANDOM NO
*
* ENTRY: D1.B - MAX VALUE OF RND NO.
*               REQUIRED+1
*
* EG. D1=10 RETURNS D1 BETWEEN 0 & 9
*
* EXIT: D1.B - RANDOM
*
* DUNNO HOW QUICK THIS IS AJS 
* RECONS ITS REAL SLOW
*
*--------------------------------
 
RND.D1
      IFNE  M68000

       MOVE.W  D0,-(A7)           ;SAVE SCRATCH
       CALL_S  RNDGEN             ;RETURNS D0.W
       MULU    D0,D1              ;MULT BY RANGE
       SWAP    D1                 ;DIV BY $10000
       AND.W   #$FF,D1            ;MAKE BYTE WISE
       MOVE.W  (A7)+,D0           ;RECOVE SCRATCH


      ENDC

      IFEQ  M68000

       DC.W    RNDTRAP2

      ENDC

       RET

       END

