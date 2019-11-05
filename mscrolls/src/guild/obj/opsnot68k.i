************************************************
*
*	@(#)opsnot68k.i	1.1
*	9/18/86
*
************************************************

* opsnot68k.i - op macros for NON 68k machines

	IFND	BespokeOps

BespokeOps	EQU	1

*
* RET macros - as per 68K RTS
*

RET	MACRO
	DC.W	$A200	
	ENDM

* 
* TEST macros - functionality as per 68K TST instruction
*

XDOTEST	MACRO	* private lable,opcode,operand
       IFEQ	NARG-4
\1	\3	\4
\2
	RORG	\1
	DC.B	$A6
	RORG	\2
	MEXIT
       ENDC
       IFEQ	NARG-5
\1	\3	\4,\5
\2
	RORG	\1
	DC.B	$A6
	RORG	\2
	MEXIT
       ENDC
	FAIL	Bad test arguments
	ENDM

DOTEST	MACRO	* opcode,operand
       IFND	99100$
	XDOTEST 99100$,88100$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99101$
	XDOTEST 99101$,88101$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99102$
	XDOTEST 99102$,88102$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99103$
	XDOTEST 99103$,88103$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99104$
	XDOTEST 99104$,88104$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99105$
	XDOTEST 99105$,88105$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99106$
	XDOTEST 99106$,88106$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99107$
	XDOTEST 99107$,88107$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99108$
	XDOTEST 99108$,88108$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99109$
	XDOTEST 99109$,88109$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99110$
	XDOTEST 99110$,88110$,\1,\2,\3
	MEXIT
       ENDC
       IFND	99111$
	XDOTEST 99111$,88111$,\1,\2,\3
	MEXIT
       ENDC
       FAIL	Too many tests!!
       ENDM

TEST_B	MACRO	* operand
	DOTEST	TST.B,\1,\2
	ENDM

TEST_W	MACRO
	DOTEST	TST.W,\1,\2
	ENDM

TEST_L	MACRO
	DOTEST	TST.L,\1,\2
	ENDM

*
* CALL - as per 68K BSR
*

CALL	MACRO
	BSR.L	\1
	RORG	*-4
	DC.B	$A4
	RORG	*+3
	ENDM

CALL_S	MACRO
	BSR.S	\1
	RORG	*-2
	DC.B	$A4
	RORG	*+1
	ENDM

CALL_L	MACRO
	BSR.L	\1
	RORG	*-4
	DC.B	$A4
	RORG	*+3
	ENDM

*
* PUSH - as per 68K MOVEM <reg list>,-(A7)
*

;PUSH	MACRO
;	MOVEM.W	\1,-(A7)
;	RORG	*-4
;	DC.B	$A8
;	RORG	*+3
;	ENDM

PUSH_W	MACRO
	MOVEM.W	\1,-(A7)
	RORG	*-4
	DC.B	$A8
	RORG	*+3
	ENDM

PUSH_L	MACRO
	MOVEM.L	\1,-(A7)
	RORG	*-4
	DC.B	$A8
	RORG	*+3
	ENDM

*
* PULL - as per 68K MOVEM (A7)+,<reg list>
*

PULL	MACRO
	MOVEM.W	(A7)+,\1
	RORG	*-4
	DC.B	$AA
	RORG	*+3
	ENDM

PULL_W	MACRO
	MOVEM.W	(A7)+,\1
	RORG	*-4
	DC.B	$AA
	RORG	*+3
	ENDM

PULL_L	MACRO
	MOVEM.L	(A7)+,\1
	RORG	*-4
	DC.B	$AA
	RORG	*+3
	ENDM

	ENDC
