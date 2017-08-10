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
*	@(#)look.asm	2.16
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.LOOK
*
*--------------------------------


	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"

      XREF     ADVERB,SEATED,V.FIND,W.NOT,W.FHERE,P.BTN,ISARE,W.CLOSED
      XREF     P.TN,W.CNTS,GET_CONT,P.LIST,P.DESC,W.DEADDN,FMSG
      XREF     CHAROUT,LINE,GETRELN,GETNPC,P.PRN,W.WEARNG,GETDATA,PRTTXT
      XREF     P.PV,W.GLOW,W.LIT,W.AND,W.SEEMS,SPACE
      XREF     W.FXTO,ONLYREL,ISANYREL,W.THERE,W.ON,P.OBJPRN,W.FXTIT,SCAN
      XREF     FULLSTOP,GETNOUNS,L.DATA,L.DATA2,P.SCN,P.Description
      XREF	BSFULLSTOP,P.TNSTOP,P.LISTSTOP


* VERB ROUTINE: LOOK,EXAMINE

	IfVerb	LOOK

	XDEF     LOOK
 
LOOK
      
      IFEQ	THE_PAWN

	XREF	SP.LOOK
	CALL	SP.LOOK
      
      ENDC
10$
	MOVE.W  ADVERB(A4),D1
	CMP.W   #IN,D1                ;IN
	BEQ.S   20$
	CMP.W	#INTO,D1
	BEQ.S	20$
	CMP.W   #WITHIN,D1              ;WITHIN (!!)
	BEQ.S   20$
	CMP.W   #UNDER,D1               ;UNDER
	BNE.S   40$
	
	TEST_W  SEATED(A4)
	BEQ     V.FIND               ;OK
	MSG	NOT
	MSGGO	FHERE
	
20$
	MOVE.B  10(A0),D1            ;CONTAINER?
	AND.B   #7,D1
	BEQ.S   40$
	MOVEQ   #$60,D2              ;MASK FOR CLOSED/LOCKED
	AND.B   (A0),D2              ;IS IT?
	BEQ.S   30$                  ;NO

      IFEQ	THE_PAWN

	BTST	#3,(A0)
	BNE.S	30$

      ENDC

	DO	P.BTN	               ;BUT THE ..
	DO	ISARE
	MSG	CLOSED
	DOGO	BSFULLSTOP
30$
	DO	P.TN
	MSG	CNTS
	DO	GET_CONT
	MOVE.L  A1,A0
	DO	P.LISTSTOP
	MSGGO	LINE
40$
	BTST    #6,4(A0)             ;NPC?
	BEQ.S   55$                  ;NO
	BTST	#1,(A0)			;Is it dead?
	BEQ.S	55$			;Nope alive and well.
	DO	GETNPC

      IFEQ	THE_PAWN

	BTST	#2,2(A3)		;animal?
	BNE.S	55$			;yes, give normal desc
      
      ENDC

	DO	P.TN	
	DO	ISARE
	MSGGO	DEADDN		;AS DEAD AS A DOORNAIL
55$
	CALL    P.Description		;First check which description.
    
      IFEQ	THE_PAWN

	BTST	#2,4(A0)		;short look?
	BEQ.S	60$			;nope
	RET				;NE => yes, forget rest of look
      
      ENDC
60$
	MSG    LINE
	MOVE.L  A0,A2
	MOVE.W  D0,D2
	DO	GETRELN	             ;CREATE BUFF=A1
	BTST    #6,4(A0)             ;CHARACTER?
	BEQ.S   STAT                 ;NOT SO
	DO	GETNPC	              ;FIND DATA

* GETNPC:
* FINDS DATA ON D0 INTO A3


	BTST    #1,2(A3)             ;SUPPRESS?
	BNE.S   STAT                 ;YES
	DO	P.PRN                ;THE NOUN#
	DO	ISARE
	MSG	WEARNG
	CLR.L   D3                   ;BIT MASK
	MOVE.L  A1,-(A7)             ;PUSH
L70
	MOVE.W  (A1)+,D0             ;GET NOUN
	BEQ.S   90$                  ;DONE
	GETINFO
	BTST    #0,11(A0)            ;WORN?
	BEQ.S   80$                  ;NO, AGAIN
	MOVE.B  D3,D4                ;SAVE MASK
	OR.B    10(A0),D3            ;WITH BITS
	AND.B   #$F8,D3              ;TOP 4 ONLY
	CMP.B   D4,D3                ;DIFFERENT
	BNE.S   L70                  ;YES, OK
80$ 
	BSET    #7,-2(A1)            ;DUMMY
	BRA.S   L70                  ;DO REST
90$
	MOVE.L  (A7)+,A0             ;PULL
	DO	P.LISTSTOP
	MSG	LINE
	MOVE.L  D2,D0                ;RESET NOUN#
	MOVE.L  A0,A1                ;A1=START
	MOVE.L  A2,A0                ;RESTORE
STAT
 
* NOW PRINT STATUS INFORMATION
  
	MOVE.B  #6,D2                ;HIGHEST BIT
	LEA     L.DATA(A4),A3        ;ADDR
	CLR.W   D4
	BTST    #7,(A0)              ;LIT?
	BEQ.S   11$                  ;NOPE

* this is really yuk!!!!!!

	PEA     11$(PC)
	CALL    P.PV
	MOVEQ   #1,D4
	BTST    #7,4(A0)             ;LIGHTABLE= LIT
	BEQ.S	10$
	MSGGO	LIT
10$
	PRINT	<'GLOWING '>
	RET

11$

	BTST    D2,(A0)              ;BIT?
	BEQ.S   12$                  ;NO
	TEST_B  (A3)+
	BEQ.S   13$
	SUBQ.L  #1,A3
	TEST_W  D4                   ;ANY?
	BEQ.S   15$
	MSG	AND
15$
	CALL    P.PV                 ;IT IS
	ADDQ.W  #1,D4
14$
	MOVE.B  (A3)+,D1             ;GET CHAR
	BEQ.S   13$                  ;DONE
	CALL    LOOK.COUT
	BRA.S   14$
12$
	MOVE.B  (A3)+,D1 
	BNE.S   12$                  ;UNTIL END
13$
	SUBQ.B  #1,D2                ;DEC COUNTER
	BNE.S   11$                  ;NEXT FLG
	TEST_W  D4
	BEQ.S   19$
	DO	BSFULLSTOP
	MSG    LINE
19$
	BTST    #6,4(A0)             ;NPC
	BEQ.S   20$
	BTST    #1,(A0)
	BNE.S   20$
	DO	GETNPC
	MOVE.B  2(A3),D2
	LSR.B   #4,D2
	BEQ.S   20$
	DO	P.PRN
	MSG	SEEMS
	MSG	SPACE
	LEA     L.DATA2(A4),A1
	SUBQ.B  #1,D2
	BEQ.S   21$
22$
	MOVE.B  (A1)+,D1
	BNE.S   22$
	SUBQ.B  #1,D2
	BNE.S   22$
21$
	MOVE.B  (A1)+,D1
	BEQ.S   23$
	CALL    LOOK.COUT
	BRA.S   21$
23$
	DO	FULLSTOP
	MSG	LINE
20$

* NOW DO FIXTURES

	MOVE.W  12(A0),D1            ;FIXTURE
	AND.W   #$3FFF,D1
	BEQ.S   LCONT                ;NOT FIXED
	CALL    P.PV
	MSG	FXTO
	MOVE.W  12(A0),D1
	EXG     D1,D0
	DO	P.TNSTOP
	EXG     D1,D0
	MSG	LINE
LCONT

* NOW DO CONTAINERS

	MOVE.W  D0,D6
	MOVE.B  10(A0),D1            ;GET DATA
	AND.B   #7,D1                ;LOW 3
	BEQ.S   40$                  ;NOT A CONTAINER
	BTST    #3,(A0)              ;DEHASSLE CONTAINER
	BNE.S   31$                  ;DO 
*
* if the container is not closed (like fountain - may not be open)
* list it's contents
*
	BTST    #5,(A0)              ;Closed??
	BNE.S   40$                  ;NE => yes
31$
	MOVE.L  A1,A3                ;SAVE
35$
	TEST_W  (A3)                 ;DT?
	BEQ.S   34$
	BCLR    #7,(A3)              ;OK
	ADDQ.L  #2,A3
	BRA.S   35$
34$
	CLR.B   D4
	MOVE.L  A1,-(A7)             ;SAVE START
32$
	MOVE.W  (A1)+,D0             ;GET NOUN
	BEQ.S   33$                  ;DONE
	GETINFO                      ;TO A0
	BTST    #5,6(A0)             ;CONTAINED?
	BEQ.S   37$                  ;NO, OK
	ADDQ.B  #1,D4
	BRA.S   32$
37$
	BSET    #7,-2(A1)             ;DUMMY
	BRA.S   32$
33$
	MOVE.L  (A7)+,A1             ;START
	TEST_B  D4
	BEQ.S   36$
	MOVE.L  A2,A0 
	DO	P.PRN
	MSG	CNTS
	MOVE.L  A1,A0
	DO	P.LISTSTOP		;LIST CONTAINED ITEMS
	MSG	LINE
36$
	MOVE.L  A0,A1                ;A1=START
	MOVE.L  A2,A0                ;RESTORE ADDR
	MOVE.W  D6,D0                ;BACK

*NOW SCAN FOR ITEMS FIXED TO IT
  
40$    
	BTST    #6,4(A0)
	BNE.S   43$                  ;NOT4NPC
	BTST    #6,6(A0)             ;ROOM?
	BNE.S   43$                  ;SKIP

	PUSH_L  D0/A0          	     ;SAVE THESE

	MOVE.W  D0,D2                ;TARGET
	MOVE.W  D6,-(A7)             ;SAVE NOUN
	DO	GETRELN	             ;RELATED TO D0
	MOVE.W  (A7)+,D6             ;BACK
	CALL    ONLYREL
	CALL    ISANYREL             ;IN THIS SENSE?
	BEQ.S   42$                  ;NO, 2 BAD

	MSG	THERE
	GETINFO                     ;ON RET D0
	DO	ISARE
	MOVE.L  A1,A0               ;START OF LIST
	DO	P.LIST              ;LIST OUT THEM
	MSG	ON
	PULL_L  D0/A0			;BAK
	DO	P.OBJPRN            ;IT
	DO	BSFULLSTOP
	BRA.S   43$
42$ 
	PULL_L  D0/A0
	
43$
	MOVE.W  D0,D2                ;TARGET
	MOVEQ   #1,D5                ;WRD
	MOVEQ   #12,D1               ;BYTE#
	DO	GETNOUNS             ;TAB=A1
	MOVE.L  A1,A0                ;ADDR TO A0
	TEST_W  (A0)                 ;NONE?
	BEQ     41$                  ;YES
	PUSH_L  D0/A0    
	MSG	THERE
	MOVE.W  (A0),D0
	GETINFO
	DO	ISARE
	PULL_L  D0/A0  
	DO	P.LIST
	MSG	FXTIT
	MSG	LINE
41$
	CMP.W   #UNDER,ADVERB(A4)        ;UNDER?
	BEQ     SCAN
99$
	RET
 
 
LOOK.COUT
 
	OR.B    #$20,D1              ;FUDGE TO L CASE
	DOGO	CHAROUT

	ENDC
 
*--------------------------------

	END
