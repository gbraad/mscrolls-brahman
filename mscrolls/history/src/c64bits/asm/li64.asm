;
; recieve a disk image from parallel port
;

SAVESP		EQU	$FFD8
PRIMM		EQU	$FF7D
SETNAM		EQU	$FFBD
OPEN		EQU	$FFC0
CLOSE		EQU	$FFC3
BSOUT		EQU	$FFD2
CKOUT		EQU	$FFC9
CLRCH		EQU	$FFCC
SETLFS		EQU	$FFBA
CLALL		EQU	$FFE7
CIA		EQU	$DD00
DAT		EQU	CIA+1
DDR		EQU	CIA+3
ICR		EQU	CIA+13

	ORG	$4000

START
	LDA	#13
	JSR	BSOUT

	LDA	#1			;current track number
	STA	TRACK
	LDA	#0			;current sector number
	STA	SECTOR
	LDA	#$00			;All lines to i/p (port B)
	STA	DDR
	JSR	INITDISK		;open files

TRACK_LOOP
	LDX	TRACK			;suss how many sectors per track
	DEX
	LDA	NSECT,X
	STA	SLEFT			;how many sectors to load
	LDA	#0
	STA	SECTOR			;start loading at sector zero

SECTOR_LOOP
	LDA	#0			;next byte to load
	STA	BYTEP

LOOP
	JSR	GETBYTE			;Get a byte from user port
	LDX	BYTEP			;store byte in buffer
	STA	BUFFER,X
	
	INC	BYTEP
	LDA	BYTEP
	CMP	#0
	BNE	LOOP

	JSR	SAVESECT		;save this sector to disk
	INC	SECTOR			;next sector for this track
	DEC	SLEFT
	BNE	SECTOR_LOOP

	INC	TRACK
	LDA	TRACK
	CMP	#36			;done?
	BNE	TRACK_LOOP

	JSR	PRIMM			;Tell the world we got it all
	.BYTE	"GOT THE DATA!"	
	.BYTE	13,0
	BRK

INITDISK
	JSR	CLALL
	LDA	#15			;open 15,8,15
	LDX	#8
	LDY	#15
	JSR	SETLFS
	LDA	#NAME15LEN
	LDX	#>NAME15
	LDY	#<NAME15
	JSR	SETNAM
	JSR	OPEN
	BCS	ERROR
	LDA	#2			;open 2,8,2,"#"
	LDX	#8
	LDY	#2
	JSR	SETLFS
	LDA	#NAME1LEN
	LDX	#>NAME1
	LDY	#<NAME1
	JSR	SETNAM
	JSR	OPEN
	BCS	ERROR
	RTS

ERROR	JSR	CLRCH
	JSR	PRIMM
	.BYTE	"DISK ERROR"
	.BYTE	13,0
	BRK

SAVESECT
	LDX	#15			;command channel
	JSR	CKOUT
	BCS	ERROR
	JSR	PRIMM
	.BYTE	"B-P 2 0"		;set buffer pointer to 0
	.BYTE	0
	JSR	CLRCH
	LDX	#2			;set to write to disk buffer
	JSR	CKOUT
	BCS	ERROR
	LDX	#0			;write data for sector
SS_LOOP
	LDA	BUFFER,X
	TXA
	PHA
	JSR	BSOUT
	BCS	ERROR
	PLA
	TAX
	INX
	BNE	SS_LOOP
	JSR	CLRCH
	LDX	#15			;print to command channel
	JSR	CKOUT
	BCS	ERROR
	JSR	PRIMM
	.BYTE	"U2 2 0"		;save a sector
	.BYTE	0
	LDA	TRACK			;track to save on
	JSR	PRTDEC
	LDA	SECTOR			;sector to save
	JSR	PRTDEC
	JSR	CLRCH
	RTS

;print value of A in decimal
;stomps A,X
PRTDEC
	CMP	#0			; negative ?
	BMI	PD_NEG
	PHA
	LDA	#32			; no so print space
	JSR	BSOUT
	PLA
	JMP	PD_GFI
PD_NEG
	PHA
	LDA	#'-'			; yes
	JSR	BSOUT
	PLA
PD_GFI
	CMP	#10			; suppress leading zeros
	BCC	PD_NOTENS
	CMP	#100
	BCC	PD_NOHUN
	LDX	#0			; do hundreds
PD_HLOOP
	CMP	#100
	BCC	PD_DONEHUN
	INX
	SEC
	SBC	#100
	BCS	PD_HLOOP		; carry always clear
PD_DONEHUN
	PHA				; save number
	TXA
	CLC
	ADC	#$30
	JSR	BSOUT
	PLA
PD_NOHUN
	LDX	#0			; do tens
PD_TLOOP
	CMP	#10
	BCC	PD_DONETENS
	INX
	SEC
	SBC	#10
	BCS	PD_TLOOP		; carry always clear
PD_DONETENS
	PHA				; save number
	TXA
	CLC
	ADC	#$30
	JSR	BSOUT
	PLA
PD_NOTENS
	CLC				; print digits
	ADC	#$30
	JSR	BSOUT
	RTS

;DOBYTE
GETBYTE
WAIT
	LDA	#255
	RTS
	LDA	ICR
	AND	#$10
	BEQ	WAIT
	LDA	DAT
	RTS

NSECT	.BYTE	21,21,21,21,21,21,21,21		; first 17 tracks have 21
	.BYTE	21,21,21,21,21,21,21,21,21
	.BYTE	19,19,19,19,19,19,19		; next 7 have 19
	.BYTE	18,18,18,18,18,18
	.BYTE	17,17,17,17,17

TRACK	.BYTE	0
SECTOR	.BYTE	0
SLEFT	.BYTE	0
BYTEP	.BYTE	0

NAME15	.BYTE	""
NAME15LEN	EQU	0
NAME1	.BYTE	"#"
NAME1LEN	EQU	1

; space to store the sector
BUFFER
