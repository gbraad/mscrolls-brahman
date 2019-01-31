******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

		 include	"equates.i"
		 include	"macros.i"
		 include	"verbequ.i"
		 include	"scenequ.i"

* Jinxter!

*--------------------------------
*
* SRC.ADV.NPC.CHARTS.CODE
*
*--------------------------------

	IFND	wantNOQUOTES


      XREF     NPCQUEST,NPCMODEL,VERB,P.MSG,LASTNPC,PRTTXT,P.SAY

      XDEF     CHARTSET,GETCMSG,SAY.HELL

*CHART DECODERS
       
       
CHARTSET
       
*DETERMINE WHETHER SPECIAL ENTRY
*FORVERB TYPE.
       
*ENTRY:
*D0=NPC.QUEST
       
*EDIT:
*D0=MESG# (IF ANY)
*D7=ERROR CONDITION
       
       
       MOVEQ   #0,D7                   ;NO ERRROR TO START
       MOVE.W  VERB(A4),D1
       LEA     WHERE.SET(PC),A0        ;1ST TABLE
       TEST_W  D0 
       BEQ.S   50$                     ;DONE ALREADY
10$
       TEST_W  (A0)+
       BNE.S   10$                     ;UNTIL ENF
       SUBQ.W  #1,D0
       BNE.S   10$                     ;UNTIL CORRECT QUESTREF.
50$
       MOVE.L  (A0)+,D0                ;GET MSG+(SET OR VERB)
       SWAP    D0                      ;(SET OR VERB) LOW
       TEST_W  D0
       BMI.S   20$                     ;IS A SET
       BEQ.S   90$                     ;END, THEN FAIL
       CMP.W   D0,D1                   ;SAME?
       BNE.S   50$                     ;NO, SCAN THROU..
       SWAP    D0                      ;MESG# LOW
       BRA.S   99$                     ;EXIT
20$
       CALL    ISASET                  ;ENTRY?
       TEST_W  D7                      ;NONE?
       BEQ.S   99$                     ;NO, EXIT
       MOVEQ   #0,D7                   ;RESET ERROR STAT
       BRA.S   50$                     ;TRY AGAIN
       
90$
       MOVEQ   #1,D7
99$
       RET

*--------------------------------

ISASET
       
*GET MESG# FROM SET (IF ANY)

       MOVE.L  A0,-(A7)                ;SAVE
       BCLR    #15,D0                  ;ZAP HI BIT
       LEA     SETS(PC),A0             ;SET INFO
23$
       SUBQ.W  #1,D0                   ;DEC SETNO.
       BEQ.S   21$                     ;THERE YET?
22$
       TEST_B  (A0)+
       BNE.S   22$                     ;UNTIL END OF LIST
       BRA.S   23$                     ;UNTIL CORRECT SET LIST
21$
       SWAP    D0                      ;MESG# LOW
24$
       MOVE.B  (A0)+,D2                ;GETVERB ENTRY
       BEQ.S   90$                     ;END, SO FAIL
       CMP.B   D2,D1                   ;SAME?
       BNE.S   24$                     ;UNTIL END
99$
       MOVE.L  (A7)+,A0
       RET                             ;EXIT
90$
       MOVEQ   #1,D7                   ;ERROR
       BRA.S   99$                     ;EXIT

*--------------------------------
       
       
GETCMSG
       
       
*GET CHART MESSAGE NUMBER
       
*ENTRY:
*D2=PERSON
       
*EXIT:
*D0=CHART MESG#
       
       MOVE.W  NPCQUEST(A4),D0     ;QUESTION NUMBER
       BMI     TYPE3
       MOVE.W  NPCMODEL(A4),D1
       
       BMI.S   10$                 ;TYPE I
       LEA     WHERE.TP4(PC),A0    ;START
       MOVE.W  D0,-(SP) 
       ASL.W   #2,D0               ;*6 (FOR FUN)
       ADD.W   (SP),D0
       ADD.W   (SP)+,D0
       ADD.W   D2,D0               ;ADD PERSON
       CMP.W   #SHALL,D1
       BEQ.S   20$                 ;YES
       CMP.W   #WILL,D1            ;FUTURE TENSE..
       BNE.S   30$                 ;NOT
20$
       ADD.W   #3,D0               ;ADD 3 IF FUTURE.
30$
       CALL    PT2
40$
       MOVE.W  (A0)+,D0            ;GET THAT MESSAGE
       BRA.S   99$                 ;EXIT.
