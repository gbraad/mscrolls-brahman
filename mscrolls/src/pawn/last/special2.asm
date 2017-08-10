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
*	@(#)special2.asm	2.4
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALS2
*
*--------------------------------




	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"roomequ.asm"
	include	"macros.asm"
	include	"scoreref.asm"


      XDEF     S_MAT,S_PEDEST,S_FBOARD

      XDEF     SP.ADVEN,SP.RIDE,SP.GO.N,SP.TEAR,SP.READ,SP.KNOCK,SP.LEAN
      XDEF     SP.PORTE,SP.ASK,SP.ASK2,SP.GET,SP.MOVE.

      XREF     R.NOUN,GETDATA,CLOCK,TMAP.ADV,COUNT.AD
      XREF     DEAD,MOVENPC,MOVE.ADV,NPCINRM,GO.N,DO.SCN,DO.SCORE
      XREF     RIDING,RIDFLG,VERB,ADVERB,CONJ,ROOM,YNHANDLR
      XREF     V.SIT,V.PUT,CONNECT,CONNECT2,LINE,SETOUT,OPEN,TIMER2
      XREF     F_GUARDS,F_PWALL,F_PDDOOR,F_PORT,F_PORT2,F_GURU,F_KMEET2
      XREF     KM2.ASK
      XREF     SP.DEVL2,SP.PRIN2,P.SCN,SP3.FLAG
      XREF     P.SAY2,P.SUB,P.TN
      XREF     SETPRON,W.YSI,W.MVG,W.ATMOMT,UTIL.BS,YCANT

      XREF     TBL.MOVE

SP.ADVEN
 
	CALL    R.NOUN             ;GET ROOM'S NOUN #
	GETINFO                    ;D0,A0
	MOVE.L  D0,D5              ;SAVE
	MOVE.L  A0,A5              ;DITTO 
 
	MOVEQ   #NADVENT,D0
	GETINFO        
	BTST    #1,(A0)            ;DEAD?

SP.ADV1

	BNE     90$                ;YES - FORGET HIM
	BTST    #7,6(A0)           ;OUT?
	BNE.S   90$                ;DITTO  
 
	CMP.L   #$01000000+NCAVERN4,6(A0)  ;GOT TO KEY? 
	BNE.S   10$
	PUSH_L  D0/A0        ;SAVE EM A SEC
	MOVE.W  #NKEY,D0
	GETINFO                    ;GET BLUE KEY INFO
	CMP.W   #NNICHE2,8(A0)     ;STILL THERE?
	BNE.S   05$                ;NOPE - FORGET IT   
	MOVE.L  #$01000000+NADVENT,6(A0) ;REL TO ADVENT!!
05$
	PULL_L  D0/A0
10$
	CMP.L   #$01000000+NJAIL,6(A0) ;HE GOT TO PRINCESS?
	BNE.S   12$                ;NOPE..
	MOVE.L  D0,D1
	MOVE.L  A0,A1
	MOVE.W  #NPRINCESS,D0
	GETINFO        
	BSET    #1,5(A0)           ;kill princess initial state stuff
	CMP.W   #RNJAIL,8(A0)      ;STILL IN JAIL?
	BNE.S   11$
	MOVE.L  #$01000000+NADVENT,6(A0)
	CMP.W   #RNJAIL,ROOM(A4)   ;WE IN ROOM TO?
	BNE.S   11$                ;NOPE - OK
	SCN     206                ;ADV JUMPS IN FRONT OF U..
	BRA     DEAD
11$
	MOVE.L  A1,A0
	MOVE.L  D1,D0
12$
	LEA     TMAP.ADV(A4),A6
	CMP.W   #500,CLOCK(A4)     ;TIME OUT ON ADV
	BPL.S   15$			;was BMI - oooops 
	TEST_W  F_GUARDS(A4)       ;ADV MOVES WHEN U GIVE NOTE
	BNE.S   15$                ;TO GUARDS   
90$
	RET
  
15$
	NOT.B   COUNT.AD(A4)         ;MOVE EVERY OTHER GO
     
	MOVE.W  8(A0),-(A7)         ;SAVE OLD ADVENT LOC
 
	CALL    MOVENPC             ;DO THE MOVE!
	                            
	MOVE.W  (A7)+,D2            ;GET OLD LOC
	MOVE.W  8(A0),D1            ;GET NEW LOC
	CMP.W   D1,D2               ;ANY CHANGE?
	BNE.S   70$
40$
	TEST_B  MOVE.ADV(A4)        ;MOVD OF LATE?
	BEQ     90$                 ;NO LEAVE MSGS    
	MOVE.W  8(A0),D2            ;INCASE DIDNT NOVE THIS TIME
	CMP.W   D5,D2               ;IS THERE AN ADV IN CURRENT ROOM?  
	BNE     90$                 ;NOPE!
 
	CMP.W   #NPLATEAU3,8(A0)    ;IS ADV AT SNOWMAN?    
	
	BNE.S   55$
	MOVEQ   #41,D1              ;FIGHTING SNOWMAN MSG
	BRA.S   60$
55$
	MOVEQ   #42,D1              ;ADV MOUNTED MSG
	BTST    #2,5(A5)            ;LIGHT IN ROOM?
	BNE.S   60$                 ;1=LIGT
	MOVEQ   #44,D1              ;WALKS PAST,LEADING HORSE
60$
	CLR.W   MOVE.ADV(A4)
	BRA.S   87$                 ;P.SCN
	 
70$
	ST      MOVE.ADV(A4)        ;OK - HE'S MOVED
 
	CMP.W   D5,D2               ;HAS ADV JUST LEFT ?
	BNE.S   80$                 ;NO
	SCN     341
	BRA.S   85$  
80$
	MOVE.W  8(A0),D2            ;GET LOC OF ADVENT NOW
	CMP.W   8(A0),D5            ;HAS HE REALLY ARRIVED?
	BNE     90$                 ;NO!!   
	SCN     342
 
* AN ADVENTURER HAS JUST ARRIVED"
85$
	MOVE.W  #NADVENT,NPCINRM(A4)
	BTST    #2,5(A5)            ;LIT ? (=1)
	BNE.S   86$
	MOVE.W  #343,D1
	BRA.S   87$                ;LEADING A WHITE HORSE..
86$
	MOVE.W  #344,D1            ;RIDING..
87$
	BRA     P.SCN
 
 
SP.RIDE
 
	MOVE.L  (A7)+,A2
	CMP.W   #NHORSE,D0
	BEQ.S   10$
	CMP.W   #NPLATFORM,D0
	BEQ     GO.N
01$
	MOVE.L  A2,-(A7)
99$
	RET
 
10$
    
	MOVE.W  #1,RIDING(A4)
	BSET    #0,RIDFLG(A4)       ;TEST & SET
	BNE.S   11$
	SCN     214                 ;RIDING HORSE MSG
11$
	CMP.W   #VNSIT,VERB(A4)     ;ALREADY IN SIT
	BEQ.S   01$
	MOVE.W  #3,ADVERB(A4)
	BRA     V.SIT
 
 
SP.GO.N
 
	CMP.W   #NROPE,D0           ;CLIMB ROPE
	BNE.S   05$
	CMP.W   #RNROOM5,ROOM(A4)   ;FROM ROOM ?
	BNE.S   02$
	MOVE.W  #NLEDGE3,D0 
	BRA     GETDATA
02$
	CMP.W   #RNLEDGE3,ROOM(A4)  ;UP FROM LEDGE
	BNE.S   05$
	MOVE.W  #NROOM5,D0
	BRA     GETDATA
05$
	CMP.W   #NPWALL,D0          ;GO PAPER WALL/RUN AT IT...
	BNE.S   20$
	SCN     96
	SCN     97
	MOVE.L  (A7)+,D0
	BRA     DEAD
20$
	RET


SP.TEAR
 
	CMP.W   #NPWALL,D0          ;ZAP THE WALL
	BNE.S   10$
	MOVEQ   #RNLEDGE3,D2        ;ROOM TO GO TO
	MOVEQ   #DIR_D,D1           ;IN DIRECTION 'D'
	CALL    CONNECT             ;FROM CURRENT ROOM
	MOVEQ   #RNROOM5,D2         ;ROOM TO GO TO
	MOVEQ   #DIR_U,D1           ;IN DIRECTION 'U'
	MOVEQ   #RNLEDGE3,D0        ;FORM ROOM
	CALL    CONNECT2            ;D0&D2 CONNECTED
	ST      F_PWALL(A4)
	SCN     96
10$
	RET
 
 
*--------------------------------
 
 
SP.READ
 
 
	CMP.W   #NNOTE,D0
	BEQ.S   10$
	RET                         ;OK
 
10$
	SCN     314                 ;TUT TUT,MAIL...
	MOVE.L  (A7)+,D0            ;ZAP THE STACK
	BRA     DEAD
 
 
SP.LEAN 
	
	MOVE.L  (A7)+,D7            ;SAVE RET ADDR. + POP STACK
	CMP.W   #NPWALL,D0          ;LEAN ON THE PAPER WALL ??
	BNE.S   10$                 ;NOPE!
	SCN     320                 ;OOPS THE PAPER WALL GIVES UNDER YOUR WEIGHT
	SCN     144                 ;DEATH IN PIT OF FIRE!   
	BRA     DEAD
10$
	CMP.W   #NSWALL,D0
	BNE.S   90$
	SCN     321                 ;THE STONE WALL SUPORTS YR WEIGHT EASILLY
	RET
 
90$
	MOVE.L  D7,-(A7)            ;FIX THE STACK
	RET
 
 
SP.KNOCK
  
	CMP.W   #NDDOORS,D0         ;PORTDDOORS?
	BEQ.S   10$
	CMP.W   #NKNOCKER,D0
	BEQ.S   10$
	CMP.W   #NDOOR2,D0          ;ADV ROOM DOOR
	BEQ.S   SP.KNOCK2
05$
	RET
 
10$
	ADDQ.W  #1,F_PDDOOR(A4)        ;HOW MANY KNOCKS?
	BNE.S   20$
	MOVE.W  #216,D1             ;VOICE FROM BEHIND...
	BRA.S   80$
20$
	MOVE.W  F_PDDOOR(A4),D7
	CMP.W   #3,D7
	BLS.S   50$
	CMP.W   #4,D7
	BNE.S   05$
	ST      F_PORT2(A4)
	MOVE.W  #NPORTER,D0
	GETINFO        
	BCLR    #7,6(A0)            ;OK - PORTER EXISTS NOW
50$
	ADD.W   #255,D7             ;GET MSG#    (BASE-1)
	MOVE.W  D7,D1
80$
	MOVE.L  (A7)+,D7            ;ZAP THE STACK 
	BRA     P.SCN
 
 
SP.KNOCK2
 
	BTST    #6,(A0)            ;'THUD' IF ADV DOOR IS NOT LOCKED
	BEQ.S   99$
  
	LEA     SP.KNOCK.YN(PC),A1 ;WHERE TO COME ON YES/NO
	CMP.L   YNHANDLR(A4),A1    ;JUST KNOCKED?
	BNE.S   10$                ;NO = GIVE KNOCKING MSG
	MOVE.W  #NVOICE,D0
	CALL    P.SAY2
	SCN     264                ;'WILL YOU STOP KNOCKING ...
	BRA.S   90$                ;POP AND RET
	
10$
	MOVE.W  #NVOICE,D0
	CALL    P.SAY2
	SCN     81                 ;U WEARING A SILVER BAND?
	MOVE.L  A1,YNHANDLR(A4)    ;SET UP YES/NO STUFF
 
90$
	MOVE.L  (A7)+,D0           ;POP
99$
	RET
 
SP.KNOCK.YN
 
* D5 = 0 = YES 1 = NO
 
	TEST_W  D5   ;YES OR NO? (EQ = YES)
	BEQ.S   10$                ;YES = GO AWAY!
	MOVEQ   #NBAND,D0
	GETINFO        
	BTST    #7,6(A0)           ;BAND BEEN REMOVED?
	BNE.S   20$                ;YES = OK     
	SCN     83                 ;LIAR!!!! LIAR!!!
	CALL    LINE
10$
	MOVE.W  #NVOICE,D0
	CALL    P.SAY2
	SCN     82                 ;CANNOT LET U IN YET!
	BRA.S   90$                ;EXIT
 
20$
	MOVE.W  #NVOICE,D0         ;BETTER REMOVE THE VOICE
	CALL    SETOUT
	MOVE.W  #NDOOR2,D0         ;OPEN THE DOOR!!!! (GOSH WOW AMAZING!)
	GETINFO        
	MOVEQ   #$20,D1            ;SO WE LEAVE D1 = (A0) FOR OPEN
	MOVE.B  D1,(A0)            ;DOOR NOW JUST CLOSED
	MOVE.W  #VNOPEN,VERB(A4)   ;IFFY! WHERE IS THE STACK AT???
	MOVE.W  #-1,CONJ(A4)       ;MAKE SURE
	MOVEQ   #0,D5              ;YAWN...
	CALL    OPEN               ;HOPEFUL EH?
90$
 
	CLR.L   YNHANDLR(A4)       ;OK BACK TO COP OUTS
	RET
 
 
SP.PORTE
 
	TEST_W  F_PORT2(A4)
	BEQ.S   99$
	CLR.W   F_PORT2(A4)    ;ONLY WARNED ONCE
	CALL    LINE
	SCN     218
	CALL    LINE
	MOVE.W  #NDDOORS,D0
	GETINFO        
	MOVEQ   #$21,D1      ;SO AS OPEN CAN SUSS IT OK
	MOVE.B  D1,(A0)             ;SET PC CLOSED - MAY BE OPENED
	MOVE.W  #-1,CONJ(A4)
	MOVEQ   #0,D5              ;NO - WE DONT WANT TO UNLOCK IT!
	CALL    OPEN
99$
	RET
 
 
*--------------------------------
 
 
SP.ASK
 
	CMP.W   #NGURU,D0
	BNE.S   05$
	CMP.W   #1,F_GURU(A4)      ;ONE = BAND NOT YET COVERED
	BNE.S   05$                ;OK - WILL TALK SINCE U HAVE COVERED IT 
 
	CALL    P.TN
	SCN     265                ;'IGNORES YOU, AND CONTINUES..
	MOVE.L  (A7)+,D0
	RET
 
05$
	CMP.W   #NGUARDS,D0
	BNE.S   10$
	TEST_W  F_GUARDS(A4)
	BEQ.S   10$                 ;CANNOT TALK AFTER GIVING NOTE
	MOVE.W  #166,D1             ;IF THE KING SEES US ...
	MOVE.L  (A7)+,D0            ;ZAP STACK
	BRA     P.SCN
 
10$
	CMP.W   #NBAND,D5
	BNE.S   20$
	TEST_B  F_KMEET2(A4)    ;STOP KMEET2 HAPPENING TWICE
	BNE.S   20$
	CMP.W   #NKRONOS,D0        ;MAKE IT HAPPEN NOW IF U ASK KRONOS
	BNE.S   15$
	MOVE.L  (A7)+,D0
	CLR.B   TIMER2(A4)         ;DO IT NOW INSTEAD!      
	ST      F_KMEET2(A4)    ;COS KMEET22$ WILL NOT HAPPEN
	BRA     KM2.ASK
15$
	TEST_W  F_GUARDS(A4)    ;IF NOT ASKING KRONOS
	BEQ.S   20$                ;THEN MUST HAVE GIVEN GUARDS THE NOTE
	MOVE.B  #10,TIMER2(A4)
20$
	RET
 
SP.ASK2
 
	CALL    SP.DEVL2     ;MAY POP STACK (TWICE!)
	CALL    SP.PRIN2     ;DITTO !! - WHY WAS THIS REM'ED OUT?
 
 
	RET                  ;LEAVE IT IN!!!!


*--------------------------------
*
*  SRC.ADV.SPECIALS3
*
*--------------------------------
 
 
SP.MOVE.
 
	ST      SP3.FLAG(A4)        ;FLAG TO SUPPRESS SCAN'S MESSAGES
	PUSH_L  D0/D1/D5/A0/A5/A6

	LEA     TBL.MOVE(A4),A6
10$
	CMP.W   (A6)+,D0
	BEQ.S   30$
	TEST_L  (A6)+
	BNE.S   10$
20$
	CLR.W   SP3.FLAG(A4)        ;ALLOW SCAN'S MESSAGES
	BRA.S   SP3.RET             ;NOTHING REVEILED
 
30$    
	BSET    #7,(A6)             ;TEST TO SEE IF ALREADY DONE+SET FLAG
	BNE.S   20$
      
	MOVE.L  (A6),D7
	AND.L   #$7FFFFFFF,D7           ;MASK OUT FLAG
	JMP     0(A4,D7.L)          ;GOTO SPECIAL (A6 INTACT)
 
SP3.RET

	CLR.W   SP3.FLAG(A4)
	PULL_L  D0/D1/D5/A0/A5/A6
	RET
 
 
*--------------------------------
 
 
 
S_FBOARD
 
	MOVE.W  #NTREEDOOR,D0
	GETINFO        
	BTST    #4,(A0)             ;DOOR MUST NOT BE OPEN
	BEQ.S   10$ 
	BCLR    #7,(A6)             ;ALLOW ANOTHER TRY
	BRA.S   SP3.RET
 
10$
	MOVE.W  #RNTREETRUNK,D0     ;ROOM 1
	MOVE.W  #RNSTAIRCASE,D2
	MOVEQ   #DIR_D,D1           ;DIRECTION ROOM 1-2
	CALL    CONNECT2
	MOVE.W  #RNSTAIRCASE,D0
	MOVE.W  #RNTREETRUNK,D2
	MOVEQ   #DIR_U,D1
	CALL    CONNECT2
 
	SCN     316                ;A DARK OPENING...
	MOVE.L  (A7)+,D0           ;POP
	BRA.S   SP3.RET
 
 
*--------------------------------
 
 
 
S_MAT
 
	MOVE.W  #NWKEY,D0
	GETINFO        
	BCLR    #1,6(A0)            ;NO LONGER HIDDEN
	BEQ     SP3.RET
	MOVE.L  #RNGARDENS,6(A0)    ;SET LOC TO CURRENT ROOM  
 
	CALL    SETPRON            ;SET 'IT' = KEY
	CMP.W	#VNLIFT,VERB(A4)
	BEQ.S	04$
	CMP.W	#VNMOVE,VERB(A4)
	BNE.S	10$		;cos get has told us you have the mat
04$
	CALL    P.SUB
	MSG     YSI
	MOVE.W  #NMAT,D0
	GETINFO        
	MSG     MVG
	CALL    P.TN
	MOVE.L  (A7)+,D0           ;POP STACK
10$
	CALL    UTIL.BS
	SCN     317                ;UNDER IT IS A KEY...
90$
	BRA     SP3.RET
 
 
 
*--------------------------------
 
S_PEDEST
 
	MOVE.W  #NNICHE2,D0
	GETINFO        
	BCLR    #7,6(A0)
	SCN     345                 ;MANAGE TO SHIFT PEDESTAL
	MOVE.L  (A7)+,A1            ;SUFFLE STACK/REG TO POP
	BRA     SP3.RET
 
  
*--------------------------------
  
 
 
SP.GET
 
	MOVE.L  (A7)+,A1            ;POP STACK
	CMP.W   #NSNOW,D0
	BNE.S   20$
	TEST_W  D5                  ;USING SOMETHING?
	BNE.S   10$                 ;YES
	SCN     346
	RET

10$
	MOVE.W  #5,CONJ(A4)         ;IN       
	BRA     V.PUT
20$
	CMP.W   #NLUMPS,D0          ;GETTING LEAD AT ROCKFACE?
	BNE.S   30$
	TEST_W  CONJ(A4)         ;WITH?
	BNE.S   30$
	TEST_W  D5
	BEQ.S   30$
	MOVE.B  3(A5),D2            ;SHAPE & TEXT 
	LSR.B   #6,D2               ;TOP 2 BITS COUNT
	TEST_B  D2                  ;SHARP ENOUGH?
	BLS.S   30$                 ;NOPE
	BSET    #4,3(A0)            ;MAKE LUMPS 'GETABLE'
	CLR.W   6(A0)               ;GET IT
	MOVE.W  ROOM(A4),8(A0)
	MOVE.W  #-1,CONJ(A4)
	CLR.L   D5
30$
	CMP.L   #NSBALBX+CONT.MASK,6(A0) ;CONTAINED BY BALLOT BOX?
	BEQ.S   35$
	CMP.L   #NLBALBX+CONT.MASK,6(A0) ;IN LARGE BALLOT BOX?
	BNE.S   40$
35$    
	CALL    YCANT
	MSGGO	ATMOMT
 
40$
	MOVE.L  A1,-(A7)            ;FIX STACK
	RET


   END

