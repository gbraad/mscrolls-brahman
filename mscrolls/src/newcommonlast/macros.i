******************************************************
*
*	%W%
*	%G%
*
******************************************************

*--------------------------------
*
*  SRC.ADV.MACROS
*
*--------------------------------

	include	"scenmacros.i"		;for equates & additional macros
 
*--------------------------------
*
* BHS - isnt a  legal opcode, so it will be soon...
*

BHS   MACRO
      BCC     \1
      ENDM

*--------------------------------
*
* ALIGN - PAD TO EVEN ADDRESS
*
*--------------------------------
 
ALIGN   MACRO
        CNOP   0,2
        ENDM
 
*--------------------------------
*
* PRINT  - DO A PRTTXT
*
*--------------------------------

PRINT   MACRO
        DO  PRTTXT
        DC.B  \1
        DC.B  0
        ALIGN
        ENDM
 
*--------------------------------
*
* WORD - DO ASCII+00 TERM
*
*--------------------------------
 
ASCWORD   MACRO 
       DC.B   \1,0
       ENDM

*--------------------------------
*
* SCORE - ADD TO SCORE (ONLY
*          DOES IT ONCE)
*
*--------------------------------

SCORE   MACRO
        DO	DO.SCORE
        DC.W    SC.\1
*	PRINT	<'[scored \1]'>
        ENDM
 
*--------------------------------
*
* SCN - PRINT SCENARIO MSG
*
*      SCN    XYZ
*
*--------------------------------


BeSure	MACRO	* a label
	IFND	\1
	XREF	\1
	ENDC
	ENDM

    IFND	wantUNIFIEDTEXT
    IFEQ	M68000
SCN	MACRO
	BeSure	MsgSCNBASE
	DC.W     $F000+MsgSCNBASE+\1
	ENDM
    ENDC

    IFNE	M68000
SCN	MACRO
	DO	DO.SCN
	DC.W	\1
	ENDM
    ENDC
    ENDC	;!wantUNIFIEDTEXT

    IFD		wantUNIFIEDTEXT
    IFEQ	M68000
SCN	MACRO
	DC.W     $F000+\1
	ENDM
    ENDC

    IFNE	M68000
SCN	MACRO
	DO	DO.SCN
	DC.W	\1
	ENDM
    ENDC
    ENDC	;wantUNIFIEDTEXT
	
*--------------------------------

      IFEQ	M68000

NPCMSG	MACRO
	BeSure	MsgNPCBASE
	DC.W	$F000+MsgNPCBASE+\1
	ENDM

      ENDC

      IFNE	M68000

NPCMSG	MACRO
	IFND	DO.NPCMSG
	XREF	DO.NPCMSG
	ENDC
	CALL	DO.NPCMSG
	DC.W	\1
	ENDM
	
      ENDC

*--------------------------------
*
* MSG - PRINT PROGRAM MSG
*
*      MSG    XYZ   = CALLW.XYZ (FOR REAL 68K MACHINES)
*
*--------------------------------


    IFND	wantUNIFIEDTEXT
    IFEQ	M68000
MSG	MACRO
	BeSure	M.\1
	DC.W	$F000+M.\1
	ENDM
    ENDC

    IFNE	M68000
MSG	MACRO
	BeSure	M.\1
	DO	DO.MSG
	DC.W	M.\1
	ENDM
    ENDC
    ENDC	;!wantUNIFIEDTEXT
    
    IFD	wantUNIFIEDTEXT
MSG	MACRO	
	BeSure	M.\1
	SCN	M.\1
	ENDM
    ENDC	;wantUNIFIEDTEXT
	
* MSGGO - instead og BRA W.xyzzy

	IFEQ	M68000

MSGGO	MACRO
	BeSure	M.\1
	DC.W	$F800+M.\1
	ENDM

	ENDC

	IFNE	M68000

MSGGO	MACRO
*	BRA	W.\1
	MSG	\1
	RTS
	ENDM

	ENDC
    

*--------------------------------
*
* GETINFO - Macro equivilent of GETDATA
*
*--------------------------------

	IFEQ	FastBits
GETINFO  MACRO
    DO     GETDATA
    ENDM
	ENDC

	IFNE	FastBits
GETINFO	 MACRO
    DC.W	$A0FE
    ENDM
	ENDC


***********************************************************
*
* TProt  - see if they've knobbled our read disk bits
*
* usage:  TProt n
*
* where n between 1 and 9
*
************************************************************


