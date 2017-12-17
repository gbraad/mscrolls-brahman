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
*	@(#)charout.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.CHAROUT
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"

      XDEF     CHAROUTL,CHAROUT,CHAROUT2,CHAR.SUB

      XREF     PLURAL,CHANNEL

    IFNE   M68000

      XREF     PUNCT,CAPITAL,LAST.CH2,LJPOS,STOWRD,POS,WIDTH
      XREF     LAST.CH,BIOSA6,BIOS.COU,SPACE

     IFNE   SpoolLen
      XREF     SPOOLOUT
     ENDC

    ENDC


     IFEQ      THE_PAWN
      XREF     VERT_BAR
     ENDC

 
*--------------------------------
*
*  OK I GIVE IN!!!!!
*
* SRC.ADV.UTILS IS TOO BIG A 
* FUCK UP SO HERE GO ALL THE
* ANNOYING BITS OF TRASH COUT
* HAS TO APPLY
*
*--------------------------------
 
 
*--------------------------------
*
*  CHAROUTL - COUT FOR PRTTXT
*             FUDGES TO LOWER
*             CASE BEFORE APPLYING
*             USUAL CAPITALISING
*             RULES
*
*--------------------------------
 
CHAROUTL
 
	CMP.B   #'A',D1            ;HI BIT SET WILL UPSET THIS
	BLT.S   10$                ;HOWEVER 'PRTTXT' DOESN'T 
	CMP.B   #'Z',D1            ;ALLOW HI BIT SET
	BGT.S   10$                ;GOES TO 10$ IF NOT UPPER CASE CHARS
	OR.B    #$20,D1            ;MAKE LOWER CASE
10$
 
*      FALLS THROUGH TO CHAROUT
 
*--------------------------------
*
*  CHAROUT - NORMAL COUT, DOES ALL
*            THE TWIDDLY BITS, SUSSES
*            OUT CAPITALISATION,
*            LEFT JUSTIFICATION,
*            TOP LINE JUNK ETC
*
*  DECOM DOES *NOT* COME TO THIS 
*  BIT, BUT CALLS CHAROUT2
*
* ON ENTRY : D1.B CHAR IS BE PRINTED
*
* D1 IS 'VAGUELY' STOMPED , '^' AND '@'
* MAY BE CONVERTED TO $0A AND 's' ALSO
* HI BIT OF BYTE WILL BE STOMPED,
* CASE MAY BE STOMPED
*
*--------------------------------

      IFNE    M68000
 
CHAROUT
 
	CMP.B   #-1,D1             ;do we just want to FOD CAPITAL?
	BNE.S   10$                ;nope
	ST      CAPITAL(A4)        ;ok, and that's all we want to do
	RET

10$
	CLR.B   PUNCT(A4)   
  
* ZAP DECOM'S SPACE AFER PUNCTUATION FLAG, SO AS TO AVOID
* SPURIOUS SPACES BEFORE THE START OF NEW MSG WHEN 
* PREVIOUS MSG ENDED IN PUNCTUATION BUT WHERE NON DECOM.
* TEXT IS INBETWEEN THE TWO MSG'S
  
CHAROUT2
 
	AND.B   #$7F,D1            ;LOSE ANNOYING HI BIT
	
	TEST_B  CHANNEL(A4)        ;GOING TO TOP LINE WINDOW?
	BEQ.S   10$                ;0= NOPE
	CMP.B   #'a',D1            ;LOWER CASE?
	BLT.S   05$                ;BRANCH IF NOT    
     
	AND.B   #$DF,D1            ;ZAP TO UPPER CASE
05$
	DOGO	CHAR.SUB           ;FORGET ALL THE JUSTIFICATION ETC
 
10$
	CMP.B   #'^',D1            ;FALSE LINE FEED?
	BNE.S   20$                ;NE= NOPE
	MOVEQ   #10,D1             ;OK - MAKE REAL LF CHAR
20$
	CMP.B   #'@',D1            ;FUDGE FOR VERB CONJUGATIONS?
	BNE.S   30$                ;NE= NOPE  
	TEST_B  PLURAL(A4)         ;WAS THE NOUN PLURAL?
	BNE	90$                ;YES = DON'T PRINT ANYTHING
	MOVEQ   #'S'+32,D1         ;NO = PRINT A LOWER CASE 'S'
30$
	TEST_B  CAPITAL(A4)        ;WAS THE PREVIOUS CHAR THE END OF
	BEQ.S   50$                ;OF A SENTENCE?
 
	CMP.B   #'A',D1            ;YES, MAKE THIS CHAR UPPER CASE
	BLT.S   50$                ;IF IT'S LOWER CASE
	CMP.B   #'Z',D1            ;IF ALREADY UPPER CASE ZAP FLAG
	BLS.S   40$                ;BRANCHES IF UPPER CASE
 
	CMP.B   #'A'+32,D1         ;SUSS IF REAL LOWER CASE
	BLT.S   50$                ;NOPE - DON'T ZAP FLAG
	CMP.B   #'Z'+32,D1         ;OTHER END OF RANGE
	BHI.S   50$                ;BRANCH IF NOT LOWER CASE
 
	AND.B   #$DF,D1            ;ZAP TO UPPER CASE
40$ 
	CLR.B   CAPITAL(A4)        ;ZAP FLAG ONCE WE'VE PRINTED AN
*                                  ;ALPHABETIC CHAR
 
50$
	CMP.B   #'.',D1            ;SHOULD NEXT CHAR BE UPPER CASE?
	BEQ.S   60$                ;YES IF ',' '?' '!' OR <LF
	CMP.B   #'?',D1
	BEQ.S   60$
	CMP.B   #'!',D1
	BEQ.S   60$
	CMP.B   #$0A,D1            ;LF = 10
	BNE.S   70$                ;OK, NOT ON 'LIST'
60$
	ST      CAPITAL(A4)
70$
	CMP.B	#'.',LAST.CH2(A4)
	BNE.S	75$
	CMP.B	#'.',D1
	BEQ.S	75$
	CMP.B	#'"',D1
	BEQ.S	75$
	CMP.B	#' ',D1
	BEQ.S	75$
	CMP.B	#')',D1
	BEQ.S	75$
	
	CALL	SPACE

75$
	CMP.B   #$20,D1            ;LOSE DOUBLE SPACES
	BNE.S   80$                ;NOT A SPACE - DON'T WORRY
	CMP.B   LAST.CH2(A4),D1    ;WAS PREVIOUS CHAR A SPACE TOO?
	BEQ.S   90$                ;YES - SO MISS OUT THIS ONE
80$

      IFEQ    THE_PAWN
	CMP.B	#'_',D1
	BNE.S	81$
	MOVEQ	#' ',D1
81$
      ENDC


	MOVE.B  D1,LAST.CH2(A4)    ;SAVE CHAR FOR NEXT TIME
	BRA.S   DO.JUSTI           ;DO JUSTIFICATION
90$
	RET
 
*--------------------------------
*
* DO.JUSTIFY - DO LEFT JUSTIFICATION
*
* ON ENTRY - D1.B = CHAR TO PRINT
*
* USES 26 BYTE BUFFER 'STOWRD'
* CURRENT POS IN BUFFER IS IN 'LJPOS'   
* BUFFER FLUSHED BY SPACE OR <LF
*
*--------------------------------
 
DO.JUSTI
 
	PUSH_L  D0-D2/A3     ;SAVE SCRATCH
 
	MOVE.W  LJPOS(A4),D0       ;GET CURRENT POSITION IN BUFFER
	LEA     STOWRD(A4),A3      ;GET START OF BUFFER
 
	MOVE.B  D1,0(A3,D0)        ;PUT CHAR IN BUFFER
 
	CMP.B   #' ',D1            ;SHOULD WE FLUSH?
	BEQ.S   10$
	CMP.B   #$0A,D1            ;CAN ALSO FLUSH ON LF 
	BEQ.S   10$
	CMP.W   #BIGWRD,D0         ;OR BUFFER OVERFLOW
	BEQ.S   10$
 
	ADDQ.W  #1,LJPOS(A4)       ;UPDATE POINTER TO END OF BUFFER
	BRA.S   90$                ;EXIT
	
10$
	MOVE.W  POS(A4),D2         ;GET CURRENT POSTION ON SCREEN
	ADD.W   D0,D2              ;CALC POSITION OF END OF BUFFER
	CMP.W   WIDTH(A4),D2       ;WILL IT FIT?
	BLT.S   20$                ;YES, GO PRINT IT  
 
* TAKE A NEW LINE COS BUFFER
* BIGGER THAN SPACE ON SCREEN
 
	MOVEQ   #10,D1             ;DO A LINE FEED, DOESN'T MATTER THAT
	CALL_S  CHAR.SUB           ;WE LOSE THE CHAR COS ITS IN BUFFER
*                                 ;SPACE, LF OR DUE TO OVERLOW
 
20$
	MOVE.B  (A3)+,D1           ;GET CHAR FROM BUFFER
	ADDQ.W  #1,POS(A4)         ;UPDATE SCREEN POSITION
	CALL_S  CHAR.SUB           ;PRINT CHARACTER
	DBRA    D0,20$
 
	CLR.W   LJPOS(A4)          ;POINT BACK TOP START OF BUFFER    
  
90$
	PULL_L  D0-D2/A3     ;GET BACK SCRATCH
	RET
 
*--------------------------------
*
* CHAR.SUB - PRINT CHAR IN D1.B
*
* ONLY 'CLEVERNESS' IS TO LOSE
* DOUBLE <LF'S AND TO SET POS 
* TO 0 WHEN A LINE FEED DOES HAPPEN
*
*--------------------------------
 
CHAR.SUB

      IFEQ	THE_PAWN
	CMP.B	#'_',D1
	BNE.S	01$
	MOVEQ	#' ',D1
01$
      ENDC

	CMP.B   #$0A,D1            ;LINE FEED?
	BNE.S   10$
	CMP.B   #$0A,LAST.CH(A4)   ;WAS THE PREVIOUS CHAR A LINE FEED
	BEQ.S   90$                ;YES - LOSE THIS ONE
	CLR.W   POS(A4)            ;ZAP POSITION ON SCREEN
	BRA.S   20$                ;DO SET LAST.CHR!
10$
	CMP.B   #$20,D1            ;DONT SET LAST.CHR IF CTRL-CHAR
	BLS.S   30$                ;OR SPACE
20$
	MOVE.B  D1,LAST.CH(A4)     ;SO WE CAN TEST NEXT TIME
30$
	PUSH_L  A4/A6        ;SAVE A4 - COUT SAVES LOTS TOO

      IFNE     SpoolLen
	LEA     SPOOLOUT,A6        ;deal with printer spool
	JSR     0(A4,A6.L)
      ENDC

	BSR	BIOSA6
	CALL    BIOS.COU           ;DO COUT

	PULL_L  A4/A6        ;GET IT BACK
90$
	RET

      ENDC

      IFEQ   M68000

CHAROUT
CHAROUT2
CHAR.SUB

	PUSH_W 	D2-D3
	MOVE.B	PLURAL(A4),D2
	MOVE.B	CHANNEL(A4),D3
	DC.W	$A0F3
	PULL_W 	D2-D3
	RET

      ENDC

      END

