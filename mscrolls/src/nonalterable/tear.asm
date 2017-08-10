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
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.TEAR/RIP/CUT
*
*--------------------------------

	include "equates.asm"
	include "macros.asm"
	include	"verbequ.asm"


      XREF     CONJ,YCANT,P.SUB,W.TEAR,P.TN,W.SHRED,SP.TEAR,SETOUT
      XREF     CHK4PT,NPC.OK,FIGHT,P.BTN,ISAREN,W.SHP,W.ENOUGH
      XREF     ISARE,W.TOO,W.TOUGH,W.NOCUT,BRKQ,CARRIED,P.MHAV
      XREF	BSFULLSTOP

	IfVerb	TEAR

	XDEF	V.TEAR

V.TEAR 
 
	TEST_W  CONJ(A4)
	BPL.S   V.CUT
	MOVE.B  1(A0),D1       ;GET PHYS CHAR
	LSR.B   #4,D1          ;WE WANT THE HIGN NIBBLE
	CMP.B   #3,D1          ;ONLY IF IT IS SOFT!
	BNE     YCANT

	ENDC

TRC.SUB

      IFEQ	THE_PAWN

	XREF	SP.TearBefore

	CALL	SP.TearBefore
    
      ENDC

	DO	P.SUB
	MSG	TEAR
	DO	P.TN
	MSG	SHRED
	DO	BSFULLSTOP
	PUSH_L	D0/A0
	CALL    BRKQ			;break it so as to rlease contents
	PULL_L	D0/A0
	DO	SETOUT			;OUT
	BRA     SP.TEAR
 

	IfVerb	CUT

	XDEF	V.CUT
 
V.CUT
	MOVEQ   #0,D1          ;'WITH'
	DO	CHK4PT
	EXG     D0,D5	      ;get indirect object
	DO	CARRIED	      ;must have object to cut with it.
	BNE     P.MHAV	      ;if not, print mess. & return.
	EXG     D0,D5	      ;exchange back for use.

     IFD	wantSP.CUT

	XREF	SP.CUT
	CALL	SP.CUT

     ENDC

	MOVE.B  3(A5),D1       ;GET SHARPNESS OF IMPLEMENT  
	LSR.B   #6,D1          ;TOP 2 BITS
	BEQ.S   60$            ;CAN'T CUT ANYTHING IF ZERO SHARPNESS
	BTST    #6,4(A0)       ;NPC?
	BEQ.S   10$            ;NO
	CALL    NPC.OK         ;OK?
	BEQ     FIGHT          ;ATTACK
10$
	MOVE.B  1(A0),D2       ;GET PHYS CHAR
	BEQ.S   90$
	LSR.B   #4,D2          ;TOP NIBBLE ONLY
	CMP.B   #3,D2          ;ANYTHING SHARP WILL CUT 'SOFT'
	BEQ.S   TRC.SUB
	CMP.B   #4,D2          ;RUBBER CAN BE CUT
	BNE.S   80$            ;'TOO TOUGH'
	CMP.B   #1,D1          ;SHARP ENOUGH?
	BGE.S   TRC.SUB        ;YES
 
60$ 
	EXG     D0,D5          ;GET IMPLEMENT
	EXG     A0,A5          ;NOUN DATA ETC
	DO	P.BTN	         ;'BUT THE XYZ '
	DO	ISAREN	        ;' IS NOT'
	MSG	SHP            ;' SHARP"
	MSGGO	ENOUGH
 
 
80$
	DO	P.TN	          ;'THE XYZ'
	DO	ISARE
	MSG	TOO
	MSG	TOUGH  
	BRA	BSFULLSTOP	; backspace then fullstop for pretty end

 
90$
	DO	P.SUB	         ;'YOU
	MSGGO	NOCUT        ; CAN'T CUT THAT!'

	ENDC

      END

