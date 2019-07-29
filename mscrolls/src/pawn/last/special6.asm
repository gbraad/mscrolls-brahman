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
* SRC.ADV.SPECIALS6
*
*--------------------------------




	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"roomequ.asm"
	include	"macros.asm"
	include	"scoreref.asm"



      XDEF     SP.BREAK,SP.MIDAI,SP.PULL,SP.CAST,SP.FREEZ,SP.LISTE,SP.SLIDE
      XDEF     SP.LIGHT,SP.PRESS,SP.DRINK,SP.TURN,SP.UNLOC,SP.SCORE,SP.RNOUN
      XDEF     SP.KRON,SP.EAT,SP.SMOKE,SP.PUT,SP.PLANT,SP.HAVEW,SP.EMPTY
      XDEF     SP.DROP


      XREF     F_VSHAFT,F_COLD,F_LIFTP,F_ADVCOL,F_AERO,F_KRDEAT,F_DEVIL
      XREF     F_KRONOS,GETRELN,P.IDFY

      XREF     P.SCN,SETOUT,DEAD,R.NOUN,GETDATA,OPEN,CLOSE,GETNOUNS,SETPRON
      XREF     VALID,P.TN,W.CLICK,W.NHAP,W.VANI,ISARE,TIMER4
      XREF     W.NOW,W.UNLOCK,W.IDFY,W.HAPPY,V.TASTE
      XREF     DOSCORE2,CARRIED,CHKPAUSE,YCANT
      XREF     ROOM,VERB,CONJ,LIGHT,CHEAT,CHEAT2,CHEAT.LI,PSUDAT,KM.ROOM
      XREF     KM.TIME,NPCINRM,CLOCK,DBUG.ON,HAVEWON,ADDNOUN,SUBJECT

      XREF     SNOWLOC,DO.SCORE,DO.SCN,PRTTXT,P.DESC



SP.BREAK
 
	CMP.W   #NSEAL,D0          ;BREAK SEAL = OPEN NOTE
	BEQ.S   30$
	CMP.W   #NLWALL,D0
	BEQ.S   20$
	CMP.W   #NBAND,D0
	BEQ.S   10$
	CMP.W   #NBAND,D5
	BEQ.S   10$
	RET
10$
	MOVE.L  (A7)+,D1
	SCN     361            ;CANT MANIPULATE BAND LIKE THAT
	RET
 
20$
	SCN     362            ;FLOODING..
	ST      F_VSHAFT(A4)
	MOVE.W  #RNCORRIDOR,ROOM(A4)
	MOVE.W  #NLRIVER,D0
	CALL    SETOUT  
	MOVE.W  #NLAVA,D0
	CALL    SETOUT  
	MOVE.L  (A7)+,D1
	RET
 
30$
	MOVE.L  (A7)+,D0           ;POP STACK   
	MOVEQ   #0,D5              ;NO ADD NOUN
	MOVE.W  #-1,CONJ(A4)
	MOVE.W  #NNOTE,D0
	GETINFO
	MOVE.W  #VNOPEN,VERB(A4)
	BRA     OPEN
 
SP.MIDAI
	MOVE.W	ROOM(A4),D0 
	CMP.W	#RNMAZE,D0
	BEQ.S	20$
	CMP.W	#RNWALL,D0
	BEQ.S	05$			;on wall, in princess' room, plateu
	CMP.W	#RNJAIL,D0		;below all check
	BEQ.S	05$
	CMP.W	#RNPLATEAU2,D0
	BNE	99$
05$
	MOVE.W  #RNWALL,D2         ;TARGET
	CALL    GETRELN
10$
	MOVE.W  (A1)+,D0
	BEQ.S   20$
	GETINFO
	TEST_W  12(A0)             ;MUST NOT BE TIED
	BNE.S   10$
	TEST_B  6(A0)              ;MUST BE 'LYING AROUND'
	BNE.S   10$
	BTST    #0,5(A0)           ;NOT A PSEUDO
	BNE.S   10$
	MOVE.W  #RNPLATEAU2,8(A0)
	BRA.S   10$
20$   
	MOVE.W  #RNMAZE,D2
	CALL    GETRELN
