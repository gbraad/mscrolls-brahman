******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.INIT
*
*--------------------------------

	include	"equates.asm"
	include	"macros.asm"
	include	"roomequ.i"
	include	"nounequ1.i"


	XDEF    INIT

      IFNE   RestartRAM

	XREF   SLADDR,INITG.E,NEWGAME

      ENDC

	XREF   RND.CHIN,RND.D1,RNDGEN,INITRND3,STWINDOW,ROOM,BIOS
	XREF   OUTMODE,SUBJECT,P.LOC,SCRPNT,LJPOS,POS,DO.SCN
	XREF   PEND2,NPCINRM,LINE,REALBAD,SCREEN,PRTTXT,INITRND
	XREF   D.NOUNS,CHAROUT,TIMERD,TIMERE,GETDATA

	XREF	BIOSA4,INBUFF

INIT

      IFEQ	M68000

      IFNE	BookProtection
	XREF	ProtEnd,ProtData,ProtSize,ProtList

	LEA	ProtEnd(PC),A7

	CALL	BIOSA4
	TEST_L	ProtData(A4)
	BNE.S	20$

*	CALL    INITRND			;null on non 68k machines

15$
	CALL     RNDGEN                   ;returns random in d0
	AND.W	 #$3FF,D0
	CMP.W	 #ProtSize,D0		  ; is it still too big?
	BGE	 15$			  ; Yes, so retry.

	MOVE.L  #ProtList,A0             ;the list of the tables
	ADD.L   A4,A0
	LSL.W   #2,D0                   ;multiply by 4 for index
	ADD.W   D0,A0                   ;a0 now points to correct word.

	MOVE.L	(A0),ProtData(A4)
20$      
      ENDC  ; BookProtection
      ENDC

*	LEA     BIOS(PC),A4
	CALL	BIOSA4

     IFEQ    M68000

	LEA     D.NOUNS-14(A4),A0	;noun data

	XREF	MessageCode,SpaceTable,MinSpaceEntry
	XREF	SLADDR,SLADDR.E

	LEA	SLADDR(A4),A1		;start of restart/save area
	LEA	SLADDR.E(A4),A2		;end of same
	SUB.L	A1,A2
	MOVE.L	A2,D0                   ; save_len

	LEA	MessageCode(PC),A3	;code to deal with MSG macros
	LEA	SpaceTable,A5		;for clever F-lines
	ADD.L	A4,A5
	MOVE.L	#MinSpaceEntry,D7	;F-lines =< this are messages

	DC.W    $A0FD               ;Set base address of noun data - SETNOUNS

     ENDC

      IFNE	PCW8256

	XREF	RestartFlag,UTSAVE
	XDEF	INIT.Restart


	ST	RestartFlag(A4)
*	CALL	UTSAVE
	SF	RestartFlag(A4)

INIT.Restart

      ENDC


     IFNE    QL128
	CLR.B   $18063             ;SENSIBLE VIDEO
	
      IFNE     PROD
 
	MOVE    #0,CCR             ;KILL X BIT !
	MOVE.W  #0,RND.CHIN(A4)
	CALL    INITRND            ;RANDOMISE
	LEA     RND.D1(A4),A3      ;WHERE TO BEGIN
	LEA     $40000,A1          ;WHERE TO END   
10$  
	CALL    RNDGEN
11$
	MOVEQ   #$0F,D1    
12$
	EOR.W   D0,(A3)+   
	DBRA    D1,12$
	CMP.L   A3,A1
	BPL.S   10$  
	LEA     $10+20$/16*16(PC),A3
	LEA     INITRND3(A4),A1      ;INITRND-32
15$
	CALL    RNDGEN
16$
	MOVEQ   #$0F,D1
17$
	EOR.W   D0,(A3)+
	DBRA    D1,17$
	CMP.L   A3,A1
	BPL.S   15$  
20$
      ENDC
     ENDC
INIT2
     IFNE    RestartRAM          ;If RAM restartable then save modifiable bits

	LEA     SLADDR(PC),A0       ;for RESTART game
	LEA     NEWGAME,A1          ;Range errors!
	ADD.L   A4,A1
	LEA     INITG.E(PC),A2      ;End of initial game area excludes funct's
10$
	MOVE.B  (A0)+,(A1)+
	CMP.L   A0,A2
	BNE.S   10$

     ENDC

  
	CALL    STWINDOW
	MOVE.W  #1,ROOM(A4)		;FIRST ROOM
	CLR.W   OUTMODE(A4)		;SET TO NORMAL
	CLR.W   SUBJECT(A4)		;TO PLAYER
	MOVEQ   #1,D0			;FIRST ROOM
	CALL    P.LOC			;LOOKS ODD WITHOUT
	MOVE.W  #1,SCRPNT(A4)
	CLR.W   LJPOS(A4)
	CLR.W   POS(A4)
	CALL    INITRND
	MOVE.W  #-1,PEND2(A4)
	MOVE.W  #-1,NPCINRM(A4)
	CALL	SetUpCard
	CALL	DisappearTimer
	MOVE.B	#30,TIMERE(A4)

        IFNE  Remastered

        ;; call dictionary lookup with a random word "A"
        ;; *before* generating initial text. This will initialise the back-end
      	LEA     INBUFF(A4),A6
        MOVE.L  A6,A0
        MOVE.B  #'A',(A0)+
        CLR.B   (A0)
	DO	SEARCH
        
        ENDC

	SCN	1			; a bit of waffle...


     IFNE    PROD
	BRA     REALBAD            ;SET STACK + PUSH SCREEN ADDR
     ENDC


     IFEQ    PROD
	DOGO	SCREEN
     ENDC

	XREF	CardOut
SetUpCard
* this will take out one of the cards 
	MOVEQ	#4,D1		;lets get a random number
	RND_D1		;returns 0 - 3
	MOVE.B	D1,CardOut(A4)	;Save it for end game
	LEA	CardTable(PC),A0	;the list of cards...
	LSL.W	#1,D1			;Mulu by 2 for index
	MOVE.W	0(A0,D1.W),D0		;get the card
	GETINFO				;to set it out
	BSET	#7,6(A0)		;it has gone chaps...
	RET

CardTable:
	DC.W	NCARD1
	DC.W	NCARD2
	DC.W	NCARD3
	DC.W	NCARD4

DisappearTimer
* set random delay before next treasure disappears
	MOVEQ	#5,D1
	RND_D1
	ADD.B	#10,D1
	MOVE.B	D1,TIMERD(A4)
	RET

	XDEF	DisappearTimer

	END

