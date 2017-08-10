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
*	@(#)open.asm	2.10
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.OPEN/CLOSE
*
*--------------------------------

	include	"equates.asm"
	include "macros.asm"
	include	"verbequ.asm"

      XDEF     EXGNOUN

      XREF     P.BTNI,SP.OPEN,P.BTN,ISARE,W.LOCKED,YCANT,P.TNIN,DOORS,SEATED
      XREF     PSUDAT,CONNECT2,SP.CLOSE,ADVERB,GET_,CHK4PT,SP.UNLOC,TBL.DOOR
      XREF     W.IDNTT,P.QN,W.QSUIT,W.CNTUNL,VERB,W.OPN,W.UNABLE,P.TN,P.TNSTOP
      XREF	BSFULLSTOP,FULLSTOP,GETDATA


	IfVerb	OPEN

	XDEF	OPEN
  
OPEN
 
	MOVE.W  #M.OPN,D1
	BTST    #4,(A0)           	;TEST IF OPEN
	BNE     P.BTNI            	;BUT THE NOUN IS ALREADY OPEN.
 
	CALL    SP.OPEN
 
	TEST_W  D5                	;SECOND NOUN?
	BEQ.S   10$               	;0= NOT
	CALL    UNLOCK            	;TRY TO UNLOCK WI IT
	TEST_L  D6                	;UNLOCK SUCCESSFUL?
	BNE.S   90$               	;NE= NOT SUCCESSFUL,MSG PRINTED
10$
	BTST    #6,(A0)           	;LOCKED?
	BEQ.S   20$               	;EQ = NO
	DO	P.BTN	            	;CANNOT OPEN IF LOCKED!
	DO	ISARE
	MSG	LOCKED
	DOGO	BSFULLSTOP
20$
	BTST    #5,(A0)			;closed?
	BEQ     YCANT
	CMP.W	SEATED(A4),D0		;are we sitting on it?
	BEQ	YCANT			;yes - bad news
	MOVE.W  #M.OPN,D1         	;'THE NOUN IS NOW OPEN'
	DO	P.TNIN            	;DO IT
	BRA	OpenDoor
90$
	RET


	ENDC

	IfVerb	CLOSE

	XDEF	CLOSE
  
CLOSE
 
	BeSure	M.CLSE

	MOVE.W  #M.CLSE,D1
	BTST    #5,(A0)
	BNE     P.BTNI            	;BUT THE NOUNIS ALEADY CLOSED
 
	CALL    SP.CLOSE
  
	BTST    #4,(A0)           	;HAS TO BE OPEN TO BE CLOSED!
	BEQ     YCANT             	;YCANT - (UNLESS OPEN)
	CMP.W	SEATED(A4),D0		;are we sitting on it?
	BEQ	YCANT			;yes - bad news
  
	MOVE.W  #M.CLSE,D1        	;'THE NOUN IS NOW CLOSED
	DO	P.TNIN
	BRA	CloseDoor


	ENDC
 
*--------------------------------
	
	IfVerb	PICK

	XDEF	PICK
PICK  
 
	CMP.W   #$8008,ADVERB(A4) 	;PICK UP
	BNE.S   UNLOCK  
	MOVE.W  #-1,ADVERB(A4)
	BRA     GET_

*--------------------------------
	
	ENDC

	IfVerb	UNLOCK

	XDEF	UNLOCK
 
UNLOCK
 
	MOVEQ   #-1,D6            	;SUCCESS FLG FOR OPEN
	MOVE.B  (A0),D1
	AND.B   #$70,D1           	;ONLY ALLOWED IF LOCKED/OPEN/CLOSED
	BEQ.S   35$               	;DOGO	YCANT
	CLR.W	D1
	DO	CHK4PT	 
	BTST    #6,(A0)            	;LOCKED?
	BNE.S   20$

	BeSure	M.UNLOCK

	MOVE.W  #M.UNLOCK,D1
	CALL    P.BTNI
	DOGO	FULLSTOP
20$
	CALL    SP.UNLOC
	LEA     TBL.DOOR(A4),A1
30$  
	CMP.W   (A1)+,D0
	BEQ.S   40$
	TEST_W  (A1)+              	;FINISHED?
	BNE.S   30$
35$                  
	DOGO	YCANT
 
40$
	CMP.W   (A1),D5            	;CORRECT KEY?  ONLY ONE PER DOOR!
	BEQ.S   50$
	MOVE.B  2(A5),D1           	;S&W OF KEY
	AND.B   #$F0,D1            	;SIZE ONL
	CMP.B   #$10,D1            	;KEYS = 1
	BEQ.S   45$
	MSG	IDNTT          	;'I DO NOT THINK THAT
	CALL    EXGNOUN
	DO	P.QN
	DO	ISARE	
	MSG	QSUIT          	;'IS QUITE SUITABLE
	DOGO	BSFULLSTOP
45$
	CALL    EXGNOUN
	DO	P.TN
	MSG	CNTUNL         	;'CANNOT UNLOCK"
	CALL    EXGNOUN
	DOGO	P.TNSTOP
   
50$
	BCLR    #6,(A0)            	;UNLOCKED     
	BSET    #5,(A0)            	;&CLOSED
	CLR.L   D6                	;SUCCESSFUL
	CMP.W   #VNUNLOCK,VERB(A4)
	BNE.S   60$
	MOVE.W  #M.UNLOCK,D1
	DO	P.TNIN
	DOGO	FULLSTOP
60$
	RET

	ENDC
  
*--------------------------------

	IfVerb	LOCK

	XDEF	V.LOCK
 
V.LOCK

	MOVEQ   #0,D1              	;WITH
	DO	CHK4PT
	BTST    #6,(A0)            	;LOCKED?
	BEQ.S   20$
	MOVE.W  #M.LOCKED,D1
	CALL    P.BTNI
	DOGO	BSFULLSTOP
20$
	BTST    #4,(A0)            	;OPEN?
	BEQ.S   22$                	;NO - OK
	DO	P.BTN	             	;BUT THE NOUN
	DO	ISARE	             	;IS
	MSGGO	OPN              	;OPEN
22$
	LEA     TBL.DOOR(A4),A1
30$  
	CMP.W   (A1)+,D0
	BEQ.S   40$
	TEST_W  (A1)+              	;FINISHED?
	BNE.S   30$
35$
	DOGO	YCANT
 
40$
	CMP.W   (A1),D5            	;CORRECT KEY?  ONLY ONE PER DOOR!
	BEQ.S   50$
	MOVE.B  2(A5),D1           	;S&W OF KEY
	AND.B   #$F0,D1            	;SIZE ONL
	CMP.B   #$10,D1            	;KEYS = 1
	BEQ.S   45$
	MSG	IDNTT              	;'I DO NOT THINK THAT
	CALL    EXGNOUN        
	DO	P.QN
	DO	ISARE	
	MSG	QSUIT            	;'IS QUITE SUITABLE
	DOGO	BSFULLSTOP
45$
	CALL    EXGNOUN
	DO	P.TN
	DO	ISARE
	MSGGO	UNABLE 
 
50$
	BSET    #6,(A0)            	;LOCKED     
	CMP.W   #VNLOCK,VERB(A4)
	BNE.S   60$
	MOVE.W  #M.LOCKED,D1
	DO	P.TNIN
	DOGO	BSFULLSTOP
60$
	RET

	ENDC

*--------------------------------

EXGNOUN
	EXG     D0,D5
	EXG     A0,A5
	RET

*
* OpenDoor - opens (quietly) the noun in D0 if it's a door
*

	XDEF	OpenDoor

OpenDoor

	GETINFO

	BCLR    #5,(A0)           	;CLR CLOSED FLAG
	BSET    #4,(A0)           	;SET OPEN FLAG
 
	LEA     DOORS(A4),A1
30$
	CMP.W   (A1),D0           	;FIND CORRECT DOOR
	BEQ.S   40$
	TEST_L  (A1)+             	;+4  FINISHED IN TABLE?
	BNE.S   30$
	RET                       	;NOT A DOOR
40$
	MOVEQ   #0,D0
	LEA     PSUDAT(A4),A2     	;CLEAR A2
	ADD.W   8(A0),A2          	;OFFSET INTO PSEUDO DATA
	MOVE.B  (A2)+,D0          	;ROOM 1 (ONE SIDE OF DOOR)
	MOVE.B  (A2),D2           	;ROOM 2 (OTHER SIDE OF DOOR)
     
	MOVE.B  2(A1),D1          	;DIRECTION 1-2
	CALL    CONNECT2
	MOVE.B  -(A2),D0          	;JUMPED ON BY CONNECT2
	
	EXG     D0,D2             	;OTHER WAY
	MOVE.B  3(A1),D1          	;DIR 2-1
	BRA     CONNECT2
90$
	RET

*
* CloseDoor - closes the door given in D0
*
	XDEF	CloseDoor

CloseDoor

	GETINFO

	BCLR    #4,(A0)
	BSET    #5,(A0)
 
	LEA     DOORS(A4),A1
10$
	CMP.W   (A1),D0           	;FIND DOOR
	BEQ.S   20$
	TEST_L  (A1)+
	BNE.S   10$
	RET
 
20$
	CLR.L   D0
	LEA     PSUDAT(A4),A2
	ADD.W   8(A0),A2          	;POINTER TO PSEUDO DATA
	 
	MOVE.B  (A2)+,D0          	;ROOM NO. OF ONE SIDE OF DOOR
	MOVEQ   #-1,D2            	;DOOR CLOSED MSG
	MOVE.B  2(A1),D1          	;DIRECTION 1-2
	CALL    CONNECT2
	CLR.L   D0                	;GOING TO BE MULT BY 10
	MOVE.B  (A2),D0           	;ROOM ON OTHER SIDE OF DOOR
	MOVE.B  3(A1),D1          	;DIRECTION 2-1
	BRA     CONNECT2     

      END