30$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	CMP.W   #NMAZE,D0
	BEQ.S   30$
	CMP.W   #NHORSE,D0         ;DONT SET HORSE OUT IF RIDING!
	BEQ.S   90$
	CALL    SETOUT  
	BRA.S   30$
90$
	MOVE.W  #NMAZE,D0
	GETINFO
	BCLR    #4,4(A0)           ;ZAP SO WE ALWAYS GET DESC
99$
	RET
  
SP.PULL   
  
	CMP.W   #NLWALL,D0    
	BEQ     SP.BREAK  
	RET    
 
*--------------------------------
 
SP.CAST
 
	CMP.W   #NSPELL,D0
	BNE.S   90$
	CMP.W   #NTOMES,D5
	BNE.S   90$
	MOVE.B  #$11,(A5)          ;SET TO OPEN
	CALL    SETOUT             ;ZAP SPELL
	SCN     363               ;IT WORKS!- BOOK OPENS
	MOVE.L  (A7)+,D0
	MOVE.W  D5,D0              ;SO WE CAN SET THE PRONOUN
	BRA     SETPRON
90$
	RET
 
*--------------------------------
 
SP.FREEZ
 
	MOVE.W  ROOM(A4),D1
	LEA     FREEZE.TBL(PC),A1
10$
	MOVE.W  (A1)+,D2
	BEQ     85$                ;ZAP STATE IF NOT IN COLD ROOM
	CMP.W   D1,D2
	BNE.S   10$
	
	MOVE.W  #$0400,D2          ;JUST WORN WORD COS OF GETNOUNS
	MOVEQ   #-1,D5             ;HAS TO BE WORDWISE
	MOVEQ   #6,D1              ;BYTE WITH INFO = 6 (LOC INFO)
	CALL    GETNOUNS           ;INTO A1
	MOVEQ   #0,D1              ;FLAG - IF REMANS ZERO = NOT WELL CLOTHED
20$
	MOVE.W  (A1)+,D0
	BEQ.S   30$
	GETINFO
	OR.B    10(A0),D1          ;ACCUMULATE WHATS COVERED  
	BRA.S   20$
30$
	AND.B   #$C8,D1            ;FEET,LEGS,GROIN,CHEST MUST BE COVERED
	CMP.B   #$C8,D1
	BNE.S   40$                ;WARN ETC..
85$
	CLR.W   F_COLD(A4)
90$
	RET   
40$  
	ADDQ.W  #1,F_COLD(A4)   ;NEXT MSG UP..
	MOVE.W  F_COLD(A4),D0
	MOVE.W  #364,D1
	CMP.W   #4,D0
	BNE.S   50$
	CALL_S  55$             ;P.SCN
	CLR.W   F_COLD(A4)      ;COS THEY MAY GET TO DIE LOTS
	BRA     DEAD
50$
	ADD.W   D0,D1           ;NTH  STAGE   
55$
	BRA     P.SCN
  
 
FREEZE.TBL
 
	DC.W     RNPLATEAU
	DC.W     RNPLATEAU2
	DC.W     RNPLATEAU3
	DC.W     RNPLATEAU4
	DC.W     RNWALL
	DC.W     0
 
 
*--------------------------------
 
SP.LIGHT 
 
	CALL    R.NOUN             ;ROOM NOUN# = D0
	GETINFO            ;ROOM INFO = A0
	BTST    #2,5(A0)
	SNE     LIGHT(A4)
	BNE.S   90$
 
	MOVE.W  #NWHITE,D0
	GETINFO
	MOVEQ   #1,D5              ;KILL ERRORS
	CALL    VALID
	TEST_B  D7
	SEQ     LIGHT(A4)
	BEQ.S   90$
	MOVE.W  #NLAMP,D0
	GETINFO
	BTST    #7,(A0)          ;MUST BE LIT!!
	BEQ.S   90$              ;NO GOOD    
	CALL    VALID
	TEST_B  D7
	SEQ     LIGHT(A4)
 
90$
	IFNE     YCHEAT
	MOVE.W  CHEAT(A4),D0       ;VARIOUS CHEAT WAYS TO GET LIGHT
	OR.W    CHEAT2(A4),D0
	OR.W    CHEAT.LI(A4),D0
	OR.B    D0,LIGHT(A4)
	
	ENDC
 
	RET
  
*--------------------------------
  
