*********************************
*
*	%G%
*	%W%
* 
*********************************


* linemacros.i - the official ALINE no.s

	IFEQ	M68000

Search		MACRO
		DC.W	$A0FF
		ENDM

*GETDATA	MACRO
*		DC.W	$A0FE
*		ENDM

SetNouns	MACRO
		DC.W	$A0FD
		ENDM

NounScan	MACRO
		DC.W	$A0FC
		ENDM

WordScan	MACRO
		DC.W	$A0FB
		ENDM

GetNouns	MACRO
		DC.W	$A0FA
		ENDM

QCarried	MACRO
		DC.W	$A0F9
		ENDM

Text		MACRO
		DC.W	$A0F8
		ENDM

Random1		MACRO
		DC.W	$A0F7
		ENDM

Random2		MACRO
		DC.W	$A0F6
		ENDM

LoadFile	MACRO
		DC.W	$A0F5
		ENDM

SaveFile	MACRO
		DC.W	$A0F4
		ENDM

CharOut2	MACRO
		DC.W	$A0F3
		ENDM

GetTied		MACRO
		DC.W	$A0F2
		ENDM

FindZero	MACRO
		DC.W	$A0F1
		ENDM

GraphicsTrap	MACRO
		DC.W	$A0F0
		ENDM

PrinterTrap	MACRO
		DC.W	$A0EF
		ENDM

RestartTrap	MACRO
		DC.W	$A0EE
		ENDM

QuitTrap	MACRO
		DC.W	$A0ED
		ENDM

GetWTabByte	MACRO		* like MOVE.B (A1),D1
		DC.W	$A0EC
		ENDM

PutWTabByte	MACRO		* like MOVE.B D1,(A1)
		DC.W	$A0EB
		ENDM

PrtWTabWord	MACRO		* print hibit terminated string -> A1
		DC.W	$A0EA	* D1 = CHANNEL
		ENDM

CopyWTabWord	MACRO		* copy hibit term string form (A1) to (A0)
		DC.W	$A0E9
		ENDM

InfoAtScreen        EQU     1
InfoAtCommand       EQU     2
InfoDump            EQU     3
InfoHeartBeat       EQU     $ff
         
InfoTrap  MACRO
        DC.W  $A0DC
        ENDM

	IFNE		0		;here for ref only - see macros.i

SETNE		MACRO
		DC.W	$A0E8
		ENDM

SETEQ		MACRO
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

	ENDC

	ENDC	;of M68000 eq
