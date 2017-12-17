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
*	@(#)push.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.PUSH/PULL
*
*--------------------------------
 
	include	"equates.asm"
	include	"macros.asm"
	include	"verbequ.asm"


      XREF     ISDOOR,CLOSE,SP.PRESS,SP.PUSH,PCOK,V.MOVE,SP.PULL,C.STRENG
      XREF     W.CGGO,P.SUB,P.TN,P.CNTO,DBS,MVE.PNT,P.MSG,BRKD0
      XREF     OPEN,GETDATA,LINE,P.TNSTOP,FULLSTOP


      IfVerb	PUSH

      XDEF	V.PUSH

V.PUSH
 
	CALL    ISDOOR             ;IS IT A DOOR?
	BEQ     CLOSE
	CALL    SP.PRESS
	CALL    SP.PUSH
	MOVE.W  D0,D5
	MOVE.L  A0,A5
	CALL    PCOK               ;TOO SOFT?
	BEQ     V.MOVE             ;EQ = NO, SO DO A MOVE
	RET
 
	ENDC
 
*--------------------------------
 
	IfVerb	PULL

	XDEF	V.PULL
 
V.PULL
 
	CALL    ISDOOR
	BEQ     OPEN               ;EQ = IS A DOOR
	CALL    SP.PULL
	BTST    #3,6(A0)           ;Intrinsic?
	BNE.S   DBS_CLOSE
	BTST    #6,6(A0)           ;Room?
	BNE.S   DBS_CLOSE
	MOVE.B  3(A0),D1           ;GET TEXTURE
	AND.B   #$0F,D1

10$
	CMP.B   #5,D1              ;SLIPPERY?
	BNE.S   20$                ;NOPE
	DO	P.SUB
	MSG	CGGO             ;CANNOT GET A GRIP ON
	DOGO	P.TNSTOP
 
20$
	MOVE.W  D0,-(A7)
	CALL_S  PULL.PNT
	MOVE.W  (A7)+,D0
	MSG    LINE
	CALL    P.CNTO
	BNE.S   20$
	RET
 
DBS_CLOSE

	DOGO	DBS

PULL.PNT

	CMP.B   #$11,6(A0)         ;insep rel?
	BEQ.S   DBS_CLOSE
	BTST    #0,5(A0)           ;pseudo?
	BNE.S   DBS_CLOSE
	MOVE.B  1(A0),D2           ;Get hardness & strength  
	MOVE.B  D2,D1
	AND.B   #$0F,D1            ;Strength
	BEQ.S   DBS_CLOSE
	LSR.B   #4,D2              ;hardness
	BEQ.S   DBS_CLOSE          ;0=> void
	CMP.B   #$3,D1             ;too strong to break?
	BHS     MVE.PNT
	CMP.B   #$0F,D2            ;non-breakable? eg colours
	BEQ.S   DBS_CLOSE

	GETINFO
	DO	P.TN

	CMP.B   #12,D2             ;flexible?   
	BEQ.S   10$
	MSG	BRKS
	BRA.S	20$
10$
	MSG	SNPS
20$
	DO	FULLSTOP
	MSG    LINE
	BSET    #2,(A0)            ;break object
	BRA     BRKD0              ;deal with containers etc

	ENDC


      END

