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
*	@(#)utils.asm	2.11
*	8/26/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.UTILS.DEMO
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"


	XDEF     STWINDOW,INPUT,P.LOC

	XREF     BIOS,BIOS.INP,LINE,INBUFF,CURSX,CURSY,P.NOUN,WIDTH,SPACE
	XREF     SCORE_,PRTNUM,CLOCK,CHANNEL,TOPCURS1,PRTTXT,CHAROUT

	XREF	BIOSA6

     IFNE    Wimp

	XREF     MS_END,GRA_EXIT

     ENDC

     IFNE    ST520

	XREF    CONTERM

     ENDC
 
*--------------------------------
*
* STWINDOWS
*
* SET UP CHANNEL 0 AS A LARGE
* WINDOW.
*
* SET UP CHANNEL 1 AS SMALL
* STATUS DISPLAY AT TOP OF SCREEN
*
*--------------------------------
 
STWINDOW
 
      IFNE     QL128

	LEA     $20000,A0
	LEA     $28000,A1
10$
	CLR.L   (A0)+
	CMP.L   A0,A1
	BNE.S   10$

      ENDC
 
	RET
  
*--------------------------------
*
* INPUT TEXT INTO BUFFER
*
*--------------------------------
 
INPUT
	
	PUSH_L  D0-D7/A0/A2-A5
	CALL	BIOSA6
	JSR     BIOS.INP(A4)        ;(A6)
	CLR.B   (A1) 
	CLR.B   1(A1)
	MSG    LINE
	LEA     INBUFF(A4),A6

     IFNE    YCHEAT
	
	CMP.B   #'*',(A6)          ;GO MONITOR?
	BNE.S   10$

      IFNE    ST520

	PULL_L  D0-D7/A0/A2-A5       ;Restore regs,so SID gets 'em.
	CALL	BIOSA6

    IFNE	Wimp

	CALL    MS_END                     ;END OF MOUSE

    ENDC

	CLR.L   -(SP)
	MOVE.W  #$20,-(SP)                 ;supervisor mode
	TRAP    #1
	ADD.W   #6,SP
	BCLR    #3,CONTERM                 ;fix hassles with ED and del/bs
	MOVE.L  D0,-(SP)

    IFNE	Wimp

	CALL    GRA_EXIT

    ENDC

	MOVE.W  #$20,-(SP)
	TRAP    #1
	ADD.W   #6,SP
*      ILLEGAL
	DC.W    $4AFC
      ENDC
    

      IFNE    QL128

	DC.W     $4AFC

      ENDC

      IFEQ     M68000

	DC.W     $4AFC

      ENDC


      IFNE     AMIGA512!MAC512

	DC.W     $4AFC

      ENDC


10$
     ENDC
  
	PULL_L  D0-D7/A0/A2-A5
	RET
  
*--------------------------------
  
  
  
P.LOC
 
      IFNE    QL128
	CALL    LINE
	PUSH_L  D0-D7/A0-A6     ;SAVE ALL
	MOVE.L  CURSX(A4),-(A7)       ;SAVE CURRENT CURSOR POS
	ST      CHANNEL(A4)           ;SO CHAROUT KNOWS
	MOVE.L  #TOPCURS1,CURSX(A4)   ;ROOM NAME POS   
	CALL    P.NOUN                ;ROOM NAME
	MOVE.W  WIDTH(A4),D4
	SUB.W   #10,D4
10$
	CALL    SPACE
	CMP.W   CURSX(A4),D4
	BNE.S   10$
	
	MOVE.W  SCORE_(A4),D6
      IFNE	THE_PAWN
	MULU    #5,D6
      ENDC
	CALL    PRTNUM
	PRINT   <'/'>
  
	MOVE.W  CLOCK(A4),D6
	CALL    PRTNUM    
	CALL    LINE                  ;SO AS THE LAST CHAR WAS A LF
*                                    ;TO FOOL COUT TO SUPPRESS NEXT ONE
	CLR.B   CHANNEL(A4)
	MOVE.L  (A7)+,CURSX(A4)       ;PUT BACK ORIG XY POS
	PULL_L  D0-D7/A0-A6
	RET

	ENDC


      IFNE    ST520

	CALL    LINE
	PUSH_L  D0-D7/A0-A6     ;SAVE ALL
	ST      CHANNEL(A4)           ;SO CHAROUT KNOWS

*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	CLR.B	CURSX(A4)



	CALL    P.NOUN                ;ROOM NAME
	MOVE.W  WIDTH(A4),D4
	SUB.W   #10,D4
10$
	CALL    SPACE
	CMP.B   CURSX(A4),D4          ;Anita's CURSX is a byte
	BNE.S   10$
	
	
	MOVE.W  SCORE_(A4),D0
      IFNE	THE_PAWN
	MULU    #5,D0
      ENDC

	MOVE.W	CLOCK(A4),D1
	MOVEQ	#9,D2
	MOVE.W	D0,-(A7)
	CALL	PadWithSpaces
	MOVE.W	(A7)+,D6

	CALL    PRTNUM
	PRINT   <'/'>
  
	MOVE.W  CLOCK(A4),D6
	CALL    PRTNUM    


	MOVE.W  WIDTH(A4),D4
20$
	CMP.B   CURSX(A4),D4          ;Anita's CURSX is a byte
	BEQ.S	30$
	CALL    SPACE
	BRA.S	20$
30$

	CALL    LINE                  ;SO AS THE LAST CHAR WAS A LF
*                                    ;TO FOOL COUT TO SUPPRESS NEXT ONE
	SF      CHANNEL(A4)
	PULL_L  D0-D7/A0-A6
	RET

      ENDC  
 
 
      IFNE    AMIGA512!MAC512

	CALL    LINE
	PUSH_L  D0-D7/A0-A6     ;SAVE ALL
	ST      CHANNEL(A4)           ;SO CHAROUT KNOWS

* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	CLR.W	CURSX(A4)




	CALL    P.NOUN                ;ROOM NAME
	MOVE.W  WIDTH(A4),D4
	SUB.W   #10,D4
10$
	CALL    SPACE
	CMP.W   CURSX(A4),D4
	BNE.S   10$
	
	MOVE.W  SCORE_(A4),D0
      IFNE	THE_PAWN
	MULU    #5,D0
      ENDC

	MOVE.W	CLOCK(A4),D1
	MOVEQ	#9,D2
	MOVE.W	D0,-(A7)
	CALL	PadWithSpaces
	MOVE.W	(A7)+,D6

	CALL    PRTNUM
	PRINT   <'/'>
  
	MOVE.W  CLOCK(A4),D6
	CALL    PRTNUM    
	CALL	SPACE

	IFNE	0

	MOVE.W  WIDTH(A4),D4
	SUBQ.W  #1,D4
20$
	CALL    SPACE
	CMP.W   CURSX(A4),D4
	BNE.S   20$

	ENDC

	CALL    LINE                  ;SO AS THE LAST CHAR WAS A LF
*                                    ;TO FOOL COUT TO SUPPRESS NEXT ONE
	SF      CHANNEL(A4)
	CLR.W   CURSX(A4)
	PULL_L  D0-D7/A0-A6
	RET

	ENDC


	IFNE    (IBM!APPLE2!C128!C64!BBC_SWR!CPC6128!VAX!PCW8256!ATARI)

	CALL    LINE			;Must be CALL not MSG!
	PUSH_L  D0-D7/A0-A6     ;SAVE ALL
	ST      CHANNEL(A4)           ;SO CHAROUT KNOWS
	CALL    P.NOUN                ;ROOM NAME
	MOVEQ   #9,D1
	CALL    CHAROUT		     ; Output a top line 'tab'


	MOVE.W  SCORE_(A4),D0         ; Mul. by 5

      IFNE	THE_PAWN
	ASL.W   #2,D0
	ADD.W   SCORE_(A4),D0
      ENDC

	MOVE.W	D0,-(A7)

* Now pad spaced before score/moves such that total = 9 

	MOVE.W	CLOCK(A4),D1		;value 2
	MOVEQ	#9,D2			;target size for field (inc '/')

	CALL	PadWithSpaces

	MOVE.W	(A7)+,D6


	CALL    PRTNUM
	PRINT   <'/'>
  
	MOVE.W  CLOCK(A4),D6
	CALL    PRTNUM    

	CALL    LINE                  ;SO AS THE LAST CHAR WAS A LF
*                                    ;TO FOOL COUT TO SUPPRESS NEXT ONE
	SF      CHANNEL(A4)
	PULL_L  D0-D7/A0-A6

	RET
	ENDC

* Pad with spaces - print (D2-((log10(D0)+log10(D1)+2)) spaces

PadWithSpaces

	LEA	DecTab(PC),A0
10$
	CMP.W	(A0)+,D0
	BPL.S	15$
	CALL	SPACE
	BRA.S	10$
15$
	LEA	DecTab(PC),A0
20$
	CMP.W	(A0)+,D1
	BPL.S	25$
	CALL	SPACE
	BRA.S	20$
25$

	RET

	
DecTab
	DC.W	1000,100,10,0	

	RET


 
*--------------------------------

	END

