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
*	@(#)protect.asm	2.6
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

****************************************
*
*   PROTECT.ASM
*   AJS -7/7/1986
*
****************************************


* NB !!!!!! the SCN calls in this code should really be MSG calls
*           to be fixed... ('cos various bits are NOT scenario dependent)

* This routine when called will.
* 1.  ask you for the word on page n, line n word n
* 2.  compare it with the known word
* 3.  if okay - continue
* 4.  if not okay allow 3 tries.
* 5.  crash!!!!

	include	"equates.asm"
	include	"macros.asm"
	include	"scen.i"

	XDEF    PROTECT2,PROTECT
	XREF    POSWRD,SCR_IGNO,PRTNUM,PRTTXT,INBUFF,SEARCH,INPUT,FNDWRD
	XREF    DO.SCN,PROT_FLG,MATCHBUF,CLOCK,PRNUM,RNDGEN,PRSTART


PROTECT:

	CMP.W    #100,CLOCK(A4)
	BHI.S    5$
	RET
5$

PROTECT2:

	TEST_B  PROT_FLG(A4)                    ;already done it?
	BEQ.S   10$
	RET
10$
	ST      PROT_FLG(A4)                    ;set flag saying done
	PUSH_L  D0-D7/A0-A6               ;save reggies:

* Insert random number bit here...

PRNMAX	EQU	$3FF			; Allows up to 1024 entries!

15$:
	CALL     RNDGEN                   ;returns random in d0
	AND.W	 #PRNMAX,D0		  ;remove the top bits
	CMP.W	 #PRNUM,D0		  ; is it still too big?
	BGE	 15$			  ; Yes, so retry.

	CALL_S   PRINTLINE                ;print the question..
	                                  ;and return with a pointer in
	                                  ;A0 TO sect no. word no.
	                                  ;the word.
	PULL_L  D0-D7/A0-A6         ;restore life.
	RET


PRINTLINE:

* this will ask the question...
* the format for the page no/line no/word no is:
* 6 bits page no.  6 bits line no.  4 bits word no.
* 6 bits section no.  10 bits word no (for search)

* D1 Contains the index no...

	MOVE.L  #PRSTART,A0             ;the list of the tables
	ADD.L   A4,A0
	LSL.W   #2,D0                   ;multiply by 4 for index
	ADD.W   D0,A0                   ;a0 now points to correct word.
	MOVE.B  (A0),D0                 ;section no.
	LSR.W   #2,D0                   ;shift it right 2 coz its 6 bits
	AND.W   #$3F,D0                 ;6 Bits
	MOVE.W  D0,-(A7)                ;save it temp.
	MOVE.W  (A0)+,D0                ;word no.
	AND.W   #$3FF,D0                ;10 Bits
	MOVE.W  D0,-(A7)

* Get the first one.

	MOVE.B  (A0),D6                 ;the page no.
	LSR.B   #2,D6                   ;shift it down 2
	AND.W   #$3F,D6                 ;we only want bottom 6 bits
	FIXED_SCN   Prot1		;... page no
	CALL    PRTNUM                  ;print out the number
	MOVE.W  (A0),D6                 ;get the line no.
	LSR.W   #4,D6                   ;move it down 4
	AND.W   #$3F,D6                 ;only want bottom 6 bits
	FIXED_SCN   Prot2		;...line
	CALL    PRTNUM                  ;print it 
	MOVE.W  (A0)+,D6                ;Word no.  
	AND.W   #$F,D6                  ;we only want 4 bits 

	FIXED_SCN   Prot3		;word no...
	CALL    PRTNUM
	FIXED_SCN   Prot4		;ta alot now enter de word!



GETCOMP:

* This will get the word that the punter has typed in.
* and compare it with the know word no. sect no.
* if okay it will return (up to 3 tries.)
* else crash.!!!!



	MOVE.W  #3,D0              ;three tries.
10$
	FIXED_SCN	Enter		;<"^Please enter word :  ">

  IFNE   Wimp
	ST       SCR_IGNO(A4)	;Ignore input from pulldown scrolls for a bit
  ENDC

      CALL     INPUT               ;Get a line from keyboard

  IFNE   Wimp
      SF       SCR_IGNO(A4)         ;Stop ignoring scrolls
  ENDC

      LEA.L    INBUFF(A4),A6        ;Get addr of filename
      MOVE.W   D0,-(A7)
      DO	FNDWRD               ;lose initial spaces
      MOVE.W   (A7)+,D0
      TEST_B   (A6)                 ;We do have something?
      BEQ.S    10$                  ;NE => yes
      MOVE.L   A6,POSWRD(A4)        ;Make sure rest of world knows


20$

* A6 points to the word....

	MOVE.W  D0,-(A7)
	DO	SEARCH              ;return d5 = word d6= section.
	MOVE.W  (A7)+,D0
	LEA     MATCHBUF(A4),A0
	MOVE.W  (A0)+,D6            ;section
	CMP.W   2(A7),D6            ;section okay?
	BNE.S   30$
	MOVE.W  (A0)+,D6            ;Word
	CMP.W   0(A7),D6            ;word okay?
	BNE.S   30$
	ADDQ.W  #4,A7
	FIXED_SCN	Thanks		;thank you for your time 
	RET


30$:
	SUBQ.W  #1,D0               ;have they run out of luck?
	BEQ.S   DEATH
	FIXED_SCN	Tryagain	;<"^Sorry please try again">
	BRA     10$

DEATH:
* for now..

      IFNE    YCHEAT

	PRINT   <'Your Amiga would have just crashed'>
	ADDQ.L  #4,A7
	RET
	
      ENDC


	FIXED_SCN	Removedisk	;Please remove disk & power off
10$
	BRA.S   10$


	END
