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
*	@(#)setmode.asm	2.2
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SETMODE1
*
*--------------------------------

	include	"equates.asm"
	include	"roomequ.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"macros.asm"


	XREF    ROOM,RIDING,SEATED,P.SUB,W.GETOFF,SCRPNT,DO.SCN,WCARR,VALID.NP
	XREF    WHRIDING,CHEAT,USER.DB,F_PORT,NEXITFLG,F_LIFT,F_LIFTUP
	XREF    DEAD,CARRIED,F_DRAG2,F_DRAG,F_DRAG3,VERB,F_VSHAFT,P.SAY2
	XREF    GETDATA,P.SCN,F_ALCH2,F_ALCH,F_PRFOLL,VALID,RND.D1,LINE
	XREF    W.ROPED,R.NOUN2,F_DEVIL,SETOUT,NPC.OK,F_TRADE,W.CGTW

	XDEF    SETMODE,NEWROOM,NEWROOM2
    XREF    SP.NEWROOM


*--------------------------------
*
* SETMODE - HANDLES SPECIALS
*           THAT CONTROL MOVEMENT
*           BETWEEN LOCATIONS 
*
* NOTE   ROOM TO ENTER IS IN D1.B
*
*        BYTEWISE
*       ++++++++++
*
*--------------------------------
  
  
 
SETMODE
 
	LEA     ROOM(A4),A5        ;SAVE MEM
	AND.W   #$FF,D1            ;URK!! - USUALLY FFXX - XX = ROOM
 
	TEST_W  RIDING(A4)         ;ON A VEHICLE?
	BNE.S   01$                ;OK - DONT GET UP THEN
	TEST_W  SEATED(A4)         ;SITTING ON SOMETHING?
	BEQ.S   01$                ;NOPE
	CALL    P.SUB            ;'YOU '
	MSG	GETOFF
	CLR.W   SEATED(A4)
01$
	CALL    WHRIDING           ;STOPPED COS OF VEHICLE
	TEST_W  D7                 ;0= NOT STOPED
	BEQ.S   02$
	RET
02$
	IFNE     YCHEAT
	TEST_W  CHEAT(A4)          ;IF CHERATING THEN FORGET SPECIALS
	BNE     NEWROOM2           ;!!! BEWARE - THIS LETS U WALK OUT OF A
	ENDC                        ;ROOM WHILEST TIED TO CONTENTS OF IT!! 
 
	TEST_B  USER.DB(A4)        ;IF IN 'USER DEBUG' THEN FORGET SPECIALS
	BNE     NEWROOM            ;BUT TAKE CARE OF 'TIED TO ROOM' BITS
*
*
*  Specials code starts here ........
*
*
	
	CMP.W   #RNPASSAGE2,(A5)   ;PORTERS PASSAGE?
	BNE.S   10$
	CMP.B   #RNSHAFT2,D1       ;PASSING PC?
	BNE.S   10$
	TEST_W  F_PORT(A4)         ;PAID PORTER?
	BNE.S   10$
	SCN     220                ;KILLED BY PORTER W EMPTY W. BOT!!
	BRA     DEAD
 
10$
	CMP.W   #RNCHAMBER2,(A5)   ;ESCAPING FROM DRAGON?  
	BNE.S   SETMODE2
	MOVEQ   #NADVENT,D0        ;HONEST ADV IS LESS THAN 7F
	CALL    CARRIED            ;U LIVE IF U HAVE HIM
	BEQ.S   SETMODE2           ;EQ= CARRIED = OK   
	TEST_B  F_DRAG2(A4)
	BMI.S   90$
	CMP.B   #$C0,F_DRAG(A4)    ;ALLOWED? - DONE POINT & SHINE
	BEQ.S   20$
15$
	SCN     77
	BRA     DEAD
 
20$
	SCN     221                ;OK U SNEAK PAST
	ST      F_DRAG2(A4)
	BRA.S   SETMODE2
90$
	TEST_B  F_DRAG3(A4)        ;WEARING RIGHT STUFF?
	BEQ.S   15$                ;DEATH..
	ST      F_DRAG3(A4)
 
SETMODE2
 
	CMP.W   #RNROOM5,(A5)      ;TRYING TO LEAVE 'PAPER WALL' ROOM?
	BNE.S   05$
	CMP.B   #RNLEDGE3,D1       ;GOING TO THE LEDGE? 
	BNE.S   05$                ;NOPE
04$
	CMP.W   #VNCLIMB,VERB(A4)  ;NASTY.... (HEHEHE)
	BNE.S   80$
	MOVE.W  #NROPE,D0          ;GOT THE ROPE?
	GETINFO        
	CALL    CARRIED
	BNE.S   80$                ;OOPPS   DEATH..
	CMP.W   #NHOOK,12(A0)      ;TIED TO HOOK?
	BEQ     NEWROOM2           ;YES, OK YOU GET TO GO THERE
80$
	SCN     144                ;DEATH IN HELL FIRE
	BRA     DEAD 
05$
	CMP.W   #RNLEDGE3,(A5)     ;TRYING TO GET BACK UP?
	BNE.S   06$
	CMP.B   #RNROOM5,D1        ;TO THE PAPER WALL ROOM
	BEQ.S   04$                ;TEST FOR ROPE ETC
06$
	CMP.B   #RNINTOWER,D1      ;TOWER?
	BNE.S   10$
	CMP.W   #RNPLATEAU3,(A5)
	BNE.S   07$
	MOVE.W  #NSNOWMAN,D0
	GETINFO        
	BTST    #7,6(A0)
	BNE.S   10$
	MOVEQ   #38,D1
09$
	BRA     P.SCN
07$
	CMP.W   #RNPLATEAU,(A5)
	BEQ.S   08$
	CMP.W   #RNPLATEAU2,(A5)
	BEQ.S   08$  
	CMP.W   #RNPLATEAU4,(A5) 
	BNE.S   10$
08$
	MOVE.W  #309,D1
	BRA.S   09$                ;P.SCN
10$
	CMP.W   #RNLAB,(A5)
	BNE.S   20$
	CMP.B   #RNPASSAGE,D1
	BNE.S   15$
	CLR.B   F_ALCH2(A4)
	BSET    #6,F_ALCH(A4)
	BNE.S   20$
	SCN     226
	BRA.S   17$
15$
	TEST_B  F_ALCH(A4)
	BMI.S   20$
	SCN     228
17$  
	MOVE.W  #RNPASSAGE,D1
20$
	CMP.W   #RNHELL,(A5)       ;LEAVING HELL?
	BNE.S   SETMODE3
	BSET    #1,F_DEVIL(A4)
 
SETMODE3
 
	TEST_W  RIDING(A4)         ;ON HORSE (MAYBE)
	BNE     50$
  
	CMP.B   #RNINTOWER,D1      ;HMM
	BNE.S   40$
	CMP.W   #RNLANDING,(A5)
	BNE.S   40$
	MOVE.W  #NBOOTS,D0
	GETINFO        
	CMP.B   #4,6(A0)           ;WEARING THE BOOTS?
	BEQ.S   35$                ;OK U LIVE (JUST)
	MOVEQ   #100,D1
32$
	CALL    P.SCN
	BRA     DEAD
35$
	MOVE.W  #NPRINCESS,D0
	GETINFO        
	TEST_L  6(A0)              ;CARRIED?
	BEQ.S   38$
	
	CMP.L   #$01000000+NLANDING,6(A0)   ;related to landing?
	BEQ.S   39$                ;Yes => death

	CMP.L   #RNLANDING,6(A0)   ;SHE HERE?
	BNE.S   40$
39$
	BTST	#1,(A0)			;she cannot follow if dead!
	BNE.S	40$
	SCN     204                ;SHE FOLLOWS DOWN SOPE
38$
	MOVEQ   #101,D1
	BRA.S   32$                ;P.SCN&DEAD
40$
	CMP.B   #RNLANDING,D1
	BNE.S   50$
	CMP.W   #RNINTOWER,(A5)   
	BNE.S   45$                ;Hmm problem with SLOPE being rel to tower
	MOVE.W  #NBOOTS,D0  
	GETINFO        
	CMP.B   #4,6(A0)           ;WORN?
	BEQ.S   50$
	MOVEQ   #99,D1
	BRA     P.SCN

45$
	CMP.W   #RNJAIL,(A5)       ;Only other allowed room
	BEQ.S   50$
	CALL    P.SUB
	MSGGO	CGTW

50$
	CMP.B   #RNWALL,D1         ;LEAVING TOWER VIA WALL?
	BNE.S   60$ 
	CMP.W   #RNJAIL,(A5)
	BNE.S   60$
55$
	MOVE.W  #NWINDOW1,D0
	GETINFO        
	BTST    #4,(A0)            ;WINDOW OPEN?
	BNE.S   56$
	SCN     322
	RET
56$
	TEST_W  NEXITFLG(A4)
	BNE     NEWROOM2  
	SCN     89                 ;NOT ROPED = DEADED
	BRA     DEAD
60$
	CMP.W   #RNWALL,(A5)       ;ROPED STILL?
	BNE.S   SM3.70
	CMP.B   #RNJAIL,D1
	BEQ.S   55$
 
SM3.70
 
	CLR.B   F_PRFOLL(A4)       ;IF WE GET OUT THEN SHE DOESNT FOLLOW
 
	MOVE.W  #NPRINCESS,D0
	GETINFO        
	MOVEQ   #1,D5              ;KILL VALID ERRORS
	CALL    VALID              ;SHE GOING TO FOLLOW?
	TEST_W  D7                 ;YES/NO
	BNE.S   SM3.80
	CALL    NPC.OK             ;DEAD ETC?
	BNE.S   SM3.80             ;YES - WONT FOLLOW

	MOVE.W  #NADVENT,D0        ;Is the adventurer around?
	GETINFO        
	CALL    VALID.NP           ;Is he local?
	TEST_B  D7 
	BNE.S   71$                ;Nope - forget him
	CALL    NPC.OK             ;Is he in any state to interfere?
	BNE.S   71$                ;Nope

	SCN     206                ;Yes, he kills you  'stand back fair maid..'
	BRA     DEAD
*
* SHOULD TEST IF SHE'S TIED TO BED??
*
 
71$
	CMP.B   #RNCAVEMOUTH,D1    ;SHE STOPS U
	BNE.S   SM3.72             ;CANNOT GO IN CAVE!
	MOVE.W  #323,D1
	BRA.S   SM2.P.SCN


*--------------------------------
* 
* SRC.ADV.SETMODE2
*
*--------------------------------
 
* HMM SPLIT AT PRINCESS TRAILING U BIT
 
SM3.72
 
	CMP.B   #RNINHUT,D1        ;NOPE - CANNOT SEE GURU!!!!
	BNE.S   10$
	SCN     324
	MOVE.B  #RNHILLS2,D1
	MOVE.W  #RNHILLS2,8(A0)    ;SET PRINCESS' LOC TOO
	BRA.S   SM3.80
10$
	CMP.B   #RNGATEWAY,D1      ;PRINCESS RUNS OFF TO DADDY?
	BNE.S   20$
	MOVE.W  #NPRINCESS,D0
*      GETINFO                    ;SETOUT COPES
	CALL    SETOUT             ;ZAP & CLEAN UP
	MOVE.W  #205,D1
	BRA.S   SM2.P.SCN
20$
	CMP.B   #RNUPTREE,D1       ;NOPE - CANNOT GO IN TREE!!
	SNE     F_PRFOLL(A4)     ;DID NONE OF ABOVE = SHE FOLLOWS
	BNE.S   SM3.80
	MOVE.W  #325,D1
 
SM2.P.SCN
 
	BRA     P.SCN
  
SM3.80
 
	CMP.W   #RNPASSAGE4,(A5)   ;LEAVING PASSAGE
	BNE.S   03$
	CMP.B   #RNLIFT,D1         ;ENTERING LIFT
	BNE.S   03$
	TEST_B  F_LIFTUP(A4)       ;HAS LIFT ARRIVED YET?
	BNE.S   03$
	SCN     326
	BRA     DEAD
 
03$
 
	CMP.W   #RNLIFT,(A5)       ;LEAVING LIFT?
	BNE.S   20$
	CMP.B   #RNROCKFACE,D1     ;FOR ROCKFACE?
	BNE.S   20$   
	BSET    #0,F_LIFT(A4)      ;HAS HE WALKED OUT OF LIFT BEFORE?
	BNE.S   20$
	SCN     327
	MOVE.W  #NHARDHAT,D0
	GETINFO        
	BTST    #2,6(A0)           ;WORN? - IF NOT DEATH
	BNE.S   15$        
	SCN     328
	BRA     DEAD
15$
	SCN     329
20$
	CMP.W   #RNRIVER,(A5)
	BNE.S   SM4   
	CMP.B   #RNVSHAFT,D1
	BNE.S   10$
	TEST_B  F_VSHAFT(A4)       ;CAN'T CLIMB SHAFT ONCE WATER LET IN
	BEQ.S   05$
	MOVE.W  #330,D1            ;SHAFT COLLAPSED
	BRA.S   50$
05$
	CMP.W   #0,WCARR(A4)       ;CANNOT CARRY STUFF UP SHAFT    
	BEQ.S   SM4
	MOVE.W  #331,D1            ;DROP ALL!!!
	BRA.S   50$
10$
	CMP.B   #RNCAVERN4,D1      ;MUST HAVE LET RIVER IN TO CROSS LAVA
	BNE.S   SM4
	TEST_B  F_VSHAFT(A4)
	BNE.S   SM4
	TEST_W  RIDING(A4)         ;ON THE HORSE?
	BNE.S   60$                ;YES
	MOVE.W  #174,D1            ;LAVA NOT TOO INVITING...
 
50$
	BRA     P.SCN
 
60$
	MOVEQ   #68,D1             ;PONCH DRAGS IN THE LAVA...
	CALL_S  50$                ;P.SCN
	BRA     DEAD
	
SM4
	CMP.B   #RNPLATFORM,D1     ;GOING N FROM WSHOP?
	BNE.S   SM5
	MOVE.W  #NKRONOS,D0
	GETINFO        
	BTST    #1,(A0)            ;KRONOS DEAD YET?
	BNE.S   05$
	SCN     332
	BRA     DEAD
05$
	CMP.W   #7,WCARR(A4)       ;CRITICAL VALUE  - SO U WEIGH SAME AS KRONOS
	BEQ.S   07$
	SCN     333
	MOVEQ   #RNWORKSHOP,D1
	BRA     SM5.10
07$
	SCN    334
    
SM5
 
	CMP.B   #RNCIRCROOM,D1
	BNE.S   SM5.10
	CMP.W   #RNPLATFORM,(A5)
	BNE.S   SM5.10
	SCN     335
	
SM5.10
	
	CMP.B   #RNCHAMBER,D1      ;ENTERING ROOM WI CRYSTALS
	BNE.S   10$
	MOVE.W  #NWHITE,D0
	GETINFO        
	CALL    CARRIED            ;IF IN CLOSED CONT FORGET
	BNE.S   10$                ;NOT GOT IT IN HANDS
	SCN     336
	RET
10$
	CMP.B   #RNVOID,D1         ;GO VOID?
	BNE.S   20$
	MOVE.W  #NBAND,D0
	GETINFO        
	BTST    #7,6(A0)           ;BAND REMOVED?
	BNE.S   20$                ;YES = OK
	MOVE.W  #306,D1
	BRA     SM2.P.SCN
20$
	CMP.W   #RNPLAIN2,(A5)     ;LEAVING JOHN?   
	BNE.S   30$
	TEST_W  F_TRADE(A4)
	BEQ.S   25$                ;DIDNT TRADE
	MOVEQ   #NJOHN,D0          ;JOHN SAYS...
	CALL    P.SAY2
	SCN     58                 ;'NICE TO DO BUSINESS....
	CLR.W   F_TRADE(A4)
	BRA.S   30$
25$
	MOVE.W  D1,-(A7)
	MOVEQ   #2,D1          ;GIVE AN RND BETWEEN 0 AND 1
	CALL    RND.D1         ;GO TO IT!
	MOVE.W  D1,-(SP)	      ; MULU #5,D1 ! (Diff between 2 msgs)
	ASL.W   #2,D1
	ADD.W   (SP)+,D1
	ADD.W   #59,D1         ;BASE MSG #
	CALL    SM2.P.SCN
	CALL    LINE
	MOVE.W  (A7)+,D1
30$

	CMP.W   #RNINHUT,(A5)      ;Leaving hut?
	BNE.S   40$
	CMP.B   #RNINHUT,D1        ;via exit/leave hut
	BNE.S   40$
	CMP.W   #VNEXIT,VERB(A4)
	BEQ.S   35$
	CMP.W   #VNLEAVE,VERB(A4)
	BNE.S   40$
35$
	MOVEQ   #RNHILL,D1          ;let 'leave hut' do a 'go hill'
40$
	BRA     NEWROOM
99$
	RET
 
NEWROOM
  
	TEST_W  NEXITFLG(A4)        ;CAN'T GO TO NEXT ROOM IF TIED TO THIS
	BEQ.S	01$
	MSGGO	ROPED             ;ROOM'S CONTENTS
01$ 
	TEST_B  F_PRFOLL(A4)       ;SHOULD WE MOVE THE PRINCESS TOO?
	BEQ.S   NEWROOM2           ;NOPE
 
	TEST_W  SEATED(A4)         ;U CAN RIDE AWAY FROM HER 
	BNE.S   NEWROOM2
 
	MOVE.W  #NPRINCESS,D0 
	GETINFO        
	CALL    CARRIED            ;IF CARRYING THE PRINCESS THEN
	BEQ.S   10$                ;DONT SAY SHE FOLLOWS
	MOVE.W  #$0100,6(A0)       ;RELATE
	MOVE.W  D1,D2
	CALL    R.NOUN2            ;GET NEW ROOM'S NOUN #
	MOVE.W  D0,8(A0)           ;PUT HER THERE
	SCN     204                ;'THE PRINCESS FOLLOWS U
10$
 
* FALL THROUGH - IE ONLY MOVE PRINCESS IF WE REALY CHANGE ROOM
 
NEWROOM2

    CALL    SP.NEWROOM          ;remaster
    
	MOVE.W  SEATED(A4),D0      ;CAN'T HAVE GOT HERE IF NOT A VEHICLE
	BEQ.S   10$                ;NO VEHICLE
	GETINFO                    ;OK, WE HAVE A VEHICLE SO LETS MOVE IT
	CLR.L   6(A0) 
	MOVE.B  D1,9(A0) 
10$
	MOVE.B  D1,1(A5)           ;NEW ROOM #
	MOVE.W  #1,SCRPNT(A4)      ;LET SCREEN KNOW TO DO DESC & CONTENTS
90$
	RET  
  
  
*--------------------------------


	END