SP.LISTE
 
	LEA     60$(PC),A1      ;LIST OF LISTENABLE (!?) ITEMS
10$
	CMP.W   (A1)+,D0
	BEQ.S   20$
	TEST_W  (A1)+
	BNE.S   10$
	RET
20$
	MOVE.L  (A7)+,D1         ;POP STACK
	MOVE.W  (A1)+,D1
	BPL     P.SCN
	AND.W   #$7FFF,D1
	MOVE.W  D1,D0
	BRA     P.DESC
 
60$
	DC.W    NBAND,67
	DC.W    NSHADOWS,318
	DC.W    NGRAVEL,374            ;STONES!
*      DC.W    NCLIFF2,375
*      DC.W    NNCLIFF,375            ;HMMMMM...
	DC.W    NROCKS,376
	DC.W    NLEWIS,282
	DC.W    NPIANO,282
	DC.W    NSCREAMS,NSCREAMS+$8000
*      DC.W    NSCLIFF,375            ;HMMMM...
	DC.W    NWALL,378
	DC.W    NWALL2,378
	DC.W    NPWALL,378
	DC.W    NSWALL,378
	DC.W    NLWALL,378
	DC.W    NNOTE,293
*      DC.W    NKEYS,???
	DC.W    NBRIDGE5,383           ;ROPE BRIDGE/CREAKS
	DC.L    0
 
 
 
SP.SLIDE
 
	CMP.W   #NLIFTDOOR,D0
	BNE.S   90$
	MOVE.L  (A7)+,D2              ;POP
	BTST    #4,D1                 ;OPEN?
	BNE     CLOSE               
	BRA     OPEN 
90$
	RET
 
*--------------------------------
 
SP.PRESS
 
	MOVE.L  (A7)+,A1               ;POP
	CMP.W   #NLIFTBUTTON,D0        ;REQUEST LIFT
	BEQ.S   10$
	CMP.W   #NBUTTON1,D0           ;GO UP
	BEQ.S   20$
	CMP.W   #NBUTTON2,D0
	BEQ     30$
	CMP.W   #NAEROBUTTON,D0
	BEQ     SP.PRESS40
	MOVE.L  A1,-(A7)               ;PUT STCK BACK
	RET 
 
10$
	MSG     CLICK
	TEST_B  F_LIFTP(A4)
	BEQ.S	15$
	MSGGO	NHAP
15$
	ST      F_LIFTP(A4)
	SCN     351                   ;IN THE DISTANCE YOU HEAR A FAINT
*                                    ; WHIRRING OF MACHINARY
	MOVE.B  #5,TIMER4(A4)
	RET
 
20$
 
	MSG     CLICK
	TEST_B  F_LIFTP(A4)
	BNE.S	35$			;W.NHAP
	MOVE.W  #NLIFTDOOR,D0
	GETINFO
	BTST    #4,(A0)
	BNE.S	35$			;W.NHAP
	ST      F_LIFTP(A4)
	SCN     352   	                ;WITH A GREAT DEAL OF GROANING AND
*                                      GRINDING THE LIFT ASCENDS
	LEA     PSUDAT(A4),A1
	MOVE.W  8(A0),D1
	MOVE.B  #RNPASSAGE4,0(A1,D1.W) ;MOVE 'SECOND' LIFT DOOR 
	RET
30$
	MSG     CLICK
	TEST_B  F_LIFTP(A4)
	BEQ.S   35$			;W.NHAP
	MOVE.W  #NLIFTDOOR,D0 
	GETINFO  
	BTST    #4,(A0)   
	BNE.S   35$			;W.NHAP
	CLR.B   F_LIFTP(A4)
	SCN     353              ;SLOWLY THE LIFT DESCENDS, MAKING EXT....
*                                MECHANICAL NOISES
	LEA     PSUDAT(A4),A1  
	MOVE.W  8(A0),D1  
	MOVE.B  #RNROCKFACE,0(A1,D1.W) ;MOVE 'SECOND' LIFT DOOR 
	RET   
35$
	MSGGO	NHAP
 
