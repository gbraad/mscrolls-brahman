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
*	@(#)decom.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.DECOM1
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

       XREF   CHAROUT,CHAROUT2,PUNCT,SPACE,STOPRON,W.IT
       XREF   GETDATA,P.TN,PLURAL,CHANNEL

       XDEF   FMSG,DOAPRON

      IFEQ   (DummyText!ExternText)
       XREF   MSGPNTR
      ENDC

       
DATA_          EQU  $A
TEXTST         EQU  $8
INDEXST        EQU  $4
TREEST         EQU  $2
TOKENST        EQU  $6
TOPNODE        EQU  $0
       
*--------------------------------
       
ATPRON         EQU  $88
 
*--------------------------------

       IFEQ   (DummyText!ExternText)
       
GTA
       
*GET TEXT ADDRES
       
*ENTRY:
*D0=MESG#
       
*EXIT:
*A2=ADDR OF TEXT.
       

       PUSH_L  D0-D1/A1/A3

      IFNE  YCHEAT

   XREF  PRTHEX,CHEATNUM

       TEST_W   CHEATNUM(A4)              ;DEBUG JEANS
       BEQ.S    05$

       PUSH_L   D0-D7/A0-A6
       MOVEQ    #0,D6
       MOVE.W   D0,D6
       CALL     PRTHEX
       MSG     SPACE
       PULL_L   D0-D7/A0-A6
05$
      ENDC

       LEA     MSGPNTR,A2
       ADD.L   A4,A2
       MOVE.L  A2,A1                ;copy address of start of text info

       ADD.W   TEXTST(A2),A2        ;START OF TEXT.

       MOVE.L  A1,A3                ;SAVE
       ADD.W   INDEXST(A1),A1       ;START OF INDEX TABLE
       MOVEQ   #0,D1
       TEST_W  DATA_(A3)
       BEQ.S   10$                  ;LONG
       
       ADD.W   DATA_(A3),A3         ;ADD OF TEXT, A2 IS REALLY HASH PNTR
       SUBQ.W  #1,D0                ;ADJUST
       BEQ.S   30$                  ;DONE ALREADY
15$
       SUB.W   #$80,D0
       BMI.S   20$                 ;DONE
       MOVE.W  (A2)+,D1            ;GET HASH POINTER
       LEA     $80(A1),A1          ;ADJUST INDEX   (ADD.L #$80,A1)
       BRA.S   15$                 ;CONTINUE
20$
       ADD.L   D1,A3               ;TXT ADR OF HASH
       CLR.W   D1
       ADD.W   #$80,D0             ;REMAINDER
       BEQ.S   30$                 ;ON EXACT BOUNDARY?
21$
       MOVE.B  (A1)+,D1            ;GET LENGTH
       ADD.W   D1,A3               ;CALC ADR
       SUBQ.B  #1,D0               ;UNTIL NONE
       BNE.S   21$                 ;CONTINUE
30$
       MOVE.L  A3,A2
       BRA.S   90$                 ;EXIT
10$
       SUBQ.W  #$2,D0
       BMI.S   90$                 ;DONE
       LSL.W   #1,D0               ;*2
       MOVE.W  0(A1,D0.W),D1       ;OFFSET
       ADD.L   D1,A2               ;ADDR OF TEXT
90$
       PULL_L  D0-D1/A1/A3
       RET


       ENDC

*--------------------------------
   
FMSG
       
*PRINT MESAGE# 
       
*ENTRY:
*D0=MESG#

      IFNE     DummyText
       MOVE.L  D1,-(A7)
       MOVEQ   #'!',D1
       DO	CHAROUT
       MOVE.L  (A7)+,D1
       RET
      ENDC


      IFEQ     (DummyText!ExternText)

       PUSH_L  D0-D7/A0-A6
       CALL    GTA                   ;GET TEXT ADRESTO  A2

       LEA     MSGPNTR,A1
       ADD.L   A4,A1
       MOVE.L  A1,-(A7)             ;Save for subsequent use

       ADD.W   TOPNODE(A1),A1
       MOVE.W  (A1),D5               ;TOPNODE

       MOVE.L  (A7)+,A1

       ADD.W   TREEST(A1),A1    ;START OF TREE
       MOVEQ   #7,D6            ;BIT#
       
       LEA     FASTLIST(PC),A3    ;START OF LIST
01$
       MOVEQ   #0,D4              ;ZERO PIPE
       MOVEQ   #4,D2              ;ZERO COUNT
10$
       
       TEST_B  PUNCT(A4)   
       BEQ.S   12$                  ;NO-PUNCTUATION
       PUSH_L  D6/A2          ;SAVE REGS USED TO POINT AT CURR. BIT
       CALL    UNDOSMALL            ;GET A NEXT CHAR FROM BIT STREAM
       PULL_L  D6/A2          ;BRING EM BACK
       CMP.B   #$20,D7              ;SPACE & CTRL NEXT = DO A SPACE NOW
       BLE.S   07$     
       CMP.B   #'A',D7              ;IS CHAR ALPHABETIC?
       BLT.S   12$                  ;LT = NO = DONT INSERT A SPACE
07$
       MOVEQ   #$20,D7              ;PRINT A SPACE
       BRA.S   11$
12$
       CALL    UNDOSMALL           ;DECODE 1 CHR, INTO D7
11$
       CALL    ISPUNC              ;ON D7
19$
       
       LSL.L   #8,D4               ;SHIFT THE PIPE
       MOVE.B  D7,D4               ;IN NEXTCHR
       MOVE.L  D4,D1               ;COPY
       AND.L   #$FFFFFF,D4         ;ZAP THE TOP OF THE PIPE.
       ROL.L   #8,D1              ;GET HI BYTE
       EXT.W   D1                 ;WORDWISE
       BEQ.S   20$                ;HUM, A ZEO?
       BMI.S   50$                ;D1 IS A TOKEN
       CALL    CHAROUT2           ;DONT ZAP PUNCT
31$
       MOVE.L  A3,A5              ;START OF LIST
       MOVE.W  #$80,D1            ;TOKEN#
21$
       MOVE.L  (A5)+,D3           ;GET FAST
       BEQ.S   10$                ;END- NO MATCH
       ADDQ.B  #1,D1              ;INC TOKEN#
       CMP.L   D3,D4              ;MATCH??
       BNE.S   21$                ;NO
 
       CMP.B   #ATPRON,D1
 
       BNE.S   14$
       CALL    DOAPRON
   
       BRA.S   01$
  
14$
  
       CALL    UNTOKEN           ;PRINT
13$
       MOVE.B  D1,D7
09$
       MOVEQ   #0,D4
       MOVEQ   #4,D2
       BRA.S   11$
  
  
50$
       CALL    UNTOKEN           ;AND PRINT
       MOVE.B  D4,-(A7)          ;SAVE ENTRY
       CLR.B   D4                ;ZAP END
       ROL.L   #8,D4     
       MOVE.B  D1,D4             ;LAST CHR
       SWAP    D4                ;REARANGE
       MOVE.L  A3,A5             ;THIS REPEAT IS FOR SPEED
       MOVE.W  #$80,D1
51$
       MOVE.L  (A5)+,D3          ;GET MATCH
       BEQ.S   52$               ;NONE FOUND
       ADDQ.B  #1,D1             ;TOKEN#
       CMP.L   D3,D4             ;MATCH?
       BNE.S   51$               ;CONTINUE
       CMP.B   #ATPRON,D1
       BEQ.S   53$
       CALL    UNTOKEN           ;PRINT ALL BUT LAST (IN D1)
       MOVEQ   #0,D4             ;CLEAR PIPE
       MOVE.B  D1,D4             ;PUT IN LAST ONE OF TOKEN
       MOVEQ   #3,D2             ;ONLY 3 BLANKS ALLOWED
       BRA.S   52$               ;AND RETURN
53$
       CALL    DOAPRON
       MOVEQ   #0,D4
       MOVEQ   #4,D2
52$
       MOVE.B  (A7)+,D7
       BRA.S   19$               ;CONTINUE WO GETTING ANOTHER
20$
       SUBQ.W  #1,D2             ;DEC ZEROS
       BNE.S   31$
90$
       PULL_L  D0-D7/A0-A6
       RET

      ENDC

      IFNE     ExternText

       PUSH_W  D0/D2-D3

      IFNE  YCHEAT

   XREF  PRTHEX,CHEATNUM

       TEST_W   CHEATNUM(A4)              ;DEBUG JEANS
       BEQ.S    01$

       PUSH_L   D0-D7/A0-A6
       MOVEQ    #0,D6
       MOVE.W   D0,D6
       CALL     PRTHEX
       MSG     SPACE
       PULL_L   D0-D7/A0-A6
01$
      ENDC

       SUBQ.W  #1,D0
       MOVE.B  CHANNEL(A4),D3
       MOVE.B  PLURAL(A4),D2
       MOVE    #0,CCR	;Clear carry first time in
05$
       DC.W    $A0F8            ; Do_TEXT
       BCS.S   10$
       PULL_W  D0/D2-D3
       RET

10$
       CALL_S  DOAPRON
       MOVE.W  #1,CCR
       BRA.S   05$

      ENDC

*--------------------------------
*
*  SRC.ADV.DECOM2
*
*--------------------------------
       
       
DOAPRON 
*PRINT RELEVENT PRONOUN INSTEAD OF AT SYMBOL
       
       PUSH_L  D0/A0
       MSG    SPACE
       MOVE.W  STOPRON(A4),D0
       BEQ.S   50$                ;HUMM, NOT SPECIFIED
       GETINFO
       DO	P.TN
99$
       PULL_L  D0/A0        ;BAK
       RET                  
       
50$
       MSG	IT
       BRA.S   99$               ;EXIT
       
       
*--------------------------------

      IFEQ    (DummyText!ExternText)

UNDOSMALL
 
       MOVE.W  D5,D7
       LSL.W   #1,D7              ;OFFSET INTO TREE (2 BYTES/NODE)
10$
       BTST    D6,(A2)            ;TEST BITSTREAM 
       BNE.S   20$                ;LEFT OR RIGHT?
       MOVE.B  1(A1,D7),D7        ;RIGHT = BIT NOT SET
       BRA.S   30$
20$
       MOVE.B  0(A1,D7),D7        ;LEFT = BIT SET
30$
       SUBQ.W  #1,D6              ;NEXT BIT
       BPL.S   40$
       ADDQ.L  #1,A2
       MOVEQ   #7,D6              ;START AT TOP BIT AGAIN
40$
       AND.W   #$FF,D7            ;'LSL' NOBBLES BITS 8-15
       LSL.W   #1,D7              ;GET OFFSET INTO TREE
       CMP.B   #-1,1(A1,D7)       ;ARE WE AT A LEAF NOW? 
       BNE.S   10$                ;NO
       MOVE.B  0(A1,D7),D7        ;GET THE DATA!
       AND.W   #$FF,D7            ;LSL'S KILL!   
       RET

*--------------------------------

UNTOKEN
       
*UNTOKENISE A BYTE
       
*ENTRY:
*D1=BYTE.
*A0=DESTINATION 
       
       PUSH_L  D0/D7/A1
       SUB.B   #128,D1
       EXT.W   D1
       CALL    GETADR          ;A1=TEXT
10$
       MOVE.B  (A1)+,D1        ;GETCHR
       TEST_B  (A1)            ;NEXT?
       BEQ.S   20$             ;DONE
       CALL    CHAROUT2
       BRA.S   10$             ;NEXT
20$
       PULL_L  D0/D7/A1
       RET

*--------------------------------
   
GETADR
       
*GET ADRRES OF TOKEN
       
*ENTRY:
*D1=TOKEN#
       
*EXIT:
*A1=ADDR OF TOKEN
       
       LEA     MSGPNTR,A1
       ADD.L   A4,A1

       ADD.W   TOKENST(A1),A1     ;START
       SUBQ.B  #1,D1              ;START FROM 1
       BEQ.S   90$                ;DONE
10$
       TEST_B  (A1)+              ;END?
       BNE.S   10$                ;NO
       SUBQ.B  #1,D1              ;DONE?
       BNE.S   10$                ;NO
90$
       RET

*--------------------------------
               
FASTLIST
               
       DC.B     0,' e '                ;$20,'E'+32,$20
       DC.B     0,'   '                ;$20,$20,$20
       DC.B     0,' r '                ;$20,'R'+32,$20
       DC.B     0,' t '                ;$20,'T'+32,$20
       DC.B     0,' o '                ;$20,'O'+32,$20
       DC.B     0,' n '                ;$20,'N'+32,$20
       DC.B     0,' s '                ;$20,'S'+32,$20
       DC.B     0,' @ '                ;SPECIAL PRONOUN CHARACTER
       DC.L     0                      ;REALLY IS A long WORD!!!

*--------------------------------
       
ISPUNC
       
       
*DETERMINE WHETHER D7
*IS PUNCTUATION REQUIRING
*TRAILING SPACE.
       
*      PUSH_L  D1/A5        ;SAVE SCRATCH (FORGET - BOTH OK)
       TEST_B  PUNCT(A4)          ;SET ALREADY?
       SEQ     PUNCT(A4)          ;RESET IF NON ZERO
       BNE.S   90$                ;AND EXIT
 
       LEA     Punc_List(PC),A5         ;GET LIST ADDR
       MOVEQ   #5,D1              ;NO OF DIFF CHARS - 1   
10$
       CMP.B   (A5)+,D7           ;IS IT THIS ONE?   
       DBEQ    D1,10$             ;LOOP TILL WE GET ONE OR RUN OUT   
       SEQ     PUNCT(A4)          ;SET IF WE DIDNT RUN OUT
 
*      PULL_L  D1/A5        ;THEY *ALWAYS* STOMPED ANYHOW AFTER CALL

90$
       RET
 
Punc_List
       DC.B     '!,.;:?'           ;CHARS WI TRAILING SPACES

      ENDC
       END


