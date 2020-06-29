

;#########################################################################
;#
;#	@(#)boot2.asm	1.3
;#	6/25/88
;#
;#########################################################################


;***************************************
;* BOOT2 - continues boot process by   *
;* using RWTS loaded by BOOT1 to grab  *
;* the rest of the runtime code.       *
;***************************************

RWTS_ENTRY	=	$BD00		;This is in the file rwtsboot.asm

	*=	$BF46

BOOT2
	STX	IOBOOT+1	;Works from any slot
	STX	IOBOOT+15
	STX	$FF		;Save slot index
REBOOT
	LDA	#IOBOOT/256
	LDY	#IOBOOT%256
	JSR	ENTER_RWTS
	BCS	REBOOT
	INC	BOOPAGE
	LDA	BOOSECT
	CMP	#15
	BEQ	UPTRACK
	INC	BOOSECT
	JMP	REBOOT
UPTRACK
	INC	BOOTRACK
	LDA	BOOTRACK
	CMP	#$04
	BEQ	BOOT_DONE
	LDA	#$00
	STA	BOOSECT
	JMP	REBOOT
BOOT_DONE
	LDX	$FF
	LDA	$C08A,X		;Engage drive 1
	LDA	$C089,X		;Switch it on
	JMP	$6000		;Go unhuff emu


IOBOOT
		.byte	1,$60,1,$68
BOOTRACK	.byte	0
BOOSECT		.byte	0
		.byte	BOODCT%256
		.byte	BOODCT/256
		.byte	0
BOOPAGE		.byte	$60
		.byte	0,0
		.byte	1
		.byte	0
		.byte	$68,$60,1
BOODCT		.byte	0,1,$EF,$D8

ENTER_RWTS
	PHP
	SEI
	JSR	RWTS_ENTRY
	BCS	BADBOOT
	PLP
	CLC
	RTS
BADBOOT
	PLP
	SEC
	RTS
;* Do NOT add anything after this *