10$
       
       LEA     WHERE.TP1(PC),A0    ;OTHER TABLE
       MOVE.W  D0,-(SP)
       ADD.W   (SP),D0
       ADD.W   (SP)+,D0
       ADD.W   D2,D0               ;ADD PERSON
       CALL    PT2                 ;GET ADR
       MOVE.W  (A0)+,D0
99$
       RET

*--------------------------------
       
TYPE3
       LEA     CAN.TP3(PC),A0      ;TABLE
       MOVE.W  NPCMODEL(A4),D0
       BMI.S   10$                 ;NO GOOD
       BEQ.S   20$                 ;MUST.
       SUBQ.W  #1,D0               ;ADJ
20$
       MOVE.W  D0,-(SP)
       ADD.W   (SP),D0
       ADD.W   (SP)+,D0
       ADD.W   D2,D0               ;PLUS PERSON
       CALL    PT2                 ;GETMSG#
       MOVE.W  (A0)+,D0            ;GETIT
       RET
10$
       PRINT   <'MU'>              ;THIS SHOULD NEVER HAPPEN
       RET

*--------------------------------
       
PT2
       TEST_W  D0                  ;ZERO???
       BEQ.S   40$                 ;HUM, ZERO ALREADY..
31$
       TEST_W  (A0)+
       BMI.S   31$                 ;REPEATON NEG.
       SUBQ.W  #1,D0               ;ZERO YET?
       BRA.S   PT2 
40$
       RET

*--------------------------------

SAY.HELL
               
*GREETING MESSAGE FOR NPC
       
*ENTRY:
* D0=NPC#
       
       DO	P.SAY
       LEA     GREETCHART(PC),A0

       MOVE.W  D0,-(SP)
       ADD.W   (SP),D0
       ADD.W   (SP)+,D0

       CALL    PT2                ;FIND THE MSG POSN
       MOVE.W  (A0),D0            ;GET IT
       BNE.S   10$                ;NOT 1ST MEETING
       MOVE.W  #1,(A0)+
98$
       MOVE.W  (A0)+,D1           ;GET NUM
99$
       MOVE.W  D1,D0
       BEQ.S   20$                ;NULL
       BCLR    #15,D1
       DO	P.MSG
       TEST_W  D0
       BMI.S   98$                ;ANOTHER
       RET                        ;EXIT
       
10$
       CMP.W   LASTNPC(A4),D0     ;SAME CONVERSATION
       BEQ.S   20$                ;YEP
       MOVE.W  D0,LASTNPC(A4)     ;UPDATE CONVERSATION
       ADDQ.L  #4,A0              ;SKIP
       MOVE.W  (A0)+,D1
       BRA.S   99$                ;EXIT
20$
       PRINT   <'Good day'>
       RET


*--------------------------------
*
* SRC.ADV.NPC.CHARTS
*
*--------------------------------
   
 
SETA   EQU     1+HI
SETV   EQU     2+HI
WILL   EQU     3
SHALL  EQU     4
       
	BeSure	M.LASTMSG
	BeSure	M.IDNK
	BeSure	M.NOIDEA
	BeSure	M.DBSURE

* REPLAY CHARTS FOR QUESTION FORMAYS
       
       
WHERE.TP4
        DC.W    M.IDNK,M.IDNK,M.LASTMSG+1
        DC.W    M.NOIDEA,M.LASTMSG+5+HI,M.LASTMSG+6,M.LASTMSG+7
       
WHO.TP4
        DC.W    M.LASTMSG+8,M.LASTMSG+9,M.LASTMSG+8
        DC.W    M.IDNK,M.IDNK,M.LASTMSG+7
       
WHAT.TP4
        DC.W    M.LASTMSG+13,M.LASTMSG+9,M.IDNK+HI,M.LASTMSG+14
        DC.W    M.LASTMSG+13,M.LASTMSG+9,M.IDNK+HI,M.LASTMSG+14
       
WHEN.TP4
        DC.W    M.LASTMSG+16,M.LASTMSG+17,M.IDNK+HI,M.LASTMSG+14
        DC.W    M.DBSURE,M.LASTMSG+17,M.IDNK+HI,M.LASTMSG+14
       
WHY.TP4
        DC.W    M.LASTMSG+4,M.LASTMSG+9,M.IDNK+HI,M.LASTMSG+14
        DC.W    M.DBSURE,M.LASTMSG+17,M.IDNK+HI,M.LASTMSG+14
       
       
*--------------------------------
	BeSure	M.WYLTK
	BeSure	M.GQUEST
	BeSure	M.IAM
       
       
WHERE.TP1
        DC.W    M.LASTMSG+21,M.LASTMSG+22,M.NOIDEA