SP.PRESS40
 
	TEST_B  F_ADVCOL(A4)     ;ADV BEEN DEAD TOO LONG?
	BNE.S   10$              ;YES
  
	MOVE.W  #NADVENT,D0  
	GETINFO
	BTST    #1,(A0)          ;DEAD?
	BEQ     10$
	MOVEQ   #1,D5            ;SUPPRESS ERRORS
	CALL    VALID            ;LOCAL? - NOT NICE IN CHEAT MODE!
	TEST_B  D7
	BNE     10$              ;OK FORGET HIM... NOT CLOSE BY
	CMP.B   #1,F_AERO(A4)    ;UNUSED?
	BNE.S   05$              ;NOPE..
	SCN     354              ;REALLY NASTY MSG ABOUT ADV'S SOUL
*                                BEING NICKED BY AEROSOUL
	BRA.S   07$
05$
	CMP.B   #2,F_AERO(A4)    ;SLIGHTLY USED?
	BNE     40$              ;NO - VERY USED - NOTHING HAPPENS
	SCN     355              ;NOT QUITE AS NASTY MSG ABOUT ADV'S SOUL
07$
	MOVE.B  #$81,F_AERO(A4)  ;USED & REFRESHED
	RET
 
10$
 
	MOVE.W  #NKRONOS,D0
	GETINFO
	
	BTST    #1,(A0)           ;DEAD?
	BEQ     40$               ;DEFAULT MSGS. (IF NOT DEAD)
	MOVEQ   #-1,D5            ;SUPRESS ERRORS IN VALID
	CALL    VALID
	TEST_B  D7
	BNE     40$               ;KRONOS NOT LOCAL - CANNOT GO
 
	MOVE.B  F_AERO(A4),D1
	AND.B   #$7F,D1           ;LOSE 'REFRESHED' BIT
	CMP.B   #2,D1             ;BEEN OVERUSED?
	BHI     30$               ;YES...
 
	CMP.B   #1,F_KRDEAT(A4)   ;IS KRONOS STILL DISOLVING?
	BEQ.S   20$               ;YES - U GOT IT!!
 
 
	MOVE.W  #356,D1           ;UNFORTUNATELY U WERE TOO SLOW!"
	BRA.S   SP6.P.SCN
  
20$
 
	MOVE.W  #357,D1          ;OK U GOT IT!!! - ANOTHER BLOOD-"
*                                CURDLING MSG GOES HERE!!"
	ST      F_KRDEAT(A4)
	ST      F_AERO(A4)
	BSET    #2,F_DEVIL(A4)
	BRA.S   SP6.P.SCN
 
30$
	MOVE.W  #358,D1          ;THE AEROSOUL MAKES A STRAINED SUCKING NOISE,
*                                BUT NOTHING ELSE HAPPENS"
	BRA.S   SP6.P.SCN
 
40$
	MOVE.B  F_AERO(A4),D1
	AND.B   #$7F,D1
	CMP.B   #2,D1            ;HOW USED?
	BHI.S   30$              ;TOO MUCH!
    TEST_B  REMASTER(A4)	; do not wear off aerosoul in remaster
	BNE.S	45$
	ADDQ.B  #1,F_AERO(A4)
	SUBQ.B  #1,D1
45$	
	BNE.S   50$             ;BNE= WAS 2, NOW 1 SO GIVE SECOND MSG
  
*                              ;BEQ= WAS 1, NOW 0 SO GIVE FIRST ONE
	MOVE.W  #359,D1         ;THE AEROSOUL SUCKS VIGOUROUSLY
	BRA.S   SP6.P.SCN
 
50$
	MOVE.W  #360,D1         ;THE AEROSOUL SUCKS GENTLY
 
SP6.P.SCN
 
	BRA     P.SCN
 
 
*--------------------------------
 
 
SP.DRINK
 
	CMP.W   #NALE,D0
	BEQ.S   10$
	CMP.W   #NWHISKY,D0
	BNE.S   90$
10$
	SCN     288             ;"GETTING DRUNK IS NO WAY...!"
	MOVE.L  (A7)+,A1       ;POP
90$
	RET

*--------------------------------
*
*  SRC.ADV.SPECIALS7
*
*--------------------------------
 
SP.TURN
 
	RET
 

        XREF    REMASTER
        
