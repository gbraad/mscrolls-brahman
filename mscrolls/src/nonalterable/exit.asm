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
*	@(#)exit.asm	2.9
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.EXIT
*
*--------------------------------
   
	include	"equates.asm"
	include	"verbequ.asm"
	include "macros.asm"


      XREF     GO.LEAVE,MOVE.TAB,ROOM,VERB,CALCNET,W.WWAY,P.EXITS
      XREF     W.BUT,P.SUBV,W.NOT,W.IN,P.TN,GO,DBS,R.NOUN2,GO.N,PRTTXT
      XREF	BSFULLSTOP,GETDATA

	IfVerb	EXIT

	XDEF	V.EXIT
	XREF	SP.EXIT

V.EXIT

	CALL    SP.EXIT
	BTST    #6,6(A0)           ;IS IT A ROOM?
	BNE.S   05$
	CALL    GO.LEAVE           ;CATCH 'EXIT SHED' ETC
05$
 
	LEA     MOVE.TAB(A4),A1
10$
	MOVE.W  (A1)+,D1
	BEQ.S   NoDirectExit
	ADDQ.L  #2,A1
	CMP.W   D1,D0
	BNE.S   10$
	MOVE.W  -(A1),D1
	CMP.B   ROOM+1(A4),D1
	BNE     ButNotInTN		;wrong room!!!!!!!
	LSR.W   #8,D1
	CMP.B   #DIR_D,D1          ;DOWN?
	BEQ.S   20$
	CMP.B   #DIR_U,D1          ;UP
	BEQ.S   25$
	ADDQ.B  #4,D1
	CMP.B   #7,D1
	BLS.S   15$
	SUBQ.B  #8,D1
15$
	CLR.L   D5
	MOVE.B  D1,D5
	MOVE.W  #VNGO,VERB(A4)
	CALL    GO   
20$
	SUBQ.B  #1,D1
	BRA.S   15$
25$
	ADDQ.B  #1,D1
	BRA.S   15$

* find 'best' exit from this room (using network)

NoDirectExit

	BTST    #6,6(A0)           ;ROOM?
	BEQ     DBS                ;OK

	DO	CALCNET			;-> A0 pointer to net info

      IFEQ	THE_PAWN

	MOVE.B	DIR_OUT(A0),D2
	BEQ.S	10$			;nothing in 'out' dir
	CMP.W	#MAXROOM,D2		;an nway_xyzzy?
	BCC.S	10$			;CC=>yes

	DO	R.NOUN2			;get noun#
	GETINFO				;and noun data
	MOVE.W	#VNGO,VERB(A4)

	BRA	GO.N			;and fake a 'go xyzzy'

      ENDC


10$

	CLR.B   D0
	CLR.W   D4
40$
	MOVE.B  (A0)+,D1
	BMI.S   45$
	BNE.S   55$                ;VALID
45$
	ADDQ.W  #1,D0
	CMP.B   #9,D0
	BLS.S   40$
	CMP.B   #1,D4              ;# VALID
	BEQ.S   50$
	DO	CALCNET
	MOVE.B  #$FF,D1 
	MOVEQ   #9,D3
41$
	MOVE.B  0(A0,D3),D5   
	BEQ.S   42$
	CMP.W   D5,D1
	BMI.S   42$
	MOVE.B  D5,D1
	MOVE.L  D3,D2
42$
	DBRA    D3,41$
	TEST_B  D1
	BPL.S   50$

	MSG	WWAY
	BRA     P.EXITS
50$
	CLR.L   D5
	MOVE.B  D2,D5
	MOVE.W  #VNGO,VERB(A4)
	CALL    GO  
55$
	MOVE.W  D0,D2
	ADDQ    #1,D4
	BRA.S   45$

ButNotInTN

	MSG	BUT
	DO	P.SUBV
	MSG	NOT
	MSG	IN
	DO	P.TN
	DOGO	BSFULLSTOP	

	ENDC
 
*--------------------------------
 
	IfVerb	EXITS

	XDEF	V.EXITS
 
V.EXITS
 
	BRA     P.EXITS

	ENDC

      END