WHO.TP1
        DC.W    M.WYLTK,0,0
WHAT.TP1
        DC.W    M.GQUEST,M.LASTMSG+15,0
WHEN.TP1
        DC.W    M.DBSURE,M.LASTMSG+17,M.IDNK+HI,M.LASTMSG+14
WHY.TP1
        DC.W    M.LASTMSG+20,M.LASTMSG+5+HI,M.IAM,M.IDNK+HI,M.LASTMSG+14
       
       
*--------------------------------
	BeSure	M.IYAUPTI
	BeSure	M.ICDOANY
	BeSure	M.IMPOLITE
       
CAN.TP3
        DC.W    M.IYAUPTI,M.ICDOANY,M.IDNK+HI,M.LASTMSG+14
MAY.TP3
        DC.W    M.LASTMSG+23,M.LASTMSG+24,M.IMPOLITE
WIL.TP3
        DC.W    M.IDNK+HI,M.LASTMSG+25,M.LASTMSG+7,M.LASTMSG+7
SHAL.TP3
        DC.W    M.LASTMSG+26,M.IDNK,M.IDNK
       
*--------------------------------

	BeSure	M.HELLO
	BeSure	M.NT
	BeSure	M.OUTWEIRD
	BeSure	SCNBASE       
       
GREETCHART
       
        DC.W    0,M.LASTMSG+27,SCNBASE+85                   ;GUARDS
        DC.W    0,M.HELLO,SCNBASE+62                        ;JOHN
        DC.W    0,M.NT,M.NT                                 ;DONKEY
        DC.W    0,M.LASTMSG+29,M.LASTMSG+30                 ;ADVENT
        DC.W    0,SCNBASE+159,SCNBASE+185                   ;HORSE
        DC.W    0,M.LASTMSG+31,M.LASTMSG+30+HI,M.LASTMSG+32 ;GURU
        DC.W    0,M.NT,M.NT                                 ;BATS
        DC.W    0,M.LASTMSG+33,M.LASTMSG+34                 ;DEVIL
        DC.W    0,M.NT,M.NT                                 ;DEMONS
        DC.W    0,SCNBASE+282,M.LASTMSG+30                  ;LEWIS
        DC.W    0,M.NT,M.NT                                 ;DRAGON
        DC.W    0,M.LASTMSG+35

        DC.L    M.LASTMSG+35                                ;RABBIT
        DC.W    0,M.HELLO,M.LASTMSG+30                      ;PORTER
        DC.W    0,M.NT,M.NT                                 ;SNOWMAN
        DC.W    0,M.HELLO,M.LASTMSG+30                      ;PRINCESS
        DC.W    0,M.NT,M.NT                                 ;HOBBITS
        DC.W    0,M.HELLO,M.LASTMSG+30                      ;KRONOS
        DC.W    0,M.HELLO,M.LASTMSG+30                      ;ALCHEMISTS
        DC.W    0,M.OUTWEIRD,SCNBASE+280                    ;ERIK
        DC.W    0,M.HELLO,M.LASTMSG+30                      ;VOICE
       
       
*SET INFO..

	BeSure	M.IDBELYU
       
WHERE.SET

        DC.W    VNEAT,M.LASTMSG+2,SETA,M.LASTMSG+3
        DC.W    VNFIND,M.NOIDEA,SETV,M.LASTMSG+3,0
       
WHO.SET
        DC.W    VNEAT,M.LASTMSG+10,SETA,M.LASTMSG+11,SETV,M.LASTMSG+12,0
       
WHAT.SET
        DC.W    SETA,M.LASTMSG+15,SETV,M.LASTMSG+15,0

WHEN.SET
        DC.W    SETV,M.LASTMSG+18,0
       
WHY.SET
        DC.W    VNSMELL,M.LASTMSG+20,VNFEEL,M.LASTMSG+20
        DC.W    SETA,M.LASTMSG+11,SETV,M.IDBELYU,0
       
       
SETS
	 SVERB	PUNCH
	 SVERB	ATTACK
	 SVERB	SMASH
	 SVERB	SHOOT
	 SVERB	STRANGLE
	 SVERB	BASH
	 SVERB	FIGHT
	 SVERB	BREAK
	 SVERB	CHOKE
	 SVERB	HIT
	 SVERB	KILL

	 DC.B	0

	 SVERB	TOUCH
	 SVERB	RAPE
	 SVERB	RUB

	 DC.B	0

	 DC.L    0

	 END



	ENDC	;wantNOQUOTES


        END