SP.UNLOC
 
	CMP.W   #NDOOR2,D0         ;EVEN BLUE KEY WONT UNLOCK
	BEQ.S   90$                ;ADV ROOM DOOR
	CMP.W   #NKEY,D5           ;WITH BLUE KEY??
	BNE.S   90$                ;NO - FORGET IT!
        
    TEST_B  REMASTER(A4)
    BEQ.S   10$

    ;; remaster version
	SCN     395             ; the key worked!
    BRA.S   20$
10$

	EXG     D0,D5              ;SAVE D0
	CALL    SETOUT             ;KILL BLUE KEY (USE ONLY ONCE)
	CALL    P.TN            ;'THE BLUE KEY'
	MSG     VANI
	EXG     D0,D5              ;GET BACK D0 (A0 ETC PRESERVED)
20$        
	BCLR    #6,(A0)            ;BUT UNLOCKS *ANY* DOOR (ALMOST)
	BSET    #5,(A0)            ;SET CLOSED
	MOVEQ   #0,D6              ;FLAG FOR OPEN = SUCCESSFUL
	MOVE.L  (A7)+,A1           ;POP
	CMP.W   #VNUNLOCK,VERB(A4)
	BNE.S   90$   
	CALL    P.TN
	CALL    ISARE
	MSG     NOW
	MSG     UNLOCK
90$
	RET
 
 
 
 
 
*--------------------------------
*
* SP.SCORE - ADD TO SCORE  IF
*               WE GET TO A SECIAL
*            PLACE  - IE PAST ROCKS
*
*--------------------------------
 
 
SP.SCORE
	
	LEA     RoomList(PC),A1
	MOVE.W  ROOM(A4),D1
 
10$
	MOVE.L  (A1)+,D0
	BEQ.S   20$
	CMP.W   D1,D0           ;LOW WORD IS ROOM
	BNE.S   10$
	SWAP    D0              ;HI WORD IS SCORE  ADDR
	EXT.L	D0		;mod to make alink compatible!
	LEA	-4(A1,D0.L),A2
	CALL    DOSCORE2       ;OK IF IN ONE ROOM CAN'T BE ANYWHERE ELSE
 
20$
	LEA     NounList(PC),A1
30$
	MOVE.L  (A1)+,D0
	BEQ.S   40$
*	CALL    CARRIED
	MOVE.L	D0,D2		;save D0 (particularly the hi word)
12305$ 
	GETINFO			;this is a hack of TOPLOC (see throw.asm)
	MOVE.B  6(A0),D1
	AND.B   #$BF,D1                  ;ALLOW ROOMS
	BEQ.S   12310$
	AND.B	#$C2,D1		;eliminate rooms/out/hidden	
	BNE.S	30$
	MOVE.W  8(A0),D0
	BNE.S	12305$
12310$
	MOVE.L	D2,D0		;recover noun no.
	TEST_W	8(A0)

	BNE.S   30$              ;NOPE
	SWAP    D0
	EXT.L	D0		;mod to make alink compatible!
	LEA	-4(A1,D0.L),A2
	CALL    DOSCORE2
	BRA.S   30$              ;NEXT
40$
	RET
 
 
NounList

	DC.W    SC.GTROP,NROPE
	DC.W    SC.GTCOI,NCOIN
	DC.W    SC.GBKEY,NKEY
	DC.W    SC.GTCOL,NPOUCH
	DC.W    SC.GBAL,NBALPAPER
	DC.L    0
	
 
RoomList

	DC.W    SC.GOSHE,RNINSHED    ;OPEN DOOR WI KEY SCORES!!
	DC.W    SC.GOTRE,RNSTAIRCASE ;THIS ONE DOES DESERVE IT!!
	DC.W    SC.DNROP,RNLEDGE3    ;CLIMBING DOWN ROPE
	DC.W    SC.PASSR,RNTRACK     ;'CLIMB ROCKS'
	DC.W    SC.PDRG1,RNWORKSHOP  ;PASS DRAGON TO GET TO WORKSHOP
	DC.W    SC.GOMIN,RNROCKFACE  ;GET OUT OF LIFT ALIVE
	
	DC.L    0
 
 

 
SP.RNOUN
  
	EXG     D0,D1
	CALL    R.NOUN
	EXG     D0,D1
	RET
 
 
 
