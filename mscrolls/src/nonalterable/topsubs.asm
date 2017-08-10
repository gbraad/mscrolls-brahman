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
*	@(#)topsubs.asm	2.8
*	5/16/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.TOPSUBS
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"


*--------------------------------

*     XDEF     P.IDFY       ;DEFINED ELSEWHERE

      XDEF     FNDSEAT,P.BYAN,P.SEPA,DEAD,P.TNIN
      XREF	P.TNSTOP
      XDEF     ISDOOR,P.MHAV,P.MHAVSTOP

      XREF     ROOM,GETRELN,GETDATA,R.NOUN,GETNOUNS,W.NOSEAT,WHOOPS,W.IDFY
      XREF     W.BUT,P.SUBV,W.NOTSEA,P.TN,SETOUT,W.SEPAWY
      XREF     EXTRA,DIED,ISARE,W.NOW,P.MSG,P.SUB

      XREF     P.BTN,W.ARDY,PP.OP.CL,W.HN


*
*P.IDFY
*       
*       ST      WHOOPS(A4)
*       BRA     W.IDFY
*
*
	 
FNDSEAT
	
*FIND SOMEWHERE TO SIT DOWN
	
	CLR.W   D7
	MOVE.W  ROOM(A4),D2
	DO	GETRELN	           ;TO A1    
10$
	MOVE.W  (A1)+,D0           ;GET ONE
	BEQ.S   20$                ;NONE FOUND
	GETINFO        
	TEST_B  6(A0)              ;IS A ROOM LOC
	BNE.S   10$                ;NO, IS SOMEWHERE ELSE
	BTST     #3,5(A0)          ;IS IT SITTABLE?
	BEQ.S   10$                ;NO
	BRA.S   99$  ;EXIT
20$
	DO	R.NOUN
	MOVE.W  D0,D2              ;SET UP TARGET.
	DO	GETNOUNS
40$
	MOVE.W  (A1)+,D0
	BEQ.S   30$                ;NOWHERE TO SIT
	GETINFO        
	BTST    #3,5(A0)           ;SITTABLE?
	BEQ.S   40$                ;NO
	BRA.S   99$                ;OPK
30$
	MSG	NOSEAT
	MOVEQ   #1,D7              ;ERROR
99$
	RET     ;EXIT
  
*--------------------------------
	        
P.BYAN
	
*PRINT "BUT (YOU ARE) NOT SEATED."
	
	MSG	BUT
	DO	P.SUBV
	MSGGO	NOTSEA
	
*--------------------------------
	
P.SEPA
	
	DO	P.TN
	DO	SETOUT	            ;SET AS OUT (NOCAN TIE LIQUIDS)
*                                 ;BUT CALLIS SHORTER THAN BSET
	MSGGO	SEPAWY
	
*--------------------------------
*
* DEAD - NOT REALLY A SUB
*        BUT *CERTAINLY*
*        NOT A SPECIAL
*
* SO ITS HERE!
*
* GENERALLY HALTS PARSING &
* PROCESSING OF COMMANDS
* AND SETS DIED FLAG TO LET
* SCREEN KNOW ABOUT DEATH
*
*--------------------------------
 
DEAD
 
	ST      WHOOPS(A4)         ;FORCE EXIT PROCESS
	CLR.W   EXTRA(A4)          ;KILL FOLLOWING COMMANDS
	ST      DIED(A4)           ;FLAG DEATH FOR SCREEN
	RET                        ;BYE
 
 
*--------------------------------
*
* P.TNINOW
*
* PRINT THE NOUN IS NOW MSG. D1
*
*--------------------------------
 
 
P.TNIN
 
	DO	P.TN
	DO	ISARE
	MSG	NOW
	DOGO	P.MSG
 
 
*--------------------------------
*
* ISDOOR   RETURNS EQ IS YES
*
*--------------------------------
 
 
ISDOOR
 
	LEA     PP.OP.CL(A4),A1  ;DOORS + CUPBOARD ETC
10$
	CMP.W   (A1)+,D0
	BEQ.S   90$
	TEST_W  (A1)+
	BNE.S   10$
	SETNE                     ;SET NOT EQUAL   
90$
	RET
	
	        
*--------------------------------
	        
P.MHAV_SUB
 
	DO	P.SUB
	MSGGO	HN

P.MHAV

	CALL_S	P.MHAV_SUB
	DOGO	P.TN

P.MHAVSTOP

	CALL_S	P.MHAV_SUB
	DOGO	P.TNSTOP
	
*--------------------------------

      END

