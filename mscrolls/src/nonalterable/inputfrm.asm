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
*	5/16/87
*	@(#)inputfrm.asm	2.11
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.INPUTFORM
*
*--------------------------------


	include	"equates.asm"
	include "macros.asm"

	XREF    W.HASSLE

	XDEF    INFORMAT

      IFNE	THE_PAWN
KillSpaces	EQU	FALSE
      ENDC

      IFEQ	THE_PAWN
KillSpaces	EQU	TRUE
      ENDC

************************************************************
* 
* INFORMAT - 'tweeks' input line so as parser/search have an
*            easier time.
*
* Features:
*            initial '"' maps to 'say '
*            ',', '.', '&' map to ' and '
*            various (!?^~) characters map to spaces
*
* possibly (depends on scenario) number prossesing happens here (yuck!!)
* also o'clock gets mapped to o_clock
*
* lastly multiple spaces are shrunk (so Guild_of_Thieves is ok)
*
* simple eh?
*
* typically in this bit of code:
*
* A0 -> at current character
* A1 -> end of input string
* A6 -> start of buffer
* A5 -> end of buffer
* A3 work register
* A4 not to be changed!!!!!
*

INFORMAT

    IFNE	TimeCode
	SF	NumFlag(A4)		;set up time trapping bits
	CLR.W	NumIndex(A4)
	CLR.W	WhichNum(A4)		;as used by GetNumber
    ENDC
 
	LEA	BUFFLEN(A6),A5     ;A6=START OF BUFFER+MAX BUFFER SIZE
	MOVE.L  A6,A0
	CMP.B   #'"',(A0)          ;LEADING '"' = 'SAY'
	BEQ	DoSay
Loop

    IFNE	TimeCode
	CMP.B	#'0',(A0)
	BLT.S	10$
	CMP.B	#':',(A0)	;valid time chars are 0-: (: follows 9)
	BLS	DoNumeral
10$

	SF	NumFlag(A4)
	CMP.B	#'''',(A0)		;o'clock?
	BEQ	FodApostrophe
    ENDC

    IFEQ	THE_PAWN
	CMP.B   #'~',(A0)		;force blank line char
	BEQ.S   FodSpace                ;MAKE IT A SPACE
    ENDC

	CMP.B   #'&',(A0)          ;EQUIVILENT TO 'AND'
	BEQ.S   FodAnd
	CMP.B   #',',(A0)          ;ALSO EQIV
	BEQ.S   FodAnd
	CMP.B   #'.',(A0)          ;SAME AS AN 'AND'
	BEQ.S   FodAnd
	CMP.B   #'^',(A0)          ;MAKE INTO A SPACE
	BEQ.S   FodSpace
	CMP.B   #'"',(A0)          ;KILL QUOTES IN BODY OF LINE
	BEQ.S   FodSpace
	CMP.B   #'!',(A0)
	BEQ.S   FodSpace           ;MAKE IT A SPACE
	CMP.B   #'?',(A0)          ;AS PER SPACE
	BNE.S   Next

FodSpace

	MOVE.B  #' ',(A0)          ;PUT IN THE SPACE
Next
	ADDQ.L  #1,A0              ;NEXT
	CMP.L   A0,A1              ;FINISHED?
	BNE.S   Loop
	MOVE.L  A6,A0              ;POINT AT START AGAIN
30$
	CLR.L   D7                 ;OK, NO HASSLE
	CLR.B   (A1)
	CLR.B   1(A1)
	CLR.B   2(A1)
	CLR.B   3(A1)

    IFNE	TimeCode
	MOVE.W	NumIndex(A4),D6		;terminate numbers list
	LEA	NumBuf(A4),A3
	CLR.B	0(A3,D6.W)
    ENDC

    IFNE	KillSpaces
	CALL	RemoveMultSpaces	;replace '   ' with ' ' 
    ENDC

	RET                        ;BACK TO SCREEN ETC
 
FodAnd

	MOVE.L  A1,A3
	ADDQ.L  #4,A3
	CMP.L   A3,A5
	BMI	Hassled			;BUFFER OVERFLOW
70$
	MOVE.B  -4(A3),(A3)
	SUBQ.L  #1,A3
	CMP.L   A3,A0
	BNE.S   70$
	ADDQ.L  #4,A1
	CLR.B   (A1)
	MOVE.B  #' ',(A0)+
	MOVE.B  #'a',(A0)+
	MOVE.B  #'n',(A0)+ 
	MOVE.B  #'d',(A0)+ 
	MOVE.B  #' ',(A0)
	BRA     Next  
DoSay
	MOVE.L  A1,A3
	ADDQ.L  #4,A3
	CMP.L   A3,A5
	BMI	Hassled			;BUFFER OVERFLOW
FodSay
	MOVE.B  -4(A3),(A3)
	SUBQ.L  #1,A3
	CMP.L   A3,A0
	BNE.S   FodSay
	ADDQ.L  #4,A1
	CLR.B   (A1)
	MOVE.B  #' ',(A0)+
	MOVE.B  #'s',(A0)+
	MOVE.B  #'a',(A0)+ 
	MOVE.B  #'y',(A0)+ 
	MOVE.B  #' ',(A0)
	BRA     Next  


    IFNE	TimeCode

	XREF	NumFlag,NumIndex,NumBuf,NumBUFSIZE,WhichNum

* put spaces each side of numbers + save number (maybe)

DoNumeral

	MOVE.W	NumIndex(A4),D6		;misc setup
	CMP.W	#NumBUFSIZE-1,D6
	BEQ.S	Hassled			;oops out of space in buffer
	LEA	NumBuf(A4),A3

	BSET	#0,NumFlag(A4)		;was previous char a number?
	BNE.S	10$
	ST	0(A3,D6.W)		;separate numbers
	ADDQ.W	#1,D6
10$
	MOVE.B	(A0),D7		
	CMP.W	#NumBUFSIZE-1,D6
	BEQ.S	Hassled
	MOVE.B	D7,0(A3,D6.W)
	ADDQ.W	#1,D6
	MOVE.W	D6,NumIndex(A4)

	MOVE.L  A1,A3
	ADDQ.L  #2,A3
	CMP.L   A3,A5
	BMI.S	Hassled			;BUFFER OVERFLOW
20$
	MOVE.B  -1(A3),(A3)
	SUBQ.L  #1,A3
	CMP.L   A3,A0
	BNE.S   20$
	ADDQ.L  #1,A1
	CLR.B   (A1)
	MOVE.B  D7,(A0)+
	MOVE.B  #' ',(A0)

	BRA     Next  
 
    ENDC
 
Hassled

	MOVEQ   #-1,D7
	MSGGO	HASSLE

    IFNE	TimeCode		;o'clock etc

FodApostrophe

	CMP.B	#'o',-1(A0)
	BEQ.S	10$
	CMP.B	#'O',-1(A0)
	BNE.S	20$
10$
	CMP.B	#'c',1(A0)
	BEQ.S	20$
	CMP.B	#'C',1(A0)
	BNE.S	20$

	MOVE.B	#'_',(A0)		;make "o'clock" "o_clock"
20$
	BRA	Next

      ENDC
    
      IFNE	KillSpaces

* kill '  ' to ' ' so as search has an easier time of Guild_Of_Thieves
*
* On arrival here A0 -> start of buffer, A1 -> end of input line
*                 A6 -> start of buffer, A5 -> end of buffer
* 

RemoveMultSpaces

	MOVE.L	A0,A3
	MOVEQ	#0,D2		;not a space!
10$
	MOVE.B	(A0)+,D1
	CMP.B	#' ',D1
	BNE.S	20$
	CMP.B	D1,D2		;same as last char?
	BEQ.S	30$		;EQ => yes, don't keep it!
20$
	MOVE.B	D1,(A3)+
30$
	CMP.L	A0,A1
	BNE.S	10$

	MOVE.L	A3,A1		;new end of input line
	CLR.B	(A3)+
	CLR.B	(A3)+
	CLR.B	(A3)+

	MOVE.L	A6,A0		;replace start of line
	RET



      ENDC



	END


