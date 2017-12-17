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
*	@(#)charin.asm	2.8
*	9/16/87
*
******************************************************

		SECTION		"ADV",CODE

*-----------------------------
*
*  CHARIN
*
*-----------------------------

      INCLUDE  "equates.asm"
      INCLUDE  "macros.asm"


*
* this program will return d1 as zero if no character is present
*
* or it will return d1 as character - or as control code mapped to the
*
* QL keyboard.
*

        XDEF    GETCHAR2


      IFNE     FunctionKeys
        XREF    FUNCBASE,FUNCTNO,FUNCPNTR
      ENDC

       IFNE    Wimp
        XREF    MS_BUTT
       ENDC

    IFNE   M68000
      IFNE     SpoolLen
        XREF    SPOOLOUT
      ENDC
    ENDC


        XREF    MORE_CNT,RND.CHIN


       IFNE    AMIGA512
*           INCLUDE  "info.amg"
            INCLUDE  "exec/types.i"
            INCLUDE  "exec/libraries.i"            ;NB path names '\' = '/'


FUNCDEF     MACRO    * function

_LVO\1      EQU      FUNC_CNT
FUNC_CNT    SET      FUNC_CNT-6

            ENDM

FUNC_CNT    SET      LIB_NONSTD

            INCLUDE  "exec/exec_lib.i"

            INCLUDE  "libraries/dos.i"
            INCLUDE  "libraries/dos_lib.i"



         XREF        CSI_FLAG,CSI_PIPE,lib.dos,buffer
         XREF        ScrnMode,ConRead
       ENDC

       IFNE    MAC512

        INCLUDE "macequ.mac"

        XREF   COUT,ENTRY_A5,ENTRY_A6,MACSHELL
        XREF   PRTHEX
CHARIN:
        CLR.W   MORE_CNT(A6)
        MCALL   STATUS                   ;gets the character
        CALL    MS_BUTT
        CLR.W   D1
        MOVE.L  MACSHELL(A6),A0
        MOVE.W  MKEY(A0),D1
        CMP.B   #-1,D1
        BNE.S   10$
        CLR.W  D1
        BRA.S  99$

10$

        CMP.W   #$FF5E,D1               ;special case for funct 6!
        BNE.S   20$
        MOVE.B  #-6,D1
        BRA.S   99$
20$
        LEA     MACCHARTABLE(PC),A0
30$
        MOVE.B  (A0)+,D0
        BEQ.S   99$
        CMP.B   D0,D1
        BEQ.S   40$
        ADDQ.W  #1,A0
        BRA.S   30$
40$
        MOVE.B  (A0)+,D1
99$
        RET



MACCHARTABLE:

        DC.B    $ED,200,$B6,200         ;Right arrow
        DC.B    $1D,200                 ;mac+ right arrow
        DC.B    $EE,216,$C4,216         ;down arrow
        DC.B    $1F,216                 ;Mac + down arrow
        DC.B    $81,208,$8C,208         ;up arrow
        DC.B    $1E,208                 ;MAC + down arrow
        DC.B    $EC,192,$A7,192         ;left arrow
        DC.B    $1C,192                 ;MAC+ left arrow
        DC.B    $AB,202,$E6,202         ;S right arrow
        DC.B    $2A,202                 ;MAC
        DC.B    $A8,218,$E7,218         ;S down arrow
        DC.B    $3D,218                 ;MAC
        DC.B    $E4,210,$FD,210         ;S up arrow
        DC.B    $2F,210                 ;MAC
        DC.B    $E5,194,$B7,194         ;S left arrow
        DC.B    $2B,194                 ;MAC +
        DC.B    $A4,27,$23,27           ;Escape
        DC.B    $60,27,$7E,27           ;` AND ~ BECOME ESC (MAC+)
        DC.B    $A3,$23                 ;pound become #!
        DC.B    $C1,-1,$DA,-1           ;FUNCT 1 ...
        DC.B    $DB,-2,$AA,-2
        DC.B    $DC,-3,$FC,-3
        DC.B    $DD,-4,$A2,-4
        DC.B    $DE,-5,$B0,-5
        DC.B    $DF,-6
        DC.B    $E0,-7,$A6,-7
        DC.B    $E1,-8,$A5,-8
        DC.B    $E2,-9,$BB,-9
        DC.B    $E3,-10,$BC,-10         ;... TO 10
        DC.B    0,0

       ENDC
       IFNE    ST520


*
*  CHARIN gets a character from the keyboard, returns D1 = char, or 
*         D1 = 0 if no character typed, also return QL like chars
*         to emulate arrow keys and -(1-10) as function keys
*


CHARIN
        CLR.W           MORE_CNT(A6)      ;The more counter - zerooed
        CALL            MS_BUTT       ;Have they pushed a mouse button?

        PUSH_L          D0/D2-D7/A0-A6

        MOVE.W          #2,-(SP)      ;?? console device
        MOVE.W          #1,-(SP)      ;?? get device status

        TRAP            #13

        ADDQ.L          #4,SP

        TEST_W          D0            ;is there a character there?
        BEQ.S           20$           ;EQ => no

        MOVE.W          #2,-(SP)      ;yes, console again
        MOVE.W          #2,-(SP)      ;read device

        TRAP            #13

        ADDQ.L          #4,SP

        CLR.L           D1
        MOVE.W          D0,D1
        BNE.S           10$           ;NE => normal key type

        SWAP            D0            ;If funct key info in HI word
        LEA             CONVTAB(PC),A0
        BTST            #10,D0        ;?? control key pressed?
        BEQ.S           05$
        LEA             CONVTAB2(PC),A0
05$
        MOVE.B          (A0)+,D1      ;?? does this seach the table?
        MOVE.B          (A0)+,D2
        BEQ.S           15$
        CMP.B           D0,D2
        BNE.S           05$
10$
        BRA.S           30$           ;???????????????????????????????????
15$
        SWAP            D0
        MOVE.B          D0,D1
        BRA.S           30$
20$
        CLR.L           D1
30$
        CMP.B           #13,D1       ;'CR' ?
        BNE.S           35$
        MOVEQ           #10,D1
35$
        PULL_L          D0/D2-D7/A0-A6
        RET


CONVTAB    DC.B    -1,$3B,-2,$3C,-3,$3D,-4,$3E,-5,$3F
           DC.B    -6,$40,-7,$41,-8,$42,-9,$43,-10,$44
           DC.B    208,$48,200,$4D,216,$50,192,$4B,0,0

CONVTAB2   DC.B    -1,$3B,-2,$3C,-3,$3D,-4,$3E,-5,$3F
           DC.B    -6,$40,-7,$41,-8,$42,-9,$43,-10,$44
           DC.B    210,$48,202,$74,218,$50,194,$73,0,0



         ENDC


         IFNE     AMIGA512



CHARIN


            PUSH_L   D0/D2-D7/A0-A6
            CLR.W    MORE_CNT(A6)            ;clear the more signal!
            CALL     MS_BUTT                 ;button?

10$
            CALL     ConRead
            TEST_B   D0                      ;0 => no char avail
            BNE.S    20$                     ;ok, do a read

            MOVEQ    #0,D1                   ;no char pending, return 0
            BRA.S    90$

20$
            MOVEQ    #0,D1
            MOVE.B   D0,D1

            TEST_B   CSI_FLAG(A6)            ;Have we recieved a CSI?
            BEQ.S    80$                     ;NO

            CALL     DO_PIPE                 ;Put char in pipe, check matches
            BEQ      10$                     ;got a char at last? EQ => no


* Check for start of 'command' sequences (ESC, Function keys, cursor keys)

80$
            CMP.B    #$9B,D1                 ;$9B = Amiga command sequence introducer
            BNE.S    90$
            MOVE.B   #1,CSI_FLAG(A6)         ;flag that pipe is now in use
            CLR.L    CSI_PIPE(A6)            ;empty pipe
            BRA      10$                     ;and look for next char

90$
            PULL_L   D0/D2-D7/A0-A6
            RET

*
* DO_PIPE - checks pipe against possible matches
*



DO_PIPE

            MOVE.B   CSI_FLAG(A6),D0
            EXT.W    D0
            CMP.B    #4,D0                   ;overflow?
            BNE.S    05$

            CLR.B    CSI_FLAG(A6)
            MOVEQ    #$20,D1                 ;ouch! - fake a space
            BRA.S    90$

05$
            ADDQ.B   #1,CSI_FLAG(A6)
            LEA      CSI_PIPE(A6),A0
            MOVE.B   D1,-1(A0,D0)            ;Put char in 'pipe'

            MOVE.L   CSI_PIPE(A6),D0

            LEA      PipeMatchList(PC),A0
10$
            MOVE.W   (A0)+,D1                ;char to use if we have a match
            BEQ.S    90$                     ;EQ => end of list
            CMP.L    (A0)+,D0                ;Do we have a match?
            BNE.S    10$

            CLR.B    CSI_FLAG(A6)
            TEST_W   D1                      ;set NE

90$
            RET


PipeMatchList

            DC.B     0,-1,'0~',0,0           ;Function keys, unshifted
            DC.B     0,-2,'1~',0,0
            DC.B     0,-3,'2~',0,0
            DC.B     0,-4,'3~',0,0
            DC.B     0,-5,'4~',0,0
            DC.B     0,-6,'5~',0,0
            DC.B     0,-7,'6~',0,0
            DC.B     0,-8,'7~',0,0
            DC.B     0,-9,'8~',0,0
            DC.B     0,-10,'9~',0,0

            DC.B     0,-1,'10~',0             ;function keys, shifted
            DC.B     0,-2,'11~',0
            DC.B     0,-3,'12~',0
            DC.B     0,-4,'13~',0
            DC.B     0,-5,'14~',0
            DC.B     0,-6,'15~',0
            DC.B     0,-7,'16~',0
            DC.B     0,-8,'17~',0
            DC.B     0,-9,'18~',0
            DC.B     0,-10,'19~',0


*  !!!!!!!!!!!!!!!!!!
*
*  NB Amiga documentation lies about the '~' in cursor key strings!!!!!
*



* Non shifted cursor keys...

            DC.B     0,208,'A',0,0,0          ;QL compatible CTRL-codes!!
            DC.B     0,216,'B',0,0,0          ;DOWN
            DC.B     0,192,'D',0,0,0          ;LEFT
            DC.B     0,200,'C',0,0,0          ;RIGHT

* Shifted cursor keys...

            DC.B     0,210,'T',0,0,0          ;SHIFT-UP
            DC.B     0,218,'S',0,0,0          ;SHIFT-DOWN
            DC.B     0,194,' A',0,0           ;SHIFT-LEFT
            DC.B     0,202,' @',0,0           ;SHIFT-RIGHT


* Help key can map to a space for today

            DC.B     0,$20,'?~',0,0



            DC.W     0,0,0                   ;terminator (+fencepost)



         ENDC

GETCHAR2

    IFNE	M68000
      ADDQ.W   #1,RND.CHIN(A6)   ;Add a taste of reality to random no.s
    ENDC 

      MOVE.L   A0,-(A7)

     IFNE      FunctionKeys
      MOVE.B   FUNCTNO(A6),D1    ;Has a function key been pressed lately
      BNE.S    10$               ;Yes, get a charcter from it
     ENDC

    IFNE	M68000
      CALL     CHARIN            ;Char to D1
    ENDC

    IFEQ	M68000
     IFNE	IBM
      XREF	SCR_IGNO
      MOVE.B	SCR_IGNO(PC),D1		;NE => ignore scrolls
     ENDC

      DC.W	$A000		;charin trap
      CMP.B	#13,D1		;fod <CR> to <LF>
      BNE.S	10$
      MOVEQ	#10,D1
10$
    ENDC

     IFNE      FunctionKeys
      CMP.B    #-11,D1           ;Function key?
      BLS.S    90$               ;No, exit

* We've got a function key, lets stuff it into inbuff by emulating CHARIN

      CLR.W    FUNCPNTR(A6)      ;If A6 gets stomped....
      MOVE.B   D1,FUNCTNO(A6)
10$      
      NOT.B    D1                ;negate and adjust (sub#1)
      LSL.W    #8,D1             ;256 Bytes per key
      ADD.W    FUNCPNTR(A6),D1   ;Current position
      LEA      FUNCBASE,A0       ;Range errors!
      ADD.L    A6,A0
      ADDQ.W   #1,FUNCPNTR(A6)   ;Update posn in funct. key buffer
      MOVE.B   0(A0,D1.W),D1     ;Get char
      BNE.S    90$
      CLR.W    FUNCTNO(A6)       ;At end of function key string
90$
      AND.W    #$FF,D1
     ENDC

    IFNE    M68000		;else native code can cope!
     IFNE      SpoolLen
      MOVE.L   A4,-(A7)
      MOVE.L   A6,A4
      LEA      SPOOLOUT,A0
      JSR      0(A6,A0.L)
      MOVE.L   (A7)+,A4
     ENDC
    ENDC

      MOVE.L   (A7)+,A0

      RET

      END

