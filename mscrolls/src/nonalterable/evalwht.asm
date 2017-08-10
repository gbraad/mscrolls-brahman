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
*	@(#)evalwht.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.EVALWHT
* 
*--------------------------------

	include	"equates.asm"
	include	"nounequ2.asm"       ;FOR NOUNNUM
	include	"macros.asm"


      XREF     NEXITFLG,WCARR,GETDATA,LINE,P.TN,W.SEPAWY
      XREF     SETOUT,FIXSYS,P.SUBV,W.CAR,W.2MUCH,W.AND
      XREF     W.ANOY,ROOM,TEMPBUF

      XDEF     EVALWHT,QCARRIED,QCARIED2
 
 
EVALWHT
 
	CLR.W   NEXITFLG(A4)       ;SO THAT MAYBE WE CAN GET OUT AGAIN
	CLR.W   WCARR(A4)          ;START FROM SCRATCH!
05$
	LEA	TEMPBUF(A4),A5		;stash all nouns whose weight we
					;use here
	MOVE.W  #NMAXNOUN,D0
10$

      IFEQ    FastBits

	CALL    QCARRIED           ;TIME?

      ENDC

      IFNE    FastBits

	DC.W    $A0F9

      ENDC

	BNE.S   20$
	GETINFO
	TEST_L  6(A0)              ;ABSOLUTELY ON INV
	BNE.S   07$                ;NOPE    
	MOVE.B  1(A0),D1           ;GET PHYS CHAR
	LSR.B   #4,D1              ;GET HARDNESS
	CMP.B   #1,D1              ;LIQUID?
	BNE.S   07$
	MSG    LINE
	DO	P.TN
	MSG     SEPAWY
	DO	SETOUT
07$
	MOVE.W  12(A0),D1
	AND.W   #$3FFF,D1
	BEQ.S   15$
	CLR.L   D5                 ;SUPRESS TEXT OUTPUT (THE X&Y ARE CON..)
	DO	FIXSYS             ;RETURNS A3=FIXTURE BUFFER (EXBUFF REALLY)
	PUSH_L  D0/A0		;SAVE PRESENT NOUN#
60$
	MOVE.W  (A3)+,D0           ;GET NOUN# FROM FIXTURE BUFF
	BEQ.S   13$                ;0=END OF LIST
	GETINFO
	BTST    #4,3(A0)           ;MOVEABLE?
	BNE.S   60$                ;YES TRY NEXT ONE
	MOVE.W  #$C800,D2          ;NOT IF ROOM,OUT OR INTRINSIC   
	CALL	QCARIED2           ;ALLOWED IF NONMOVEABLE OBJ IS CARRIED
	BEQ.S   60$
	ST      NEXITFLG(A4)       ;SORRY CANNOT GET OUT THE ROOM FOLKS!
13$
	PULL_L  D0/A0	   	 ;RESTORE ORDER
 
15$
	MOVE.W	D0,(A5)+		;save noun# in case we want to drop it
	MOVE.B	2(A0),D1		;GET S&W
	AND.W	#$000F,D1		;WEIGHT ONLY
	ADD.W	D1,WCARR(A4)		;SUM IT
	CMP.W	#MAXWGHT,WCARR(A4)	;TOO MUCH?
	BHI.S	90$			;YUP!
20$
	SUBQ.W	#1,D0			;NEXT NOUN  
	BNE.S	10$
	RET
 
90$
	BTST	#2,6(A0)	;WORN??
	BNE.S	FindDrop	;NOT ON - GET ANOTHER ITEM 
	TEST_W	12(A0)		;TIED?
	BNE.S	FindDrop	;YES = FIND ANOTHER OBJ
	TEST_W	8(A0)		;really carried (ie not like pocket!)
	BNE.S	FindDrop

DropToGround

	MSG    LINE           
	DO	P.SUBV	            ;YOU ARE
	MSG     CAR                ;CARRYING
	MSG     2MUCH              ;TOO MUCH
	MSG     AND                ;AND
	DO	P.TN
	MSG     ANOY               ;ANNOYINGLY TO THE GROUND   
	MOVE.W  ROOM(A4),8(A0)     ;DO A DROP
	DO	SETOUT	            ;UNTIE ETC
	CLR.W   6(A0)              ;NOT RELATED ETC + IN
	BCLR    #0,11(A0)          ;kill 'worn by NPC' bit
	BRA     EVALWHT

*
* FindDrop
*
* Finds an object on inventory which may be dropped if to much is carried
*

FindDrop

	MOVE.W	#-1,(A5)	;mark end of buffer of 'carried' nouns
	LEA	TEMPBUF(A4),A5	;get start of buffer

10$
	MOVE.W	(A5)+,D0
	BMI.S	20$		;hmmm nothing to drop! (Can this happen?)

	GETINFO
	BTST	#2,6(A0)	;WORN??
	BNE.S	10$		;NOT ON - GET ANOTHER ITEM 
	TEST_W	12(A0)		;TIED?
	BNE.S	10$		;YES = FIND ANOTHER OBJ
	TEST_W	8(A0)		;really carried (ie not like pocket!)
	BEQ	DropToGround	;yes, drop it and recalc.
	BRA	10$

20$

*
* this should never happen!!
*
* if WCARR is too big but nothing 'carried' can be dropped 
* 'cos it's worn/tied etc then we come here (panic!!) so lets stop them
* leaving the room (safe bet!) and return (exit evalwht)
*
* only hassles I can see are down to free liquids being missed for a time
* since evalwht may not get a chance to consider all nouns 
*
	ST	NEXITFLG(A4)	;sorry cannot get out the room folks!
	RET

*--------------------------------


QCARRIED
 
	MOVE.W  #$CC00,D2         ;NOT IF ROOM,OUT OR INTRINSIC OR WORN 
 
QCARIED2

	PUSH_L  D0-D1/A0     ;SAVE NOUNNUM & SCRATCH
05$
	GETINFO
	BTST    #0,5(A0)           ;PSEUDO?
	BNE.S   20$                ;YES - YUK!
	MOVE.W  6(A0),D1           ;GET LOC. INFO
	BEQ.S   10$                ;NOT POINTED NOUN
	AND.W   D2,D1              ;LIKE ABOVE BUT CHANGES BETWEEN TWO
	BNE.S   15$                ;NOT ON INVENT
	MOVE.W  8(A0),D0           ;GET NEXT NOUN UP
	BNE.S   05$                ;NE = POINTED NOUN, SO LOOP
 
* 0= FALL THROUGH, COS ON INV 
	
10$ 
	TEST_W  8(A0)		;ON INV?
	BRA.S   90$
15$
	TEST_W  D0
	BRA.S   90$
 
20$
	SETNE				;SET NE = NOT ON INVE
90$
	PULL_L  D0-D1/A0		;RECOVER ORIGINAL NOUN & DATA 
	RET

	END

