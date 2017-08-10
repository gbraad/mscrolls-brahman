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
*	@(#)suss.asm	2.10
*	9/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
* 
*  SRC.ADV.SUSS
*
*--------------------------------
 
* AID TO DEBUGGING

	include	"equates.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"
 
      XREF     P.TN,LINE,CHAROUT,GETNPC,IGNORAJ
      XREF     HAVEWON,W.IDFY,USER.DB,P.NOUN,VERB,VERB2,ADDNOUN,CONJ
      XREF     GETDATA,GETVECT,PRTTXT


      XDEF     V.CHEAT

     IFNE     YCHEAT

      XDEF     SUSS,V.FOD,PHEXBYTE,PHEXWORD,PRTHEX,V.LOOP

      XREF     DBUG.ON,CHEAT,CHEAT.LI,CHEAT2,CHEATNUM


SUSS
	TEST_W  DBUG.ON(A4)
	BEQ     90$
	PUSH_L  A0/D0
	MSG    LINE
	DO	P.TN
	MOVEQ   #'#',D1
	DO	CHAROUT
	MOVE.W  D0,D6
	CALL    PHEXWORD
	MSG    LINE
	MOVE.L  A0,D6
	CALL    PRTHEX
	MSG    LINE
	MOVE.B  #6,D2
10$
	MOVE.B  (A0)+,D6
	CALL    PHEXBYTE
	MSG    SPACE
	SUBQ.B  #1,D2
	BNE.S   10$
	MSG    LINE
	MOVE.L  (A0)+,D6
	CALL    PRTHEX
	MSG    SPACE
	MOVE.W  (A0)+,D6
	CALL    PHEXWORD
	MSG    SPACE
	MOVE.W  (A0)+,D6
	CALL    PHEXWORD
	MSG    LINE
	PULL_L  A0/D0
	PUSH_L 	D0/A0
	TEST_W	IGNORAJ(A4)	 ;quietly/and adverb?
	BPL.S	20$
	CALL	LONG_SUSS
20$
	PULL_L 	D0/A0
	BTST    #6,4(A0)
	BEQ.S   90$
	DO	GETNPC	              ;A3
	MOVE.L  A3,D6
	CALL    PRTHEX
	PRINT   <': '>
	DO	GETNPC
	MOVE.L  (A3),D6
	CALL    PRTHEX
90$
	RET
 
      ENDC
   
*--------------------------------
  
  
V.CHEAT
 
      IFNE     YCHEAT
  
	TEST_W  DBUG.ON(A4)
	BEQ     90$

   IFNE	THE_PAWN
	CMP.W   #NBAND,D0          ;CHEAT BAND?
   ENDC 

   IFEQ	THE_PAWN
	CMP.W	#NBAG1,D0
   ENDC

	BNE.S   10$
	PRINT   <'OK THEN'>
	NOT.W   CHEAT2(A4)
	RET
10$ 

   IFNE	THE_PAWN
	CMP.W   #NSHIRT,D0
   ENDC 

   IFEQ	THE_PAWN
	CMP.W	#NSWEATSHIRT,D0
   ENDC

	BNE.S   20$  
	PRINT   <'...'>
	NOT.W   CHEAT.LI(A4)
	RET
20$
	CMP.W    #NJEANS,D0
	BNE.S    30$
	PRINT    <'!!!'>
	NOT.W    CHEATNUM(A4)
	RET
30$
	PRINT   <'HMM'>
	NOT.W   CHEAT(A4)
	RET
90$
 
      ENDC

      IFNE	THE_PAWN
  
	TEST_W  HAVEWON(A4)        ;WON YET?
	BNE.S	9001$
      
      ENDC

	MSGGO	IDFY             ;HONEST! 

      IFNE	THE_PAWN
9001$
	NOT.W   USER.DB(A4)        ;OK LET EM TOGGLE   
	RET
      
      ENDC

*--------------------------------
 
	IFNE     YCHEAT
	  
V.FOD
 
	TEST_W  DBUG.ON(A4)
	BEQ     90$
	BSET    #1,(A0)      ;DEAD TOO
	MSG	LINE
	DO	P.NOUN
	PRINT   <' NOW DECEASED'>
90$
	RET
 
*--------------------------------
	
V.LOOP
	
	MOVE.W  VERB2(A4),VERB(A4) ;REAL VERB
	MOVE.W  D5,D3              ;TARGET
	PUSH_L  D0/A0  
	MOVE.W  ADDNOUN+2(A4),D0
	MOVE.W  CONJ+2(A4),CONJ(A4)
	GETINFO        
	MOVE.L  D0,D5
	MOVE.L  A0,A5
	PULL_L  D0/A0
10$
	CMP.W   D3,D0              ;THERE YET?
	BHI.S   20$                ;YES, DONW
	PUSH_L  D0-D1/D3/D5/A0/A5
	MOVE.W  VERB(A4),D2
	DO	GETVECT            ;TO A1
	JSR     (A1)               ;DO IT
	PULL_L  D0-D1/D3/D5/A0/A5
	MSG    LINE
	ADDQ.W  #1,D0              ;NEXY
	GETINFO        
	MOVE.B  (A0),D1
	BRA.S   10$                ;OK?
20$
	PRINT   <'DONE^'>
	RET     
	
	ENDC
   
NOVERB
 
	PRINT   <'NO CHANCE!^'>
	RET
 
 
*--------------------------------
 
  
SPACE
	MOVEQ   #$20,D1
	DOGO	CHAROUT
 
 
	IFNE     YCHEAT
  
M.HEXTBL
	DC.B     '0123456789ABCDEF'

PRTHEX
	MOVE.L  #28,D5 
  
PRTHEX2
 
	MOVE.L  D6,D4
	CALL_S  90$
	SUBQ.L  #4,D5
	BNE     PRTHEX2
	MOVE.L  D6,D4
	BRA.S   95$
  
90$
	LSR.L   D5,D4  
95$
	AND.W   #$F,D4
	LEA     M.HEXTBL(PC),A3
	MOVE.B  0(A3,D4.W),D1 
	DOGO	CHAROUT
 
PHEXBYTE
 
	MOVEQ   #4,D5
	BRA.S   PRTHEX2
 
PHEXWORD
 
	MOVEQ   #12,D5
	BRA.S   PRTHEX2
 
      ENDC

      IFNE	YCHEAT

	XREF	R.NOUN2,PSUDAT

TXT	MACRO
	DC.B	'\1 ',0
	ENDM

LONG_SUSS

* noun states (status)

      IFNE	M68000
	RET
      ENDC

      IFEQ	M68000

BYTE0
	MSG	LINE
	BTST	#7,(A0)		;lit?
	BEQ	10$
	PRINT	<'Lit '>
10$
	BTST	#6,(A0)		;locked?
	BEQ	20$
	PRINT	<'Locked '>
20$
	BTST	#5,(A0)	
	BEQ	30$
	PRINT	<'Closed '>
30$
	BTST	#4,(A0)
	BEQ	40$
	PRINT	<'Open '>
40$
	BTST	#3,(A0)
	BEQ	50$
	PRINT	<'Get without opening '>
50$
	BTST	#2,(A0)
	BEQ	60$
	PRINT	<'Broken '>
60$
	BTST	#1,(A0)
	BEQ	70$
	PRINT	<'Dead NPC '>
70$
	BTST	#0,(A0)
	BEQ	80$
	PRINT	<'Plural '>
80$

* Physical characteristics

BYTE1_T

	MOVE.B	1(A0),D6
	LSR.B	#4,D6
	AND.W	#$F,D6
	BEQ	BYTE1_B
	PRINT	<'^Hardness = '>
	CALL	PrtHexByte
	PRINT	<' = '>
	LEA	BYTE1_T_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE1_B
	DO	CHAROUT
	BRA.S	30$

BYTE1_T_Text

	TXT	Void
	TXT	Liquid
	TXT	Quantity
	TXT	<Can be cut with a sharp object>
	TXT	<Can be cut with a very sharp object>
	TXT	Normal
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	Brittle
	TXT	Tool
	TXT	<Can be tied>
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	<Unbreakable (and cannot break things!)>

	ALIGN

BYTE1_B

	MOVE.B	1(A0),D6
	AND.W	#$F,D6
	BEQ	BYTE2_T
	PRINT	<'^Strength = '>
	CALL	PrtHexByte

BYTE2_T

	MOVE.B	2(A0),D6
	LSR.B	#4,D6
	AND.W	#$F,D6
	BEQ	BYTE2_B
	PRINT	<'^Size = '>
	CALL	PrtHexByte
	BTST	#6,6(A0)		;room?
	BEQ	BYTE2_B
	PRINT	<' = '>
	LEA	BYTE2_T_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE2_B
	DO	CHAROUT
	BRA.S	30$

BYTE2_T_Text

	TXT	Normal
	TXT	<Squeeze, you must drop everything>
	TXT	<Small, only large enough for one person>
	TXT	<Low - no vehicles>
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	<Whack, you hit your head!>
	TXT	<The [vehicle] stumbles>
	TXT	<Ridiculous to ride there>
	TXT	UNDEFINED
	TXT	UNDEFINED
	TXT	UNDEFINED

	ALIGN

BYTE2_B

	MOVE.B	2(A0),D6
	AND.W	#$F,D6
	PRINT	<'^Weight = '>
	CALL	PrtHexByte

BYTE3_T

	MOVE.B	3(A0),D6
	LSR.B	#6,D6
	AND.W	#$3,D6
	BEQ	BYTE3_T_bits
	PRINT	<'^Sharpness = '>
	CALL	PrtHexByte
	PRINT	<' = '>
	LEA	BYTE3_T_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE3_T_bits
	DO	CHAROUT
	BRA.S	30$

BYTE3_T_Text

	TXT	N/A
	TXT	Blunt
	TXT	Sharp
	TXT	<Very sharp>

	ALIGN

BYTE3_T_bits

	MSG	LINE
	BTST	#5,3(A0)		;wet?
	BEQ	10$
	PRINT	<'Wet '>
10$	
	BTST	#4,3(A0)
	BEQ	20$
	PRINT	<'Moveable '>
	BRA	30$
20$
	PRINT	<'Steadfastly refuses to be moved '>
30$

* texture

BYTE3_B

	MOVE.B	3(A0),D6
	AND.W	#$F,D6
	BEQ	BYTE4_T
	PRINT	<'^Texture = '>
	CALL	PrtHexByte
	PRINT	<' = '>
	LEA	BYTE3_B_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE4_T
	DO	CHAROUT
	BRA.S	30$

BYTE3_B_Text

	TXT	Void
	TXT	Smooth	
	TXT	Rough
	TXT	Hot
	TXT	Cold
	TXT	Slippery
	TXT	Furry
	TXT	Clammy
	TXT	Soft
	TXT	Hard
	TXT	Warm
	TXT	NPC
	TXT	Thorny
	TXT	Sticky
	TXT	UNDEFINED
	TXT	UNDEFINED

	ALIGN

BYTE4_T

	MSG	LINE
	BTST	#7,4(A0)	;lightable?
	BEQ	10$
	PRINT	<'Lightable '>
10$
	BTST	#6,4(A0)	
	BEQ	20$
	PRINT	<'Is an NPC '>
20$
	BTST	#5,4(A0)	
	BEQ	30$
	PRINT	<'NPC refuses to use object '>
30$
	BTST	#4,4(A0)	
	BEQ	40$
	PRINT	<'Explored '>
40$
	BTST	#3,4(A0)	
	BEQ	50$
	PRINT	<'Money '>
50$
	MOVE.B	4(A0),D6
	AND.W	#7,D6
	BEQ	60$
	PRINT	<'Value = '>
	CALL	PrtHexByte
60$

BYTE5

	MSG	LINE
	BTST	#7,5(A0)		;Slow ROOTNUM?
	BEQ	10$
	PRINT	<'Slow ROOTNUM '>
10$
	BTST	#6,5(A0)		;NPC will give away ?
	BEQ	20$
	PRINT	<'NPC will give away  '>
20$
	BTST	#5,5(A0)
	BEQ	30$
	PRINT	<'Force plural '>
30$
	BTST	#4,5(A0)
	BEQ	40$
	PRINT	<'Force adjective '>
40$
	BTST	#3,5(A0)
	BEQ	50$
	PRINT	<'Can be sat on '>
50$
	BTST	#2,5(A0)
	BEQ	60$
	PRINT	<'Lit room '>
60$
	BTST	#1,5(A0)
	BEQ	70$
	PRINT	<'Initial state done '>
70$
	BTST	#0,5(A0)
	BEQ	80$
	PRINT	<'Pseudo '>
80$

BYTE6
	MSG	LINE
	PRINT	<'LOCATION INFO: '>
	BTST	#0,5(A0)		;pseudo?
	BEQ	BYTE6_main
*
* this may take forever!
*
	PRINT	<'^Pseudo item in: '>

	LEA	PSUDAT(A4),A2		;get start of pseudos list
	ADD.W	8(A0),A2		;get the first one for this noun
10$
	MOVE.B	(A2)+,D2		;get room no.
	BEQ.S	20$
	AND.W	#$FF,D2
	DO	R.NOUN2			;noun# to D0
	DO	P.NOUN
	PRINT	'('
	MOVE.W	D2,D6
	CALL	PrtHexByte
	PRINT	<') '>
	BRA	10$
20$
	MSG	LINE
	BRA	BYTE10

BYTE6_main

	BTST	#7,6(A0)		;out?
	BEQ.S	10$
	PRINT	<'Out '>
10$
	BTST	#6,6(A0)		;room?
	BEQ	20$
	PRINT	<'is a room'>
	BRA	BYTE10
20$
	MOVE.B	6(A0),D1
	AND.B	#$7F,D1
	BNE	NotSimple
	MOVE.W	8(A0),D2
	BNE	25$
	PRINT	<'inventory'>
	BRA	BYTE10
25$
	PRINT	<'Simply in room: '>
	DO	R.NOUN2
	DO	P.NOUN
	BRA	BYTE10

NotSimple


	BTST	#4,6(A0)		;inseparable?
	BEQ	10$
	PRINT	<'inseparable '>
10$
	BTST	#5,6(A0)
	BEQ	20$
	PRINT	<'Contained in: '>
20$
	BTST	#0,6(A0)
	BEQ	30$
	PRINT	<'Related to: '>
30$
	BTST	#1,6(A0)
	BEQ	40$
	PRINT	<'Hidden by: '>
40$
	BTST	#3,6(A0)		;intrinsic
	BEQ	50$
	PRINT	<'Intrinsic '>
50$
	BTST	#2,6(A0)		;worn
	BEQ	60$
	PRINT	<'Worn by player '>
60$
	MOVE.B	6(A0),D1
	AND.B	#$0C,D1			;worn/intrinsic mask
	BEQ	70$
	TEST_B	7(A0)			;secondry locator
	BEQ	65$
	PRINT	<'!* invalid secondary locator *! '>
65$
	TEST_W	8(A0)			;location must be zero
	BEQ	BYTE10
	PRINT	<'!* invalid location *! '>
	BRA	BYTE10
70$
	MOVE.L	D0,D5
	MOVE.L	A0,A5

	MOVE.W	8(A0),D0
	GETINFO
	DO	P.TN
	TEST_B	7(A5)		;secondary loc. of orig noun
	BEQ	80$		;0=> ok
	BTST	#0,5(A0)	;else its loc must be a PSEUDO (probably!)
	BNE.S	75$
	PRINT	<'!* marginal secondary locator *!'>
75$
	PUSH_L 	D5/A5
	MOVE.B	7(A5),D2
	AND.W	#$FF,D2
	DO	R.NOUN2
	PRINT	<'in room: '>
	DO	P.NOUN
	PULL_L 	D5/A5
80$
	MOVE.L	D5,D0
	MOVE.L	A5,A0

BYTE10
	
	MSG	LINE
	MOVE.B	10(A0),D1
	AND.B	#$F8,D1			;mask clothing
	BEQ	container_vol
	PRINT	<'Clothing - covers: '>
	BTST	#7,D1
	BEQ	10$
	PRINT	<'feet & legs '>
10$
	BTST	#6,D1
	BEQ	20$
	PRINT	<'groin '>
20$
	BTST	#5,D1
	BEQ	30$
	PRINT	<'arms '>
30$
	BTST	#4,D1
	BEQ	40$
	PRINT	<'head '>
40$
	BTST	#3,D1
	BEQ	50$
	PRINT	<'chest '>
50$

container_vol

	MSG	LINE
	MOVE.B	10(A0),D6
	AND.B	#7,D6
	BEQ	90$		;not a container
	SUBQ.B	#1,D6
	PRINT	<'Container volume '>
	CALL	PrtHexByte
90$

BYTE11

	MSG	LINE
	MOVE.B	11(A0),D6
	LSR.B	#5,D6
	BEQ	BYTE11_M
	PRINT	<'^Edibility = '>
	MOVE.W	D6,-(A7)
	CALL	PrtHexByte
	MOVE.W	(A7)+,D6
	PRINT	<' = '>
	LEA	BYTE11_T_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE11_M
	DO	CHAROUT
	BRA.S	30$

BYTE11_T_Text

	TXT	<Not edible>
	TXT	Fatal
	TXT	Poisoned
	TXT	<Become ill>
	TXT	Edible
	TXT	Tasty
	TXT	<Very good>
	TXT	<Magical (healing properties)>

	ALIGN



BYTE11_M

	MOVE.B	11(A0),D6
	LSR.B	#3,D6
	AND.W	#3,D6
	BEQ	BYTE11_B
	PRINT	<'^Combustability = '>
	MOVE.W	D6,-(A7)
	CALL	PrtHexByte
	MOVE.W	(A7)+,D6
	PRINT	<' = '>
	LEA	BYTE11_M_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE11_B
	DO	CHAROUT
	BRA.S	30$

BYTE11_M_Text

	TXT	<Will not burn>
	TXT	<Burns away>
	TXT	Melts
	TXT	Explodes

	ALIGN

BYTE11_B

	MOVE.B	11(A0),D6
	LSR.B	#1,D6
	AND.W	#3,D6
	BEQ	BYTE11_bit0
	PRINT	<'^Wearability = '>
	MOVE.W	D6,-(A7)
	CALL	PrtHexByte
	MOVE.W	(A7)+,D6
	PRINT	<' = '>
	LEA	BYTE11_B_Text(PC),A1
10$
	SUBQ.B	#1,D6
	BMI.S	30$
20$
	TEST_B	(A1)+
	BNE.S	20$
	BRA.S	10$
30$
	MOVE.B	(A1)+,D1
	BEQ	BYTE11_bit0
	DO	CHAROUT
	BRA.S	30$

BYTE11_B_Text

	TXT	<Cannot wear>
	TXT	Clothing
	TXT	Protective
	TXT	<Extra protective>

	ALIGN

BYTE11_bit0

	BTST	#0,11(A0)
	BEQ	10$
	PRINT	<'^Worn by npc'>
10$

WORD12

	MSG	LINE
	BTST	#7,12(A0)		;permanently fixed?
	BEQ	10$
	PRINT	<'Permanently fixed to '>
10$
	MOVE.W	12(A0),D0
	AND.W	#$7FFF,D0
	BEQ	90$
	DO	P.TN
90$
	MSG	LINE
	RET

PrtHexByte

	PUSH_L 	D0-D7/A0-A6
	CALL	PHEXBYTE
	PULL_L 	D0-D7/A0-A6
	RET

	ENDC
	ENDC





      END

