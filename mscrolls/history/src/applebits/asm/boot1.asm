

;#########################################################################
;#
;#	@(#)boot1.asm	1.3
;#	6/25/88
;#
;#########################################################################


;*************************************
;* Boot code for track 0 sector 0.   *
;* Loaded by ROM on power-up or re-  *
;* boot.			     *
;* When executed,it loads sectors 1  *
;* to 8 on track zero which contain  *
;* RWTS and BOOT2 and then passes    *
;* control to BOOT2.		     *
;* Paul.
;*************************************


		*=	$800

RWTSTART	=	$B800
BOOT2		=	$BF46

INISECT		=	$3D
INITRK		=	$41
LDPOINT		=	$26
ZEROREAD	=	$C65C


	.byte	$1		;Used by ROM to suss
BOOT1				;# of sectors to read.
	LDA	FIRST		;First time through?
	BNE	BT_01		;=> no
	STX	CARDNO
	TXA
	LSR	A
	LSR	A
	LSR	A
	LSR	A
	ORA	#$C0
	STA	BT_01-1

MOD	LDY	#$01
	STY	FIRST
	LDA	TABLE,Y		;Interleave table
	STA	INISECT
	LDA	#RWTSTART%256
	STA	LDPOINT		;Point at destination page
MOD1	LDA	#RWTSTART/256
	STA	LDPOINT+1
	LDA	#$01
	STA	BOOT1-1

	JMP	ZEROREAD
BT_01
	INC	MOD1+1
	INC	MOD+1
	LDA	MOD+1
	CMP	#$9		;All done?
	BNE	MOD
	LDX	CARDNO
	JMP	BOOT2
TABLE
	DFB	$0,$D,$B,$9,$7,$5,$3,$1
	DFB	$E,$C
FIRST	DFB	0
CARDNO	DFB	0

	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
