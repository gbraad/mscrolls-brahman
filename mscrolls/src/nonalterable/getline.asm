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
*	@(#)getline.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.KEN.GETLINE
*
*--------------------------------
 
*CURSOR.CHAR    EQU  $5F           ;UNDERLINE
*CURSTIME       EQU  -24           ;TIME BETWEEN SWAPING CUSROR 
*CHARIN         EQU  0  
*COUT           EQU  0   
*BUFLEN         EQU  $FF         NOW IN DATAFILE
*INBUFF         EQU  0   
  
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

BUFLEN   EQU   BUFFLEN-1

	XDEF    GETLINE,GETCHAR
	
	XREF    COUT,GETCHAR2,INBUFF,BIOS,CHAR.SUB,BIOSA6
	
      IFNE	M68000				;Very long cond bit! 
	XREF	CURSTIME,CUR.CHAR


      IFNE    Wimp
	XREF    SCR_PNTR,SCR_BUF,SCR_IGNO,LOCK_M,UNLOCK_M
	XREF    WH_SCR,SCROLLIT,END_MENU
      ENDC


      IFNE    ESC_edit
	XREF    VOLDBUF
      ENDC

      IFNE     SpeechAvail
	XREF    InGetLn
      ENDC


GETLINE

 
	PUSH_L  D0-D7/A0/A2-A5  

      IFNE     SpeechAvail
	LEA     InGetLn(A6),A0     ;so we don't try to say things mid
	ST      (A0)               ;edit 
      ENDC

	
	MOVEQ   #$20,D5            ;D5 = SPACE   
 
	LEA     INBUFF(A6),A0      ;GET BUFFER BASE POINTER
	MOVE.L  A0,A2              ;COPY TO CUR. POSN POINTER
	MOVE.L  A0,A1              ;COPY TO END OF LINE POINTER
	MOVE.W  #BUFLEN,D0         ;BUFFER SIZE
	LEA     -5(A0,D0.W),A5     ;MAZ BUFFER = BASE + LEN - A BIT
 
10$
	MOVE.B  D5,0(A0,D0)        ;CLEAR BUFFER TO SPACES 
	DBRA    D0,10$

      IFNE    Wimp
	TEST_L  SCR_PNTR(A6)       ;Is there a half typed command pending?
	BEQ.S   20$                ;Nope, carry on as normal
	CALL    SCROLL_RECOVER
      ENDC

20$
	CALL_S  GETCHAR_GETLINE    ;GET CHAR TO D1 (DOES CURSOR)
	
	LEA     FEATURES(PC),A3    ;LIST OF 'FEATURES'
	MOVEQ   #0,D2              ;CLEAR FOR ADDR BYTE
30$
	MOVE.B  (A3)+,D2           ;GET ADDR BYTE
	BEQ.S   40$                ;NOT A FEATURE - DO INSERT
	CMP.B   (A3)+,D1           ;CORRECT FEATURE?
	BNE.S   30$                ;NOPE, NEXT?
  
	JSR     -2(A3,D2.W)        ;DO THE FEATURE  (!!) 
	
	BRA.S   20$                ;LOOP SOME MORE
 
40$
	CMP.B   #32,D1             ;Control char?
	BMI.S   20$                ;MI => yes, so ignore
	TEST_B  D1		  ;hi bit set?
	BMI.S   20$

	CMP.L   A1,A5              ;TOO BIG?
	BEQ.S   20$                ;EQ = YES!
	ADDQ.L  #1,A1              ;INCREMENT END OF LINE POINTER
	MOVE.L  A1,A3              ;WHERE TO START MOVING FROM
50$
	MOVE.B  -(A3),1(A3)        ;MOVE UP A BYTE
	CMP.L   A2,A3              ;MOVED CHAR AT CUR. POSN.?
	BNE.S   50$                ;NOPE, DO TILL THEN
	MOVE.B  D1,(A2)+           ;PUT IN NEW CHAR
	CALL    COUT.NEAR          ;PRINT IT (& UPDATE CURSOR POS)
	CALL    REPRINT
	BRA.S   20$                ;LOOP FOR MORE
 
*--------------------------------
*
* GETCHAR - A2 POINTS AT CURRENT
*           POS IN INBUFF
*
* RETURNS CHAR IN D1.B
*
*--------------------------------

GETCHAR

	CALL	BIOSA6 		     ;Cos RUSURE & stuff use this

      IFNE    Wimp
	ST      SCR_IGNO(A6)       ;else VERY embarasing when u select a scroll
	MOVE.L  SCR_PNTR(A6),-(A7) ;Better not forget about any pending scroll
	CALL_S  GETCHAR_GETLINE
	MOVE.L  (A7)+,SCR_PNTR(A6)
	SF      SCR_IGNO(A6)      ;hmm had best avoid using this if SCR_IGNO set
	RET
      ENDC


GETCHAR_GETLINE
 
*      PUSH_L  D6/D7        ;NOT REALLY NEEDED

      IFEQ    AMIGA512
      IFNE    Wimp
	CALL    UNLOCK_M           ;Liberate the mouse
      ENDC
      ENDC

	MOVEQ   #-1,D7             ;SET CURSOR 'ON' INITIALLY
10$
	MOVE.W  #CUR.CHAR,D1       ;UNDERLINE
20$
	CALL    COUT.NEAR
	DO	UTIL.BS	           ;PUT CURSOR (VIDEO) BACK
	MOVE.W  #CURSTIME,D6       ;TIME BETWEEN CHANGE
30$
	CALL    GETCHAR2     ;CHARIN             ;GET A CHAR - INTO D1.B

      IFNE    Wimp
	TEST_B  SCR_IGNO(A6)       ;Are we ignoring the pulldown scrolls?
	BEQ.S   40$                ;NE => we are, so lets FOD any that are there
	CLR.L   SCR_PNTR(A6)
40$
	TEST_L  SCR_PNTR(A6)       ;Have the silly sods hit a SCROLL command?
	BNE     SCROLL_COMMAND
      ENDC

	TEST_B  D1                 ;WAS A KEY PRESSED?
	BNE.S   90$                ;YES!
	SUBQ.W  #1,D6              ;CURSOR TIMER
	BNE.S   30$                ;LOOP SOME MORE
	NOT.B   D7                 ;OK - TIME TO FLASH CURSOR
	BNE.S   10$                ;USE CURSOR CHAR THIS TIME
	MOVE.B  (A2),D1            ;USE BUFFER CHAR THIS TIME
	BRA.S   20$                ;GO DO IT!
 
90$
	 IFNE    Wimp
* no scrolls allowed to be pulled down when there is a character pressed!

	TEST_B  WH_SCR(A6)
	BEQ.S   91$
	PUSH_L  D0-D7/A0-A6
	CALL    SCROLLIT
	CALL    END_MENU
	PULL_L  D0-D7/A0-A6
91$
	 ENDC

	TEST_B  D7                 ; Was the cursor there?
	BEQ     95$                ; No.. so simple exit
	MOVE.W  D1,-(SP)           ; Save our character
	MOVE.B  (A2),D1            ; Replace with char at cursor
	CALL_S  COUT.NEAR          ; ... and backspace
	CALL_S  UTIL.BS
	MOVE.W  (SP)+,D1           ; Restore our character
95$

      IFEQ    AMIGA512
      IFNE    Wimp
	CALL    LOCK_M             ; Lock out the mouse
      ENDC
      ENDC

*      PULL_L  D6/D7        ;RECOVER REG'S - NOT NEEDED!
	RET
 
FEATURES

	   DC.B    DEL.R-*,127    ;DELETE KEY
	   DC.B    DEL_L-*,8      ;BACKSPACE KEY

	   DC.B    MOVE.R-*,200   ;RIGHT ARROW = MOVE RIGHT
	   DC.B    MOVE_L-*,192   ;LEFT ARROW = MOVE LEFT
	   DC.B    DEL.R-*,202    ;CTRL-RIGHT = DEL CHAR TO RIGHT
	   DC.B    DEL_L-*,194    ;CTRL-LEFT = DEL CHAR TO LEFT
	   DC.B    ENDLINE-*,10   ;'ENTER' = FINISHED

	  IFNE     AMIGA512!MAC512

	   DC.B    ENDLINE-*,13   ;REAL 'CR'

	  ENDC


	   DC.B    MOVE.WL-*,208  ;UP ARROW = MOVE WORD LEFT
	   DC.B    MOVE.WR-*,216  ;DOWN ARROW = MOVE WORD RIGHT
	   DC.B    DEL.WL-*,210   ;CTRL-UP = DEL WORD LEFT
	   DC.B    DEL.WR-*,218   ;CTRL-DOWN = DEL WORD RIGHT

	  IFNE    ESC_edit

	   DC.B    REEDIT-*,$1B   ;ESC = Re-edit last command

	  ENDC

	   DC.W    0              ;END OF FEATURES
 
MOVE.WL        
  
10$
	CALL_S  MOVE_L             ;MOVE 1 CHAR LEFT
	CMP.L   A2,A0              ;NOT PAST START OF LINE
	BEQ.S   90$
	CMP.B   -1(A2),D5          ;AT A SPACE YET? 
	BNE.S   10$
	CMP.B   (A2),D5            ;AT A SPACE?
	BEQ.S   10$                ;YES = TRY SOME MORE..
90$
	RET
 
MOVE_L
	
	CMP.L   A2,A0              ;START OF LINE?
	BEQ.S   RETURN.NEAR        ;CANT GO PAST START
*      MOVE.B  (A2),D1            ;INCASE OF CURSOR ON SCREEN LAST
*      CALL_S  COUT.NEAR
*      CALL_S  UTIL.BS
*      BRA.S   UTIL.BS            ;UPDATE CURSOR (VIDEO) TO BE AS POSN
	SUBQ.L  #1,A2              ;UPDATE POSN IN BUFFER

 
UTIL.BS
 
	MOVE.L  D1,-(A7)
	MOVEQ   #8,D1              ;CTRL-H
	CALL_S  COUT.NEAR
	MOVE.L  (A7)+,D1
 
RETURN.NEAR
 
	RET
  
MOVE.R
	
	CMP.L   A2,A1
	BEQ.S   RETURN.NEAR        ;CAN'T GO PAST END OF LINE 
	MOVE.B  (A2)+,D1           ;UPDATE POSN,GET CHAR AT OLD POS
 
COUT.NEAR
	
	BRA     COUT               ;IN CASE CURSOR WAS ON SCREEN
 
 
DEL_L
 
	CMP.L   A2,A0              ;NOT AT START
	BEQ.S   RETURN.NEAR
DEL_L2
  
	CALL    DEL.DO             ;!!!!
	CALL_S  UTIL.BS
 
REPRINT.NEAR
  
	BRA.S   REPRINT
 
MOVE.WR
 
10$
	CALL_S  MOVE.R             ;MOVE 1 CHAR RIGHT
	CMP.L   A2,A1              ;AS FAR AS WE CAN GO
	BEQ.S   90$
	CMP.B   -1(A2),D5          ;R WE TO RIGHT OF A SPACE?
	BNE.S   10$                ;NOPE
	CMP.B   (A2),D5            ;R WE ON A SPACE?
	BEQ.S   10$                ;YES - NEXT...
90$
	RET
  
DEL.R
 
	CMP.L   A2,A1              ;NOT AT END!
	BEQ.S   RETURN.NEAR
	CALL_S  MOVE.R             ;MOVE RIGHT 1
	BRA.S   DEL_L2             ;DEL,BS,REPRINT
 
DEL.WL
 
	MOVE.L  A2,A3              ;SAVE OLD CURS POS
	CALL_S  MOVE.WL            ;GET FIRST SPACE TO LEFT
	CMP.L   A2,A3              ;NO CHANGE = RET
	BEQ.S   RETURN.NEAR
	CALL_S  DEL.DO2
	MOVE.L  A4,A1              ;NEW END OF BUFFER
20$
	MOVE.B  D5,(A4)+           ;ZAP JUNK AT END OF BUFFER
	CMP.L   A3,A4              ;GOT TO OLD BUF. END YET?.
	BNE.S   20$                ;NOPE
	SUB.L   A1,A4              ;CALC NO. OF SPACES NEEDED TO 
	MOVE.L  A4,D0              ;BLAT OVER OLD STUFF 
	
	BRA.S   REPRINT2           ;REDO SCREEN
 
 
DEL.WR
  
	MOVE.B  D5,(A2)            ;MARK CURS POS IN CASE IN MIDDLE OF WRD
05$
	CMP.L   A2,A1              ;AT THE END ALREADY?
	BEQ.S   RETURN.NEAR
	CMP.B   (A2),D5            ;MOVE RIGHT TILL WE FIND A WORD TO 
	BNE.S   10$                ;DELETE!  (PS-NE=WE FOUND ONE!)
	CALL_S  MOVE.R             ;OK - STILL ON SPACES = GO RIGHT
	BRA.S   05$                ;END OF LINE ETC?,,   
10$
	CALL_S  MOVE.WR            ;MOVE WORD RIGHT  
	CALL_S  DEL.WL             ;DELETE WORD LEFT
	BRA.S   DEL_L              ;KILL SPACE WE PUT IN
 
REPRINT
	
	MOVEQ   #0,D0              ;PRINT NO SPACES

*
* REPRINT2 - reprints line to right of cursor (A2) then prints D0 spaces
*            to stomp over junk

REPRINT2
 
	MOVE.L  A2,A4              ;CURRENT POSN/CURSOR
10$
	MOVE.B  (A4)+,D1           ;LEFT AS SPACE AT END!
	CALL_S  COUT.NEAR
	CMP.L   A4,A1
	BPL.S   10$
 
	MOVE.B  D5,D1              ;PUT A SPACE IN
20$
	CALL_S  COUT.NEAR          ;PRINT A SPACE
	ADDQ.L  #1,A4              ;KEEP TRACK FOR NO. OF BS'S TO DO
	DBRA    D0,20$
 
30$
	CALL_S  UTIL.BS
	SUBQ.L  #1,A4              ;COUNT
	CMP.L   A4,A2              ;ENOUGH?
	BNE.S   30$                ;NOPE..
	
	RET                        ;YES
  
ENDLINE 

	ADDQ.L  #4,A7              ;Came here via a JSR

	CLR.B   (A1)               ;Mark end of line

      IFNE    ESC_edit
	LEA     VOLDBUF(A6),A3     ;Save a copy of the buffer for use via ESC
10$
	MOVE.B  (A0)+,(A3)+
	BNE.S   10$
      ENDC

20$
	MOVE.B  (A2)+,D1
	BEQ.S   30$
	CALL    COUT
	BRA.S   20$
30$

EXIT_GETLINE

	CLR.B   (A1)+              ;END LINE WITH A #00

      IFNE     SpeechAvail
	LEA     InGetLn(PC),A0     ;back to '.' => time to speak
	MOVE.B  #1,(A0)            ;Flag next LF = do speech
      ENDC

	PULL_L  D0-D7/A0/A2-A5
	RET
 
DEL.DO
 
	MOVE.L  A2,A3              ;GET START OF AREA TO MOVE
	SUBQ.L  #1,A2
 
DEL.DO2
 
	MOVE.L  A2,A4              ;GET BYTE TO MOVE TO
10$
	CMP.L   A3,A1              ;AT END OF LINE?
	BEQ.S   90$                ;YES, FINISHED
	MOVE.B  (A3)+,(A4)+        ;MOVE ONE BYTE LEFT
	BRA.S   10$                ;DO MORE
90$
	MOVE.B  D5,-(A1)           ;UPDATE END OF LINE POINTER
	RET


      IFNE    ESC_edit

REEDIT

	MOVE.L  A2,D0              ;Current cursor posn.
	SUB.L   A0,D0              ;less start of buffer = no of BS to do
	SUB.L   #1,D0              ;don't BS over prompt!
	BMI.S   15$
10$
	DO	UTIL.BS
	DBRA    D0,10$
15$
	MOVE.L  A1,D0              ;Save, so we can calc. how many spaces to add

	LEA     INBUFF(A6),A1      ;Will leave endofbuf pointer in A1
	MOVE.L  A1,A2              ;And current posn as start of line

	MOVE.W  #BUFLEN,D1         ;Better clear out INBUFF to spaces
20$
	MOVE.B  D5,0(A1,D1.W)      ;D5 = a space (#$20)
	DBRA    D1,20$

	LEA     VOLDBUF(A6),A3

      ENDC

      IFNE    ESC_edit!Wimp

REEDIT.30

	MOVE.B  (A3)+,(A1)+        ;Copy very old buffer into INBUFF
	BNE.S   REEDIT.30          ;Until zero term.
	MOVE.B  D5,-(A1)           ;Zap the zero
	SUB.L   A1,D0              ;Calculate no. of spaces needed
	BMI     REPRINT            ;Neg => recovered line longer, so no spaces
	BRA     REPRINT2           ;Display line, + spaces to cover old line
      ENDC

      IFNE     Wimp


***********************************************
*
*  SCROLL COMMAND - this deals with a command
*                   passed by a pull down menu thing
*
*  SCR_PNTR points at the text of the command
*
***********************************************


SCROLL_COMMAND

	ADDQ.L  #4,A7             ;loose CALLret addr to GETCHAR

	MOVE.L	#SCR_BUF,A3
	ADD.L	A6,A3

	CLR.B   (A1)+             ;Mark end of line
10$
	MOVE.B  (A0)+,(A3)+       ;Save it (A0=INBUFF initially)
	BNE.S   10$

	LEA     INBUFF(A6),A0     ;Now move cursor to start of line
20$
	CALL    MOVE_L
	CMP.L   A0,A2             ;At start of line yet?
	BNE.S   20$

	MOVE.L  SCR_PNTR(A6),A3   ;Get pointer to new command string
	MOVE.L  A1,D0             ;So we can calculate how diff in size
	MOVE.L  A0,A1             ;Put new command at start of buffer

	CALL    REEDIT.30         ;Just to annoy Roddy let's CALLto a local
	                          ;from another routine!! (next.. the XREF ..)
	BRA     EXIT_GETLINE


**********************************************
*
*  SCROLL_RECOVER  after having delt with
*                  the command above, come here
*                  to recover the stuff being
*                  done at the time
*
**********************************************


SCROLL_RECOVER

	CLR.L    SCR_PNTR(A6)         ;better clean up abit

	MOVE.L	#SCR_BUF,A3
	ADD.L	A6,A3		     ;where the old text is stored.
10$
	MOVE.B   (A3)+,(A1)+
	BNE.S    10$
	MOVE.B   D5,-(A1)             ;Stomp a space on the #00 term

	BRA      REPRINT

      ENDC

	ENDC			;of IFNE M68000

	IFEQ	M68000
*
* get a character, forced to a line of chars by native code interface so:
*
* ignores any initial spaces
* waits till a LF is met before exiting
*

GETCHAR

	MOVE.B	(A2),D1			;get char to use as default
	DO	CHAR.SUB		;print it (ignore justification)
	MOVEQ	#8,D1			;use a BS to
	DO	CHAR.SUB		;position cursor 
10$
	CALL	GETCHAR2
	CMP.B	#' ',D1			;ignore spaces at start of line
	BEQ.S 	10$
	MOVE.W	D1,-(A7)		;save the char we will use
20$
	CMP.W	#10,D1
	BEQ.S	90$			;found a LF so exit
	CALL	GETCHAR2		;not found one, so look again
	BRA.S	20$
90$
	MOVE.W	(A7)+,D1
	RET

* A1 on exit points at char after end of line

GETLINE

	PUSH_L 	D0/D1
	LEA	INBUFF(A6),A1
	MOVE.W	#BUFLEN-2,D0		;max no of chars less a bit
10$
	CALL	GETCHAR2
	MOVE.B	D1,(A1)+
	CMP.B	#10,D1
	DBEQ	D0,10$
20$
	CLR.B	-1(A1)
	PULL_L	D0/D1	
	RET

	ENDC


	END