TESTPROT MACRO

         IFNE     ABSOLUTELY_THE_LAST_LINK

         XREF     FRIG_\1,CHK.FRIG

         MOVE.L   D1,-(A7)
         JSR      CHK.FRIG(A4)
         CMP.L    #MAGIC_NORM,D1
         BEQ.S    TProt1\@

TProt0\@
         PULL_L   D0-D7/A0-A6       ;this should kill it

TProt1\@
         MOVE.L   #\1,FRIG_\1(A4)
         JSR      CHK.FRIG(A4)
         CMP.L    #MAGIC_FRIG\1,D1
         BNE.S    TProt0\@
         CLR.L    FRIG_\1(A4)
         MOVE.L   (A7)+,D1

         ENDC

         ENDM

BRAL	MACRO	* routine
	XREF	_\1
	JMP	_\1(A4)
	ENDM

BSRL	MACRO	* routine
	XREF	_\1
	JSR	_\1(A4)
	ENDM

	IFD	HardToLink

LCALL	MACRO	* routine
	PEA	*+14(PC)
	MOVE.L	A4,-(A7)
	ADD.L	#\1,(A7)
	RET
	ENDM

LJUMP	MACRO	* routine
	MOVE.L	A4,-(A7)
	ADD.L	#\1,(A7)
	RET
	ENDM

	ENDC

	IFND	HardToLink

LCALL	MACRO	* routine
	CALL	\1
	ENDM

LJUMP	MACRO	* routine
	BRA	\1
	ENDM

	ENDC

* POPSP - used to lose a return address from stack, esp. in specials!
* NB: do not change to addq as this affects the flags
POPSP	MACRO
	ADD.L	#4,A7
	ENDM


******************************************
* MACROS used to make tables for
* the 'MessageFromTable' routine.
******************************************

MFT_Noun MACRO
	DC.W	N\1+HI
	ENDM

MFT_OtherNoun MACRO
	DC.W	N\1,N\2+HI
	ENDM

MFT_Scen MACRO
	DC.W	N\1,\2
	ENDM

MFT_End	MACRO
	DC.W	0
	ENDM

IfVerb	MACRO
	IFD	VN\1
	ENDM

*       
* SVERB is used to build lists of verb bytes. A byte is only laid down
* if the required verb is defined already.
* 

SVERB	MACRO

	IFD	VN\1		; if verb defined

	  DC.B	  VN\1

	ENDC

	ENDM


	IFEQ		M68000


DO	MACRO	* routine name
	BeSure	DO_Num_\1
	DC.W	$F000+DO_Num_\1
	ENDM

DOGO	MACRO	* routine name
	BeSure	DO_Num_\1
	DC.W	$F800+DO_Num_\1
	ENDM

RND_D1	MACRO
	DC.W	$A0F6
	ENDM

	ENDC

	IFNE	M68000

DO	MACRO
	BeSure	DO_Num_\1
	JSR	DO_Num_\1(A4)
	ENDM

DOGO	MACRO
	BeSure	DO_Num_\1
	JMP	DO_Num_\1(A4)
	ENDM
		
RND_D1	MACRO
	DO	RND.D1
	ENDM

	ENDC


Connect	MACRO	* room1,room2,dir
	BeSure	CONNECT3
	CALL	CONNECT3
	DC.B	RN\1,DIR_\3,RN\2,0
	ENDM

Connect4	MACRO	* room1,room2,dir
		BeSure	CONNECT3
		CALL	CONNECT3
		DC.B	\1,\3,\2,0
		ENDM

    IFNE	M68000
LINK_	MACRO
	link	\1,\2
	ENDM
	
UNLINK_	MACRO
	unlk	\1
	ENDM

    ENDC	;M68000
	
    IFEQ	M68000

LINK_	MACRO	
	MOVE.L	\1,-(A7)
	MOVE.L	A7,\1
	IFNC	'#0','\2'
	ADD.W	\2,A7
	ENDC
	ENDM
UNLINK_	MACRO
	MOVE.L	\1,A7
	MOVE.L	(A7)+,\1
	ENDM
	
    ENDC	;!M68000

PUSH	MACRO
	MOVE.W	\1,-(a7)
	ENDM
POP	MACRO
	MOVE.W	(A7)+,\1
	ENDM


************************
* parameter macros

ARGW	MACRO
	MOVE.W	\1,-(A7)
	ENDM

ARGL	MACRO
	PEA	\1
	ENDM

ARGD	MACRO
	MOVE.L	\1,-(A7)
	ENDM