SP.KRON
  
	MOVE.W  #NKRONOS,D0
	CALL    CHKPAUSE
	BNE     90$                 ;KRONOS IS PAUSED
 
	MOVE.W  KM.ROOM(A4),D1     ;GET ROOM # IF HAPPENED BEFORE
	BEQ.S   05$                ;NOT HAPPEND = DONT CARE
	CMP.W   ROOM(A4),D1        ;CANNOT CHANGE ROOM!
	BNE     90$                ;WAIT THEN!
05$
	MOVE.W  ROOM(A4),D1
	MOVE.W  D1,KM.ROOM(A4)
	TEST_W  F_KRONOS(A4)
	BNE.S   20$
 
	MOVE.W  CLOCK(A4),KM.TIME(A4) ;WHEN IT STARTED
	MOVE.W  #NKRONOS,D0
	GETINFO
	CALL    SETPRON
	MOVE.W  D0,NPCINRM(A4)
	CALL    R.NOUN
	MOVE.W  #$100,6(A0)         ;RELATE TO ROOM
	MOVE.W  D0,8(A0)
*      MOVEQ   #NPLATFORM,D0       ;REMMED OUT TO AVOID DISPLACING 
*      GETINFO          	   ;THE ROOM!!!
*      MOVE.W  D1,8(A0) 
	
	OR.W    #1,F_KRONOS(A4)
	MOVEQ   #28,D1
01$
	BRA     P.SCN    	     ;KRONOS ARRIVES
 
20$
	MOVE.W  KM.TIME(A4),D0     ;FIND WHEN FIRST MEET STARTED
	ADD.W   #20,D0             ;ADD MAX TIME FOR IT
	CMP.W   CLOCK(A4),D0       ;COMPARE WI CURRENT TIME
	BMI     KM1.EXIT                ;TOO LONG!
	CMP.W   #1,F_KRONOS(A4)
	BHI.S   30$ 
	OR.W    #2,F_KRONOS(A4)
	MOVE.W  #NNOTE,D0  
	GETINFO   
	CALL    SETPRON
	MOVE.W  #$100,6(A0)        ;RELATE
	MOVE.W  D1,8(A0)  
	MOVEQ   #29,D1   
	BRA.S   01$ 
30$
	CMP.W   #3,F_KRONOS(A4)
	BHI.S   40$
	OR.W    #4,F_KRONOS(A4)
	MOVE.W  #NNOTE,D0
	GETINFO
	MOVE.W  SUBJECT(A4),D0      ;GET PERSONA ID
	CMP.W   8(A0),D0            ;GOT THE NOTE?
	BNE.S   35$                 ;NOPE - KRONOS GETS ANGRY!
	SCORE   GTNOT               ;POINTS
	OR.W    #8,F_KRONOS(A4)
90$
	RET
35$
	MOVE.W  #190,D1             ;KRONOS GETS UPSET
	BRA.S   01$
40$
	CMP.W   #7,F_KRONOS(A4)
	BHI.S   50$
	MOVE.W  #NNOTE,D0
	GETINFO
	MOVE.W  SUBJECT(A4),D0      ;HAS THE NOTE BEEN TAKEN ?
	CMP.W   8(A0),D0
	BNE.S   45$
	SCORE    GTNOT            ;CAN ALSO SCORE  THIS WAY
	BRA.S   50$
45$
	SCN      189                 ;KRONOS  GIVES UP
	MOVE.W  #NNOTE,D0  
	CALL    SETOUT             ;ZAP FROM GAME & CEAN UP
50$
	CMP.W   #15,F_KRONOS(A4)
	BHI.S   90$
	SCN     187                ;KRONOS LEAVES
KM1.EXIT
	MOVE.W  #-1,F_KRONOS(A4)
	MOVE.W  #NKRONOS,D0
	GETINFO
	MOVE.L  #RNWORKSHOP,6(A0)   ;PUT KRONOS IN WORKSHOP!
	RET
 
 
SP.EAT
 
	CMP.W   #NRABBIT,D5
	BNE.S   10$
	ADDQ.L  #4,(A7)            ;MISS OUT EDTS.OK
	RET
