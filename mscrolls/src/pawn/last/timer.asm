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
*	@(#)timer.asm	2.2
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.TIMER
*
*--------------------------------

	include	"equates.asm"
	include	"roomequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"



	XREF    CHEAT,TIMERS,GETDATA,ROOM,P.TN,ISARE,DO.SCN,DEAD
	XREF    W.REBUIL,GETRELN,TIMER2,P.SCN,TIMER3,SETPRON,TIMERA
	XREF    CHKPAUSE,F_KMEET2,F_KRONOS,F_LIFTUP,F_KRDEAT,R.NOUN
	XREF    TIMER6,TIMER7,TIMER8,SETOUT,F_ADVCOL,C.STATUS


	XDEF    TIMER,KM2.ASK


 
TIMER
 
	IFNE    YCHEAT
	TEST_W  CHEAT(A4)
	BNE.S   99$
	ENDC
 
	MOVEQ   #$F,D4
10$
	LEA     TIMERS(A4),A1
15$
	TEST_B  1(A1,D4)
	DBNE	D4,15$
	BEQ.S   99$

	SUBQ.B  #1,1(A1,D4)
	BNE.S   20$
	MOVE.L  D4,-(A7)
	LSL.W   #1,D4
	LEA     TimeTable(PC),A1
	MOVE.W  2(A1,D4),D4
	JSR     0(A1,D4)
	MOVE.L  (A7)+,D4
20$
	DBRA	D4,10$
99$
	RET
 
TimeTable:

	DC.W    0			 ;Padding 
	DC.W    TSNOW-TimeTable           ;TIMER 1
	DC.W    KMEET2-TimeTable          ;TIMER 2
	DC.W    KMEET2.2-TimeTable        ;TIMER 3
	DC.W    LIFTNOISE-TimeTable       ;TIMER 4
	DC.W    KRDEATH1-TimeTable        ;TIMER 5
	DC.W    KRDEATH2-TimeTable        ;TIMER 6
	DC.W    KRDEATH3-TimeTable        ;TIMER 7
	DC.W    KRDEATH4-TimeTable        ;TIMER 8
	DC.W    ADVDEATH-TimeTable        ;TIMER 9
	DC.W    DOPOISON-TimeTable        ;TIMER A
 
 
 
TSNOW

	MOVE.W  #NSNOWMAN,D0
	GETINFO
	BCLR    #7,6(A0)
	CMP.W   #RNPLATEAU3,ROOM(A4)
	BNE.S   20$
	CALL    P.TN
	CALL    ISARE
	MSG     REBUIL
20$
	RET
 
 
KMEET2
 
	MOVE.W  ROOM(A4),D2
	CALL    GETRELN
10$
	MOVE.W  (A1)+,D0
	BEQ.S   20$
	CMP.W   #NKRONOS,D0          ;SILLY TO STOP HIM IF NPC *IS* KRONOS
	BEQ.S   25$                  ;BUT LOSE MSG ABOUT ARRIVAL
	GETINFO
	BTST    #6,4(A0)
	BEQ.S   10$
	ADDQ.B  #1,TIMER2(A4)
	RET
20$
	SCN     28
25$

KM2.ASK
*                       COS OF STUPID LINKER!!
KMEET2.ASK
 
	MOVE.B  #1,TIMER3(A4)
	MOVE.W  #NKRONOS,D0
	GETINFO
	MOVE.W  ROOM(A4),8(A0)
	CLR.W   6(A0)
*                                 ;IF U ASKJ K ABOUT BAND IT COMES
*                                 ;TO HERE! (URK!!)
 
	MOVE.W  #NCHEST2,D0          ;PUT CHEST IN ROOM
	GETINFO
	CALL    SETPRON
	MOVE.W  ROOM(A4),8(A0)
	CLR.W   6(A0)
	SCN     368                  ;POINTING...
	MOVEQ   #30,D1               ;'I CAN REMOVE..
10$
	BRA     P.SCN
 
KMEET2.2
 
	MOVE.W  #NKRONOS,D0
	CALL    CHKPAUSE
	BEQ.S   10$                  ;OK NOT PAUSED = FLY OFF
	MOVE.B  #1,TIMER3(A4)        ;REPEAT UNTIL NOT PAUSED
	RET
10$
	MOVE.W  #NKRONOS,D0
	GETINFO
	MOVE.L  #RNWORKSHOP,6(A0)    ;PUT KRONOS BACK
	ST      F_KMEET2(A4)
	MOVE.W  #-1,F_KRONOS(A4)	;IF ASKING KRONOS THEN STOP
*                                 ;DOUBLE 'FLYS OFF' MSG
	SCN     187
	RET
 
LIFTNOISE
 
	ST      F_LIFTUP(A4)
	CMP.W   #RNPASSAGE4,ROOM(A4)
	BNE.S   10$
	SCN     369
	RET
 
10$
	SCN     370
	RET
 
 
*--------------------------------
*
* KRDEATH1-4
*
* KRDEATH 1 TRIGGERED BY
* 'THROW BOTTLE AT KRONOS'
*
* IT GIVES FIRST DESC OF DISSOLVING
* BODY & SETS F_KRDEAT TO 1
* (FOR SP.PRESS (AEROSOUL BUTTON)
* THEN TRIGGERS KRDEATH2
* WHICH TRIGGERS KRDEATH3
*
*--------------------------------
 
KRDEATH.LOC 
 
	CALL_S  05$
	BEQ     P.SCN
	RET
 
05$
 
	MOVE.W  #NKRONOS,D0
	GETINFO
	MOVE.L  6(A0),D2
	BCLR    #31,D2               ;ZAP OUT BIT
	BTST    #24,D2               ;RELATED TO ROOM?
	BNE.S   10$
	CMP.W   ROOM(A4),D2          ;INC THIS ROOM?
	RET                          ;EQ = YES IS LOCAL
10$
	CALL    R.NOUN
	CMP.W   D0,D2                ;RELATED TO THIS ROOM?
	RET
 
KRDEATH1 
 
	MOVE.B  #1,F_KRDEAT(A4)
	 
	MOVE.B  #1,TIMER6(A4)        ;TRIGGER NEXT MSG
	MOVE.W  #191,D1              ;MSG1 - DISSOLVING
	BRA     KRDEATH.LOC   
 
KRDEATH2
 
	
	MOVE.B  #1,TIMER7(A4)        ;AND NEXT...
	MOVE.W  #371,D1              ;MSG 2 
	BRA     KRDEATH.LOC
 
KRDEATH3
	
	MOVE.B  #1,TIMER8(A4)
	MOVE.W  #372,D1              ;MSG 3 
	BRA     KRDEATH.LOC
 
  
KRDEATH4
 
	TEST_B  F_KRDEAT(A4)     ;DID WE GET SOUL?
	SMI     F_KRDEAT(A4)     ;ZAP TO 00 IF NOT (-1 IF WE DID)
	MOVE.W  #NKRONOS,D0
	GETINFO
	MOVE.L  6(A0),D1
	CALL    SETOUT             ;ZAP FROM GAME & CLEAN UP
	MOVE.W  #NCLOAK,D0
	CALL_S  UNWEAR 
	MOVE.W  #NHAT2,D0          ;POINTY HAT
	CALL_S  UNWEAR 
	MOVE.W  #NWAND,D0
	CALL_S  UNWEAR 
	MOVE.W  #373,D1
	BRA     KRDEATH.LOC
 
UNWEAR
	GETINFO
	MOVE.L  D1,6(A0)
	BCLR    #0,11(A0) ;NO LONGER WORN
	RET
 
 
*--------------------------------
 
 
ADVDEATH 
 
	ST      F_ADVCOL(A4)   ;ADV BEEN DEAD TOO LONG
	RET
 
 
*--------------------------------
 
 
DOPOISON 
  
	CMP.B   #1,C.STATUS(A4)    ;FATAL POISON?
	BNE.S   10$
	SCN     379                ;OK U DIE NOW
	BRA     DEAD
10$
	CMP.B   #2,C.STATUS(A4)    ;JUST POISONOUS
	BNE.S   20$
	SCN     381                ;OK VERY ILL  (MAYBE SOMETHIN U ATE)
	MOVE.B  #1,C.STATUS(A4)    ;OK DIE NEXT TIME
	MOVE.B  #3,TIMERA(A4)
	RET
20$
	SCN     380                ;DONT FEEL TO GOOD..
	RET


	END


