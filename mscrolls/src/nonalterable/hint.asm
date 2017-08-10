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
*	@(#)hint.asm	2.7
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

********************************************
*
* HINT.ASM - frigged for Guild of Thieves fuckup
*
********************************************


	include "equates.asm"
	include	"macros.asm"

	XREF     CHAROUT,LINE,PRTTXT,SCORE_,OLDTEXT

	XDEF     UNDOHINT

UNDOHINT

	CALL    UNCODE_A_HINT     ;Returns A6 -> msg D7=0 => ok
	TEST_L  D7
	BEQ.S   10$
	MSGGO	TRYAGAIN	;<'^Sorry, try that one again.'>
10$
	SUB.B   #'A',(A6)         ;A=> zero points ok
	MOVE.B  (A6)+,D0
	AND.W   #$FF,D0
	LEA     SCORE_LIST(PC),A0
	MOVE.B  0(A0,D0),D0
	CMP.W   SCORE_(A4),D0
	BHI.S   BAD_SCORE
	MSG     LINE
20$
	MOVE.B  (A6)+,D1
	BEQ.S   90$
	DO	CHAROUT
	BRA.S   20$
90$
	RET

BAD_SCORE

	MSGGO	MOREPOINTS	;'^You will need more points to find that out!'


SCORE_LIST

	DC.B     0           ;A = 0 
	DC.B     1           ;B = 5
	DC.B     10
	DC.B     20
	DC.B     30

	ALIGN


;* DEcrypt - this programme, passed a pointer to a line of ASCII by A6
;* will decrypt it back into itself -
;* and checksum it
;* and make sure you're allowed it with your score
;* I HOPE - ajs




TryUncode

	MOVE.L  A6,A5
	MOVE.L  A6,A0
DE5:
	MOVE.B  (A0)+,D0
	BEQ.S   OLD20
	CMP.B   #' ',D0
	BEQ.S   DE5
	CMP.B	#'<',D0
	BEQ.S	DE5
	CMP.B	#'>',D0
	BEQ.S	DE5

	MOVE.B  D0,D1
	CALL    RETURNCHAR
	AND.B   #$1C,D2
	LSL.B   #3,D2
	MOVE.B  D2,D0
DE10:
	MOVE.B  (A0)+,D1
	BEQ.S   OLD20
	CMP.B   #' ',D1
	BEQ.S   DE10
	CMP.B	#'<',D1
	BEQ.S	DE10
	CMP.B	#'>',D1
	BEQ.S	DE10

	CALL    RETURNCHAR
	SUBQ.W	#1,D2
	AND.B   #$1F,D2
	OR.B    D0,D2
	MOVE.B  D2,(A5)+
	BRA.S   DE5
OLD20:
	RET

UNCODE_A_HINT

	MOVE.L	A6,A0
	LEA	OLDTEXT(A4),A1
04$
	MOVE.B	(A0)+,D1
	MOVE.B	D1,(A1)+
	BEQ.S	08$
	AND.B	#$5F,D1
	CMP.B	#'L',D1
	BNE.S	04$
	MOVE.B	#'1',-1(A0)
	MOVE.B	#'1',-1(A1)
	BRA.S	04$
08$

	CALL_S	TryUncode

	PUSH_L	D0-D7/A0-A6
	MOVE.B	1(A6),D1		;second byte (first non-score)
	MOVE.B	-1(A5),D7		;checksum

	LEA	Frig(PC),A0		;frig table
	MOVE.W	#136,D6			;no of hints-1
10$
	CMP.B	(A0),D1			;get id word
	BNE.S	15$
	CMP.B	1(A0),D7
	BEQ.S	20$			;EQ => we got it!
15$
	ADDQ.W	#3,A0
	DBRA	D6,10$
*	PULL_L	D0-D7/A0-A6		;fall through => didn't reconise it
*	BRA.S	RETURNFAIL		;so fail
	MOVEQ	#7,D7			; If it fails then try the 'odd' hint
	BRA.S	26$
20$
	MOVEQ	#0,D7
	CMP.W	#136,D6
	BNE.S	25$
	MOVE.W	#$EE2,D7
25$
	MOVE.B	2(A0),D7		;get list of 1/L (1=1, L=0)
26$
	
	MOVEQ	#0,D6
	MOVE.L	A6,A1
	LEA	OLDTEXT(A4),A0
30$
	MOVE.B	(A0)+,D1
	MOVE.B	D1,(A1)+
	BEQ.S	90$
	CMP.B	#'1',D1
	BNE.S	30$
	BTST	D6,D7
	BNE.S	40$			;NE => 1, so leave
	MOVE.B	#'L',-1(A1)
40$
	ADDQ.W	#1,D6
	BRA.S	30$
90$
	PULL_L	D0-D7/A0-A6

	CALL	TryUncode		;do now fixed

	MOVEQ   #10,D6
	MOVE.L  A6,A1                   ;A0 POINTS AT END
	CMP.L   A1,A5
	BEQ.S   RETURNFAIL

DE30:
	MOVE.B  (A1),D0
	EOR.B   D6,D0
	MOVE.B  D0,(A1)+
	EOR.B   D0,D6
	CMP.L   A1,A5
	BNE.S   DE30
DE40:
	
	TEST_B  D0
	BNE.S   RETURNFAIL
DE50:
	MOVEQ   #0,D7
	RET

RETURNFAIL:

	MOVEQ   #-1,D7
	RET

RETURNCHAR:

	CMP.B  #$60,D1           ;kill case with care (numerics)
	BMI.S  05$
	AND.B  #$5F,D1
05$
	LEA    INDEXLOOP(PC),A3
	CLR.W  D2
D10:
	CMP.B   (A3)+,D1
	BEQ.S   DE202
	ADDQ    #1,D2
	CMP.B   #32,D2
	BNE.S   D10
	BRA     RETURNFAIL
DE202:
	RET

INDEXLOOP:
	DC.B    'COPYRIGHTMA5NE7XSKW0JLZ1986BDFQUV234!'

	ALIGN

FrigMacro	MACRO

		DC.B	\1/256
		DC.B	\1&255
		DC.B	\2
		
		ENDM

Frig
	FrigMacro	$1c27,$00e2		;really EE2

	FrigMacro	$1945,$0001
	FrigMacro	$192a,$0000
	FrigMacro	$0172,$0000
	FrigMacro	$095f,$0004
	FrigMacro	$0c64,$002c
	FrigMacro	$3862,$0000
	FrigMacro	$207e,$001b
	FrigMacro	$1a26,$0000
	FrigMacro	$0e24,$0000
	FrigMacro	$182f,$0000
	FrigMacro	$0947,$0000
	FrigMacro	$2a16,$000f
	FrigMacro	$0106,$0030
	FrigMacro	$094a,$0000
	FrigMacro	$092f,$0003
	FrigMacro	$1c1b,$0018
	FrigMacro	$2c19,$0018
	FrigMacro	$0b78,$0003
	FrigMacro	$0b56,$003f
	FrigMacro	$205f,$0003
	FrigMacro	$2023,$0000
	FrigMacro	$0e7f,$0000
	FrigMacro	$0118,$0001
	FrigMacro	$1e79,$0001
	FrigMacro	$0336,$003c
	FrigMacro	$0e3e,$0000
	FrigMacro	$1f3f,$0011
	FrigMacro	$1f5d,$0017
	FrigMacro	$2a7e,$0000
	FrigMacro	$1c0b,$0010
	FrigMacro	$0b5f,$00bc
	FrigMacro	$1c7e,$0004
	FrigMacro	$180b,$0000
	FrigMacro	$0c1d,$0001
	FrigMacro	$0917,$0002
	FrigMacro	$1c4d,$0000
	FrigMacro	$0e21,$0001
	FrigMacro	$1f20,$0001
	FrigMacro	$1808,$0000
	FrigMacro	$0b64,$0000
	FrigMacro	$0b06,$000c
	FrigMacro	$1c59,$0000
	FrigMacro	$0544,$0000
	FrigMacro	$183d,$0000
	FrigMacro	$0a4a,$0000
	FrigMacro	$0c6e,$0000
	FrigMacro	$0a68,$0003
	FrigMacro	$1c39,$0001
	FrigMacro	$1d65,$0000
	FrigMacro	$095d,$0009
	FrigMacro	$0c6a,$0000
	FrigMacro	$1f16,$000f
	FrigMacro	$2923,$0000
	FrigMacro	$122b,$0000
	FrigMacro	$2909,$0000
	FrigMacro	$0f13,$0003
	FrigMacro	$1e4b,$0006
	FrigMacro	$2545,$0001
	FrigMacro	$1b4d,$0001
	FrigMacro	$0c5e,$0000
	FrigMacro	$0a76,$0007
	FrigMacro	$0a38,$0003
	FrigMacro	$3b36,$0007
	FrigMacro	$1b18,$0003
	FrigMacro	$001e,$0003
	FrigMacro	$0031,$0001
	FrigMacro	$095c,$0003
	FrigMacro	$6a63,$0007
	FrigMacro	$1c41,$0012
	FrigMacro	$3b7f,$0000
	FrigMacro	$0a18,$000b
	FrigMacro	$0301,$0000
	FrigMacro	$1a22,$0023
	FrigMacro	$1b02,$0000
	FrigMacro	$0f6f,$0000
	FrigMacro	$093e,$0006
	FrigMacro	$212f,$0001
	FrigMacro	$263d,$000a
	FrigMacro	$2015,$0000
	FrigMacro	$094e,$0000
	FrigMacro	$1f16,$0007 	; was originally 1f16
	FrigMacro	$0a7b,$0001
	FrigMacro	$0312,$0000
	FrigMacro	$1b30,$0006
	FrigMacro	$010a,$0001
	FrigMacro	$3b7c,$0000
	FrigMacro	$0f4e,$0004
	FrigMacro	$1b05,$0000
	FrigMacro	$1956,$000d
	FrigMacro	$0a6f,$0000
	FrigMacro	$1f5f,$0000
	FrigMacro	$1f05,$0000
	FrigMacro	$0f16,$0000
	FrigMacro	$0416,$0006
	FrigMacro	$0000,$0001
	FrigMacro	$0c1e,$0000
	FrigMacro	$070f,$0033
	FrigMacro	$1a3d,$0010
	FrigMacro	$2759,$000f
	FrigMacro	$0a33,$0001
	FrigMacro	$2c18,$0004
	FrigMacro	$045f,$0001
	FrigMacro	$0041,$0005
	FrigMacro	$276a,$0000
	FrigMacro	$076d,$0001
	FrigMacro	$3a69,$0000
	FrigMacro	$1c40,$0000
	FrigMacro	$0a1f,$0003
	FrigMacro	$0640,$0000
	FrigMacro	$1715,$0000
	FrigMacro	$3a1f,$0003
	FrigMacro	$033a,$0004
	FrigMacro	$3c00,$0000
	FrigMacro	$0b71,$0001
	FrigMacro	$0837,$001f
	FrigMacro	$087f,$0000
	FrigMacro	$084d,$0001
	FrigMacro	$2a63,$0000
	FrigMacro	$0467,$0001
	FrigMacro	$1d2e,$0018
	FrigMacro	$1d7f,$0000
	FrigMacro	$2472,$0000
	FrigMacro	$046b,$002b
	FrigMacro	$1a3b,$0000
	FrigMacro	$1a5f,$0007
	FrigMacro	$3a43,$0001
	FrigMacro	$1747,$0001
	FrigMacro	$254b,$000c
	FrigMacro	$1a76,$00f6
	FrigMacro	$3d29,$0001
	FrigMacro	$0b1a,$0003
	FrigMacro	$0a2d,$0000
	FrigMacro	$1a09,$0006
	FrigMacro	$2c58,$0073
	FrigMacro	$1f0a,$0000
	FrigMacro	$3b0b,$0006

	END