ADJUST	MACRO
	IFGT	\1-8
	ADD.L	#\1,A7
	ENDC
	IFLE	\1-8		;otherwise
	IFNE	\1		;!=0
	ADDQ.L	#\1,A7		;addq
	ENDC
	ENDC
	ENDM

RTD_	MACRO
	RET
	ENDM			;this cant be done currently

	
CALLS	MACRO
	CALL	\2
	ADJUST	\1
	ENDM


* sort of switch statement macros...	(see case.elt)

CaseOf	MACRO	* variable
	BeSure	DO_CaseOf
	MOVE.W	\1,-(A7)
	CALL	DO_CaseOf
	ENDM

When	MACRO	* constant,label to execute
	DC.W	\2,\1
	ENDM

EndCase	MACRO
	DC.W	0
	ENDM

	IFNE	M68000

SCMSG	MACRO
	BeSure	\1
	IFEQ	NARG-2
	DO	DO.SCN
	DC.W    \1+\2
	ENDC
	IFNE	NARG-2
	DO	DO.SCN
	DC.W    \1
	ENDC
	ENDM
	
	ENDC

	IFEQ	M68000

SCMSG	MACRO
	BeSure	\1
	IFEQ	NARG-2
	DC.W     $F000+\1+\2
	ENDC
	IFNE	NARG-2
	DC.W     $F000+\1
	ENDC
	ENDM

	ENDC

	IFNE	M68000

SCMGO	MACRO
	BeSure	\1
	IFEQ	NARG-2
	DO	DO.SCN
	DC.W    \1+\2
	RTS
	ENDC
	IFNE	NARG-2
	DO	DO.SCN
	DC.W    \1
	RTS
	ENDC
	ENDM
	
	ENDC

	IFEQ	M68000

SCMGO	MACRO
	BeSure	\1
	IFEQ	NARG-2
	DC.W     $F800+\1+\2
	ENDC
	IFNE	NARG-2
	DC.W     $F800+\1
	ENDC
	ENDM

	ENDC

	IFNE	M68000

*--------------------------------
*
* SETNE - CLEAR ZERO FLAG
*
*
*--------------------------------
       
SETNE   MACRO
        MOVE    #0,CCR             ;CLEAR ZERO FLAG
        ENDM

*--------------------------------
*
* SETEQ - SET ZERO FLAG
*
*
*--------------------------------
       
SETEQ   MACRO
        MOVE    #4,CCR             ;SET ZERO FLAG
        ENDM

SETNE_RET	MACRO
		MOVE	#0,CCR
		RET
		ENDM

SETEQ_RET	MACRO
		MOVE	#4,CCR
		RET
		ENDM

POPSP_RET	MACRO
		ADDQ.W	#4,A7
		RET
		ENDM

TST_BROKEN	MACRO
		BTST	#2,(A0)
		ENDM
TST_CLOSED	MACRO
		BTST	#5,(A0)
		ENDM
TST_CONTAINED	MACRO
		BTST	#5,6(A0)
		ENDM
TST_DEAD	MACRO
		BTST	#1,(A0)
		ENDM
TST_EXPLORED	MACRO
		BTST	#4,4(A0)
		ENDM
TST_HIDDEN	MACRO
		BTST	#1,6(A0)
		ENDM
TST_INITIALDONE	MACRO
		BTST	#1,5(A0)
		ENDM
TST_INSEPARABLE	MACRO
		BTST	#4,6(A0)
		ENDM
TST_INTRINSIC	MACRO
		BTST	#3,6(A0)
		ENDM
TST_ISAROOM	MACRO
		BTST	#6,6(A0)
		ENDM
TST_LIGHTABLE	MACRO
		BTST	#7,4(A0)
		ENDM
TST_LIT	MACRO
		BTST	#7,(A0)
		ENDM
TST_LITROOM	MACRO
		BTST	#2,5(A0)
		ENDM
TST_LOCALCONTENTS	MACRO
		BTST	#3,(A0)
		ENDM
TST_LOCKED	MACRO
		BTST	#6,(A0)
		ENDM
TST_MONEY	MACRO
		BTST	#3,4(A0)
		ENDM
TST_MOVEABLE	MACRO
		BTST	#4,3(A0)
		ENDM
TST_NPC	MACRO
		BTST	#6,4(A0)
		ENDM
TST_NPCREFUSES	MACRO
		BTST	#5,4(A0)
		ENDM
TST_OPEN	MACRO
		BTST	#4,(A0)
		ENDM
TST_OUT	MACRO
		BTST	#7,6(A0)
		ENDM