10$
	CMP.W   #NSNOW,D0          ;EAT SNOW?
	BNE.S   20$                ;NOPE
	CALL    V.TASTE            ;PRINT HOW SNOW TASTES
	MOVE.L  #SNOWLOC,6(A0)     ;SET LOC INFO,POINTER TO PSUDAT
	BSET    #0,5(A0)           ;BACK TO PLATEAU
	MOVE.L  (A7)+,A2           ;POP STACK 
* FALL OUT
20$
	IFNE    YCHEAT
	CMP.W   #NHEAD,D0          ;QUIETLY EAT HEAD WITH HANDS
	BNE.S   90$
	CMP.W   #NHAND,D5
	BNE.S   90$
	NOT.W   DBUG.ON(A4)
	MOVE.L  (A7)+,D0
90$
	ENDC
	RET
 
 
 
SP.SMOKE
 
	CMP.W   #NPLANT,D0
	BNE.S   90$
	CALL    SETOUT             ;KILL IT FROM GAME
	SCN     280                ;'HEAVY...
	MOVE.L  (A7)+,D0           ;POP
90$
	RET
 
 
 
*--------------------------------
*
* SP.PUT - CALLED AT THE *END* 
*          OF PUT - ONCE ACTION
*          HAS HAPPENED *MUST*
*          RETURN TO VERB ROUTINE
*
*--------------------------------
 
SP.PUT
 
	CMP.W   #NSBALBX,D5        ;PUT IN A BALLOT BOX?
	BEQ.S   10$
	CMP.W   #NLBALBX,D5
	BNE.S   90$
10$
	MOVE.B  #2,10(A5)          ;RE ZAP CONTAINER VOLUME
	CMP.W   #NBALPAPER,D0      ;VOTING?
	BNE.S   90$
	SCORE   VOTE               ;POINTS FOR VOTING
	CMP.W   #NLBALBX,D5        ;BUT DID U VOTE FOR GRINGO
	BNE.S   90$                ;NO - SHAME ON U!
	SCORE   VTGRI
90$
	RET
 
*--------------------------------
 
SP.PLANT
 
	MOVE.L  (A7)+,A2           ;SO WE DONT HAVE TO POP TO MUCH
	CMP.W   #NPLANT,D0         ;PLANT PLANT
	BNE.S   90$
	CMP.W   #NPOT,D5           ;IN POT
	BNE.S   90$
	CMP.W   #5,CONJ(A4)        ;'IN'
	BNE     P.IDFY
	TEST_W  CONJ+2(A4)         ;'WITH'
	BNE     P.IDFY
	CMP.W   #NTROWEL,ADDNOUN+2(A4)
	BNE.S   90$
	CALL    P.TN
	MSG     HAPPY            ;SEEM@ MUCH HAPPIER NOW
	SCORE   PLANT
	CALL    SETOUT             ;CLEAN UP TIED THINGS
	MOVE.L  #$11000000+NPOT,6(A0) ; PLANT TO POT (INSEP REL)
	RET
90$
	MOVE.L  A2,-(A7)           ;COULD BE JMP  (A2) I GUESS...
	RET                        ;HMM....
 
 
SP.HAVEW
 
	CMP.W   #RNROOM,ROOM(A4)   ;GOT INTO ADV. WRITERS ROOM?
	BNE.S   90$                ;NOPE
	 
	IFNE     YCHEAT             ;FORGET IF IN DB MODE REALLY
	TEST_W  CHEAT(A4)
	BNE.S   90$
	ENDC
 
	ST      HAVEWON(A4)        ;OK - FLAG U GOT THERE
	MOVE.W  #NLISTING,D0       ;DEBUGBITS
	GETINFO
	CLR.L   6(A0)              ;PUT IT ON INV
90$
	RET
 
*--------------------------------
 
SP.EMPTY
 
	CMP.W   #NLBALBX,D0       ;EMPTY BALLOT BOX A?
	BEQ.S   10$
	CMP.W   #NSBALBX,D0
	BNE.S   90$
10$
	MOVE.L  (A7)+,D7           ;POP STACK
	BRA     YCANT
90$
	RET
*--------------------------------
	        
SP.DROP 
*                        -EDS HUGH
	
	CMP.W   #NBOTTLE1,D0			;POTION BOTTLE?
	BNE.S   90$				;TOO BAD
	CALL    SETOUT				;CANT HAVE IT LYING ABOUT
90$
	RET
	
*--------------------------------

      END
