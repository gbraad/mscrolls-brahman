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
*	@(#)special0.asm	2.2
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALS0
*
*--------------------------------


	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"roomequ.asm"
	include	"macros.asm"
	include	"scoreref.asm"

  
      XREF     SP.LIGHT,SP.SCORE,SP.HAVEW,SP.FREEZ,SP.MIDAI
      XREF     SP.SNOW,SP.DEVIL,SP.ALCH,SP.DEMON,SP.DRAGO,SP.PORTE
      XREF     SP.ADVEN,SP.RNOUN,SP.KRON,SP.KRON3,SP.BREAK
      XREF     P.SCN,DO.SCORE,DO.SCN,PRTTXT
      XREF     GET_CONT,GETDATA,SETPRON,TIMER,FIXCHK
      XREF     W.UNABLE,W.FOR,W.SNPS
      XREF     P.LOC,P.XFERGS,P.QN,P.SUBV,P.SAY2,P.TN
      XREF     GETRELN,DEAD
      XREF     WHOOPS,CHEAT,USER.DB,SUBJECT,ROOM,JON.FLG
      XREF     LINE,SETOUT,TIMER1,TIMER9,CONNECT,DRP.PNT2
      XREF     SP.AT.HJ,SP.AT.AD,SP.AT.DE,TBL.ATTA,TMAP.ADV
      XREF     F_ALCH,F_BOULDE,F_GURU,F_PORT,F_GUARDS,F_AERO
	  XREF     REMASTER


      XDEF     SPECIALS,SP.ATTAC,SP.MELT,SP.WEAR,SP.UNWEA,SP.JOHN
      XDEF     SP.GIVE,SP.LEVER,SP.LOWER,SP.PUSH,SP.RUB,SP.TOUCH

SPECIALS

	CALL    SP.LIGHT
 
	IFNE     YCHEAT
	TEST_W  CHEAT(A4)          ;CHEAT MODE - NO SPECIALS
	BNE.S   90$
	ENDC
 
	TEST_W  USER.DB(A4)        ;GOT TO ADV ROOM?     
	BNE.S   90$                ;YES = NO MORE SPECIALS
 
	CALL    TIMER              ;DO TIMERS
	CALL    SP.SCORE           ;SUSS IF ROOM GIVES A SCORE
  
  
	CALL    SP.HAVEW           ;IN ADV. ROOM?
	TEST_W  SUBJECT(A4)        ;WHEN HORSE SP.FREEZE N/A & FALLS OVER
	BNE.S   10$
	CALL    SP.FREEZ           ;COLD ON PLATEAU
10$
	CALL    SP.MIDAI           ;DO MAZE AND WALL ITEMS
	CALL    SP.KRON3           ;GOT TO WORKSHOP?
	CALL    SP.SNOW            ;AT LAST !!
	CALL    SP.DEVIL
	CALL    SP.ALCH
	CALL    SP.DEMON
	CALL    SP.DRAGO           ;DO DRAGON MSGS?
	CALL    SP.PORTE           ;ISSUE WARNING?
	CALL    SP.ADVEN           ;MOVE THE ADVENTURER
 
	CALL    SP.RNOUN           ;GET NOUN NO. OF CURRENT ROOM
	 
	CMP.W   #RNINHUT,ROOM(A4)  ;ARE WE IN THE GURU'S HUT  ?  
	BEQ.S   SP.GURU            ;YES
	CMP.W   #NPLAIN2,D1
	BEQ     SP.JOHN
	CMP.W   #NPLAIN3,D1 
	BEQ     SP.KRON
	CMP.W   #NWILDERNESS,D1
	BEQ     SP.KRON
  
90$
	RET
 
  
 
SP.GURU

	XDEF    PROT3

PROT3
	TESTPROT 3

	LEA     F_GURU(A4),A6
	MOVEQ   #NBAND,D0
	CALL    FIXCHK
 
*      MOVEQ   #NBAND,D2
*      MOVEQ   #1,D5
*      MOVEQ   #12,D1
*      CALL    GETNOUNS           ;ALL FIXED TO IY
*      CLR.W   D4                 ;COUNTER
*
*SPGURU2
*
*      MOVE.W  (A1)+,D0
*      BEQ.S   SP.GURU4           ;FIN
*      GETINFO        
*      MOVE.B  2(A0),D1
*      AND.B   #$F0,D1            ;SIZE
*      CMP.B   #1,D1              ;MUST BE 1 TO COVER BAND
*      BLS.S   SPGURU2            ;NEXT
*      ADDQ    #1,D4
*      BRA.S   SPGURU2            ;DO REST
SP.GURU4
*      TEST_B  D4                 ;ANY?
	BEQ.S   30$                ;EQ = NOPE
	OR.W    #2,(A6)            ;ONCE COVERED
	SCORE  CBAND              ;U GET SOME POINTS!!!!!!!!!!!!!
30$
	MOVE.W  (A6),D1
	BNE.S   40$
	OR.W    #1,(A6)
	MOVEQ   #2,D1              ;GURU LAUGHS   
	BRA     P.SCN
40$
	CMP.W   #1,D1
	BLS     SPGURU3
	CMP.W   #3,(A6)
	BHI     50$
	SCN    3                  ;P.SCN WI D1=3
	OR.W    #4,(A6)
	MOVEQ   #NRICE,D0
	GETINFO        
	CLR.W   6(A0)
	MOVE.W  ROOM(A4),8(A0)
45$
	MOVE.W  #NBOWL,D0
	GETINFO        
	CLR.W   6(A0)
	MOVE.W  SUBJECT(A4),8(A0)
	BRA     SETPRON            ;SET 'IT' TO BOWL
50$  
	CMP.W   #8,(A6)
	BPL     SPGURU3
	MOVEQ   #NBOWL,D0
	GETINFO        
	CMP.W   #NGURU,8(A0)
	BNE.S   SPGURU3
	CALL    GET_CONT
	CMP.L   #NWATER*$10000,(A1)
	BEQ.S   55$
	MOVE.W  (A1),D0
	GETINFO        
	MOVE.B  11(A0),D1
	AND.B   #$E0,D1            ;EDIBILITY 
	CMP.B   #$60,D1
	BLS.S   59$		  ; Note the UNSIGNED branch
	CMP.W   #NRICE,D0	  ; OK, it's edible, so is it rice?
	BNE.S   51$
	SCN     9                  ;RICE TO LAST A LIFETIME
	BRA.S   45$
51$
	CMP.W   #NPERRIER,D0
	BNE.S   52$
	SCN     22                 ;PERRIER? NO PERRIER...
	BRA.S   45$
52$
	SCN     21		  ; General 'edible' copout
	BRA.S   45$
55$
	SCORE   GGWAT
	BCLR    #6,5(A0)           ;SET BOWL TO NON RETURNABLE
	MOVEQ   #NPOUCH,D0
	GETINFO        
	BCLR    #7,6(A0)           ;NO LONGER OUT OF ADV
*       MOVE.B  #NSTUMP,D0        ;THIS WOULD HAVE OVERFLOWED THE STUMP!!
*       GETINFO                   ;ANOTHER DISCOVERY BY AJS!!
*       ADD.B   #4,$A(A0)         ;THANK GOD FOR REAL PROGRAMMERS!!!
	MOVE.W  #$7FFF,(A6)
	MOVEQ   #19,D1             ;GIVE ADVICE ABOUT COLOURS
	BRA     P.SCN
 
59$
	SCN     23		  ;Incompetent fool! Its inedible
	BRA     45$  
	 
SPGURU3
	RET

*--------------------------------
*
*   SRC.ADV.SPECIALS1
*
*--------------------------------
  
  
 
   
SP.ATTAC
 
	LEA     TBL.ATTA(A4),A1
10$
	CMP.W   (A1)+,D0
	BEQ.S   20$
	TEST_W  (A1)+
	BNE.S   10$
	RET
 
20$
	MOVE.L  (A7)+,D1           ;ZAP STACK
	MOVE.W  (A1),D1            ;GET MSG#
	BMI.S   30$   
	PEA     DEAD(PC)           ;IF HIGH BIT CLR THEN RET VIA READ
30$
	AND.W   #$7FFF,D1
	CALL    P.SCN
	CMP.W   #NJOHN,D0
	BNE.S   40$                ;YOU DIE SECOND TIME!
	MOVE.W  #61,SP.AT.HJ+2(A4)
40$
	CMP.W   #NADVENT,D0
	BNE.S   50$                ;DITTO
	MOVE.W  #71,SP.AT.AD+2(A4)
50$
	CMP.W   #NDEVIL,D0
	BNE.S   60$
	MOVE.W  #70,SP.AT.DE+2(A4)
60$
	CMP.W   #NALCH,D0          ;KILLING ALCHEMISTYS?
	BNE.S   70$                ;NOPE
	MOVE.W  #RNPASSAGE,ROOM(A4) ;CHANGE ROOM SINCE THEY THREW U OUT
	MOVE.W  #NPASSAGE,D0
	BRA     P.LOC            ;PRINT NEW STATUS LINE
70$
	RET
*--------------------------------
   
SP.MELT
   
	CMP.W   #NSNOWMAN,D0
	BNE.S   90$
	EXG     D0,D5
	EXG     A0,A5
	GETINFO
	EXG     D0,D5
	EXG     A0,A5
	MOVE.B  3(A5),D1
	AND.B   #$0F,D1
	CMP.B   #$0A,D1       ;must be warm
	BNE.S   90$
	MOVE.B  #5,TIMER1(A4)
	SCORE   PASSS         ;POINTS FOR PASSING SNOWMAN
	CALL    SETOUT  
	MOVE.W  D5,D0
	GETINFO        
	CALL    DRP.PNT2
	SCN     40
	MOVE.L  (A7)+,D0
90$
	RET
*--------------------------------
  
SP.WEAR
   
	RET
*--------------------------------
   
SP.UNWEA
  
	RET
*--------------------------------
   
SP.JOHN
 
	TEST_W  JON.FLG(A4)
	BNE.S   99$
10$
	CALL    LINE
	MOVEQ   #NCART,D2
	CALL    GETRELN 
	
	LEA     HJLIST(PC),A2
30$
	MOVE.W  (A1)+,D0
	BEQ.S   20$
	BMI.S   30$
	MOVE.L  A2,A3
40$
	MOVE.L  (A3)+,D1
	BEQ.S   30$
	CMP.W   D1,D0
	BNE.S   40$
	SWAP    D1
	CALL    P.SCN
41$
	MSG     FOR
	GETINFO        
	MOVE.B  4(A0),D1
	AND.B   #$7,D1
	CALL    P.XFERGS
	
	
	PRINT   <'. '>
	BRA.S   30$
50$
	CALL    P.QN
	BRA.S   41$
20$
	SCN     57
	MOVE.B  #1,JON.FLG(A4)
	RET
   
99$
	MOVE.W  #NJOHN,D0
	GETINFO        
	MOVE.L  #RNPLAIN2,6(A0)    ;PUT JOHN IN ROOM (DE-RELATE)
	RET
 
*--------------------------------
   
HJLIST
	DC.W     46,NARMOUR2      ;HJ ARMOUR
	DC.W     47,NBOTTLE2      ;WHISKY BOTTLE
	DC.W     48,NBOTTLE4      ;ALE BOTTLE
	DC.W     49,NBOTTLE3      ;WATER BOTTLE
	DC.W     50,NRATIONS      ;RATIONS
	DC.L     0
*--------------------------------
   
SP.GIVE
	
	MOVE.L  (A7)+,A2
	CMP.W   #NALCH,D5
	BNE.S   50$ 
	CMP.W   #NRICE,D0
	BEQ.S   02$
	CMP.W   #NLUNCH,D0
	BNE.S   45$  
02$
	SCORE   FODAL          ;FOR FEEDING ALCHEMISTS
	CALL    SETOUT  
	MOVE.W  #224,D1
	MOVE.B  #$1F,F_ALCH(A4)
01$
	BRA     P.SCN
45$
*      CMP.W   #NBULLETS,D0
*      BEQ.S   03$
	CMP.W   #NLEAD,D0
	BNE     SP.GIVE90
03$
	SCORE   LEDAL           ;FOR GIVING THEM LEAD
	ST      F_ALCH(A4)
	CALL    SETOUT  
	MOVE.W  D5,D0              ;NOW ZAP ALCHEMISTS
	CALL    SETOUT
	MOVE.W  #225,D1
	BRA.S   01$
50$
	CMP.W   #NADVENT,D5
	BNE.S   60$
	CMP.W   #NCHEST2,D0        ;MAHOG CHEST?  
	BNE.S   58$
	SCORE   KILLA              ;POINTS FOR KILLING ADV
	SCN     337                ;MSG ABOUT ADV DEATH BY CHEST    
	BSET    #1,(A5)            ;ADVENTURER IS DEAD!!!!
	CLR.W   6(A5)              ;KILL LOC INFO - IN ROOM STRAIGHT
	BSET    #4,3(A5)           ;SET TO MOVEABLE
	MOVE.W  ROOM(A4),8(A5)
    
	CALL    SETOUT             ;CHEST IS GONE
	MOVE.W  #NHORSE,D0
	GETINFO        
	MOVE.W  ROOM(A4),8(A0)
	CLR.W   6(A0)              ;ZAP HORSE HERE.
	MOVE.B  #3,TIMER9(A4)      ;3 MOVES TO USE AEROSOUL
	RET
58$
	SCN     200
	BRA     SP.GIVE90
60$
 
 
	CMP.W   #NPORTER,D5
	BNE.S   70$
	CMP.W   #NBOTTLE2,D0       ;WHISKEY BOTTLE?
	BNE.S   70$
	MOVE.W  #NWHISKY,D0
	GETINFO        
	MOVE.W  #NBOTTLE2,D0       ;SO WE KNOW WHICH NOUN TO ZAP
	CMP.L   #$20000000+NBOTTLE2,6(A0)      ;IN BOTTLE?    
	BNE.S   65$
	SCN    219
	SCORE   BOZPO               ;POINTS!!
	ST      F_PORT(A4)          ;OK TO PASS BY NOW
	BRA     SETOUT
 
65$    MOVE.W  #NBOTTLE2,D0       ;WE JUST NOBBLED D0/A0
	GETINFO        
 
70$
	CMP.W   #NGURU,D5
	BNE.S   80$
	
	CMP.W   #NBOWL,D0
	BEQ.S   80$
	SCN     165                ;GURU HAS NO NEED FOR MATERIAL..
	RET
80$
	CMP.W   #NNOTE,D0
	BNE.S   SP.GIVE85
	CMP.W   #NGUARDS,D5
	BNE     SP.GIVE90
	TEST_W  F_GUARDS(A4)
	BNE     SP.GIVE90
	ST      F_GUARDS(A4)
	CALL    SETOUT             ;NOTE OUT OF ADV.
	SCORE   GVNOT
	
	MOVEQ   #NADVENT,D0
	GETINFO        
	BCLR    #7,6(A0)
	MOVE.W  8(A0),TMAP.ADV(A4) ;CURRENT LOC OF ADV
  
	MOVEQ   #27,D1             ;GUARDSLEAD YOU...
	BRA     P.SCN
 
SP.GIVE85
 
	CMP.W   #NAEROSOUL,D0
	BNE     SP.GIVE90
	CMP.W   #NDEVIL,D5
	BNE     SP.GIVE90
	
	CALL    SETOUT             ;ZAP AEROSOUL;
	
	CMP.B   #-1,F_AERO(A4)     ;NOBBLED KRONOS?
	BNE.S   SP.GIVE87          ;NOPE
	SCN    338
	SCORE   RMVBA              ;POINTS!!
	MOVE.W  #NBAND,D0
	GETINFO        
	BRA     SETOUT
 
SP.GIVE87
 
	SCN     339                ;FAILED- DEVIL FEEDS U TO DEMONS
	TEST_B  REMASTER(A4)
	BEQ.S	90$
	MOVE.W  #RNCHAMBER,ROOM(A4)
90$
	BRA     DEAD
 
SP.GIVE90
	
	CMP.W   #NLEWIS,D5       ;GIVE LEWIS?
	BNE.S   20$
	CMP.W   #NBOTTLE4,D0     ;BEER BOTTLE?
	BNE.S   20$
10$
	MOVE.W  #NALE,D0
	GETINFO        
	MOVE.W  #NBOTTLE4,D0       ;SO WE CAN ZAP RIGHT NOUN
	CMP.L   #$20000000+NBOTTLE4,6(A0) ;CONTAINED BY BOTTLE?
	BNE.S   15$
	SCORE   BOZJL             ;POINTS
	SCN     286               ;THANKS PAL!   
	BRA     SETOUT
15$
	GETINFO                   ;COS WE JUST STEPPED ON EM!
20$
	CMP.W   #NJOHN,D5          ;GIVE JOHN XYZZY?
	BNE.S   30$
	MOVE.B  4(A0),D1           ;GET VALUE
	AND.B   #$07,D1            ;IGNORE IF MONEY 
	BNE.S   30$                ;OK HE'LL TAKE IT
	MOVE.W  D5,D0              ;JOHN SAYS
	CALL    P.SAY2
	SCN     230                ;'WHAT DO I WANT WITH THIS...
	ST      WHOOPS(A4)          NO MORE OUTPUT
	RET                        ;'.. COULDN'T SELL IT IN 10^6 YEARS..
30$
	CMP.W   #NSNOWMAN,D5       ;GIVING THINGS TO SNOWMAN?
	BNE.S   40$
	MOVEQ   #24,D1             ;TRIES TO COMMUNICATE..
	BRA     P.SCN
 
40$
     
	MOVE.L  A2,-(A7)
	RET
 
*--------------------------------
  
SP.LEVER
 
	MOVE.L  (A7)+,A2
	CMP.W   #NBOULD,D0
	BEQ.S   10$
	CMP.W   #NROCKS,D0
	BEQ.S   05$
	CMP.W   #NROCKFALL,D0
	BNE     LV99
05$
	SCN    37                 ;AS YOU MOVE ONE OF THE ROCKS...
	BRA     DEAD
10$
	CALL    LV80
	MOVEQ   #NRAKE,D2
	CMP.W   #NHOE,D5
	BEQ.S   20$
	MOVEQ   #NHOE,D2
	CMP.W   #NRAKE,D5
	BEQ.S   20$
09$
	CALL    P.SUBV
	MSGGO	UNABLE
 
15$
	SCN     340	     ;Not strong enough alone
	RET
20$
	EXG     D5,D0
	MOVE.L  A5,A0
	CALL    FIXCHK
	CMP.W   D2,D1              ;OK?
	BEQ.S   21$
22$
	MOVE.W  (A1)+,D1
	BEQ.S   15$
	CMP.W   D2,D1
	BNE.S   22$
21$
	SCORE  LVRBO
	SCN    315                ;TOGETHER..
*      CALL    CALCNET            ;TO 0
	TEST_B  F_BOULDE(A4)
	BNE.S   LV11   
	ST      F_BOULDE(A4)
	SCN    319              ;revealing a nw path
LV11
	MOVEQ   #DIR_NW,D1
	MOVEQ   #RNTRACK,D2
	CALL_S  20$                ;CONNECT
	MOVEQ   #DIR_U,D1
	MOVEQ   #RNTRACK,D2
20$
	BRA     CONNECT
      
LV80
	MOVE.B  1(A5),D1
	AND.B   #$F,D1
	BEQ.S   LV81
	CMP.B   #4,D1
	BHI.S   LV81
	MOVE.W  D5,D0
	MOVE.L  A5,A0
	CALL    P.TN
	MSG     SNPS
	BSET    #2,(A5)
	MOVE.L  (A7)+,D0
LV81
	RET
LV99
	MOVE.L  A2,-(A7)
	RET
   
 
 
*--------------------------------
 
SP.LOWER
	
	
	RET
	
*--------------------------------
 
SP.PUSH
 
	CMP.W   #NLWALL,D0
	BEQ     SP.BREAK
	CMP.W   #NROCKFALL,D0
	BEQ.S   80$
	CMP.W   #NROCKS,D0
	BNE.S   90$
80$
	MOVE.L  (A7)+,A2
	SCN     37                 ;ROCKS FALL ON U
	BRA     DEAD
90$
	RET
  
*--------------------------------
 
SP.RUB
 
SP.TOUCH
  
	RET

      END