TST_PLURAL		MACRO
		BTST	#0,(A0)
		ENDM
TST_PSEUDO	MACRO
		BTST	#0,5(A0)
		ENDM
TST_SITTABLE	MACRO
		BTST	#3,5(A0)
		ENDM
TST_WET	MACRO
		BTST	#5,3(A0)
		ENDM
TST_WORNBYPLAYER	MACRO
		BTST	#2,6(A0)
		ENDM
TST_RELATED	MACRO
		BTST	#0,6(A0)
		ENDM
	ENDC

	IFEQ	M68000

SETNE	MACRO
	DC.W	$A0E8
	ENDM

SETEQ	MACRO
	DC.W	$A0E7
	ENDM

SETNE_RET	MACRO
		DC.W	$A0E6
		ENDM
	    
SETEQ_RET	MACRO
		DC.W	$A0E5
		ENDM

POPSP_RET	MACRO
		DC.W	$A0E4
		ENDM


TST_BROKEN	MACRO
	DO	is_TST_BROKEN
	ENDM
TST_CLOSED	MACRO
	DO	is_TST_CLOSED
	ENDM
TST_CONTAINED	MACRO
	DO	is_TST_CONTAINED
	ENDM
TST_DEAD	MACRO
	DO	is_TST_DEAD
	ENDM
TST_EXPLORED	MACRO
	DO	is_TST_EXPLORED
	ENDM
TST_HIDDEN	MACRO
	DO	is_TST_HIDDEN
	ENDM
TST_INITIALDONE	MACRO
	DO	is_TST_INITIALDONE
	ENDM
TST_INSEPARABLE	MACRO
	DO	is_TST_INSEPARABLE
	ENDM
TST_INTRINSIC	MACRO
	DO	is_TST_INTRINSIC
	ENDM
TST_ISAROOM	MACRO
	DO	is_TST_ISAROOM
	ENDM
TST_LIGHTABLE	MACRO
	DO	is_TST_LIGHTABLE
	ENDM
TST_LIT	MACRO
	DO	is_TST_LIT
	ENDM
TST_LITROOM	MACRO
	DO	is_TST_LITROOM
	ENDM
TST_LOCALCONTENTS	MACRO
	DO	is_TST_LOCALCONTENTS
	ENDM
TST_LOCKED	MACRO
	DO	is_TST_LOCKED
	ENDM
TST_MONEY	MACRO
	DO	is_TST_MONEY
	ENDM
TST_MOVEABLE	MACRO
	DO	is_TST_MOVEABLE
	ENDM
TST_NPC	MACRO
	DO	is_TST_NPC
	ENDM
TST_NPCREFUSES	MACRO
	DO	is_TST_NPCREFUSES
	ENDM
TST_OPEN	MACRO
	DO	is_TST_OPEN
	ENDM
TST_OUT	MACRO
	DO	is_TST_OUT
	ENDM
TST_PLURAL		MACRO
	DO	is_TST_PLURAL	
	ENDM
TST_PSEUDO	MACRO
	DO	is_TST_PSEUDO
	ENDM
TST_SITTABLE	MACRO
	DO	is_TST_SITTABLE
	ENDM
TST_WET	MACRO
	DO	is_TST_WET
	ENDM
TST_WORNBYPLAYER	MACRO
	DO	is_TST_WORNBYPLAYER
	ENDM
TST_RELATED	MACRO
	DO	is_TST_RELATED
	ENDM

	ENDC

*------------------------------------------

* This sequence of instructions crops up a few times
* in the code. This macro replaces the old code
* and makes way for a change.

LIQUID		MACRO	\1
		MOVE.B	1(A0),\1
		LSR.B	#4,\1
		CMP.B	#1,\1
		ENDM

*------------------------------------------

NVALID		MACRO	;noun#
		XREF	IsItemValid
		CALL	IsItemValid
		DC.W	N\1
		ENDM

*------------------------------------------

STOPWAIT	MACRO
		XREF	Waiting
		CLR.W	Waiting(A4)
		ENDM

*------------------------------------------

ScenPic		MACRO
	IFNE	Graphics
		XREF	Do_PictureD0,\1
		CALL	Do_PictureD0
		DC.W	\1
	ENDC	;Graphics
		ENDM

*----------------------------------------

MOVEQW		MACRO
		IFLE	\1-127
		MOVEQ	#\1,\2
		ENDC
		IFGT	\1-127
		MOVE.W	#\1,\2
		ENDC
		ENDM

*----------------------------------------

