;*********************************************
;
;
; undo.asm 6502 unhuf/construct proggy
;
;
;*********************************************

; ken 10/9/86

	*=	$6000		;a random address....
				;must match with ones in reloc.asm

START:
	JMP	UNHUF

	.byte	" CORRUPTION (C) 1988 MAGNETIC SCROLLS LTD   "
;	.byte	" JINXTER (C) 1987 MAGNETIC SCROLLS. Go away "
;	.byte	" GUILD OF THIEVES (C) 1987 MAGNETIC SCROLLS "
;	.byte	" THE PAWN (C) 1985,1986 MAGNETIC SCROLLS "

; this is going to be conditional 


BITSTREAM	=	$14+0	;For C64
TARGET		=	$14+2
SUM		=	$14+4

SIZE	=	START+256	;6502 style - no. of NIBBLES to undo
LTREE	=	SIZE+2		;left branches
RTREE	=	LTREE+16	;right branches
DAT	=	RTREE+16	;the huffed data


; ok, real code starts here...

UNHUF:

	LDA	#DAT		;get address of code being loaded
	STA	BITSTREAM	;put it in some zero page
	LDA	#DAT/256	;(high byte too)
	STA	BITSTREAM+1

	LDA	SIZE+1		;work out where to start to end at 
	LSR	A		;SIZE is no. of nibbles!!
	PHA			;so we know where to start moving from
	STA	TARGET+1
	LDA	SIZE
	ROR	A
	PHA
	STA	TARGET

	LDA	#NextBit	;destination address 
	SEC			;the NextBit LSR (16 bit sub)
	SBC	TARGET
	STA	TARGET
	LDA	#NextBit/256
	SBC	TARGET+1
	STA	TARGET+1

	LDY	#0		;start at begining of data...	
	LDX	#0		;start at top of tree
	INC	SIZE+1		;so we get to FrigBit AFTER it is stomped
	LDA	SIZE+1
	PHA			;so this INC looks real (+RTS later)
	BNE	SameByte	;Always taken

FrigBit:
	
	LDA	BITMASK		;some junk to be over-written with a JMP

NextBit:

	LSR	BITMASK	
	BCC	SameByte	;carry clear => still bits left in this byte

	ROR	BITMASK		;get mask back to $80 (bit 7)
	INC	BITSTREAM	;low byte
	BNE	SameByte
	INC	BITSTREAM+1	;hi byte	

SameByte:

	LDA	(BITSTREAM),Y	;get bitstream
	BIT	BITMASK		;see if bit is set
	BEQ	DO_RIGHT	;0=> right of tree
	LDA	LTREE,X
	JMP	DO_BOTH

DO_RIGHT:

	LDA	RTREE,X

DO_BOTH:

	BPL	GOT_NIBBLE	;Neg => node, Pos => leaf
	AND	#$7F		;loose flag bit
	TAX			;use as next pos in tree
	BNE	NextBit		;Always taken

GOT_NIBBLE:

	TAX
	LDA	NibFlag		;High come first 
	EOR	#$FF		;in one go
	STA	NibFlag
	BNE	HiNibble

LowNibble:

	TXA
	ORA	(TARGET),Y	;get the nibble there already and fall thru
	STA	(TARGET),Y	;save nibble/byte
	INC	TARGET		;add to target cos we got second nibble
	BNE	NextNib
	INC	TARGET+1	
	JMP	NextNib

; data is going here today (mot too nice i know... )

BITMASK:	.byte	$80		;start at bit 7
NibFlag:	.byte	0		;0 => high  (high come first)

HiNibble:

	TXA
	ASL	A		;move to high
	ASL	A
	ASL	A
	ASL	A

	STA	(TARGET),Y	;save nibble/byte

NextNib:
	
	LDX	#0		;back to root of trees
	DEC	SIZE		;low byte of nibble count
	BNE	NextBit

	DEC	SIZE+1
	BPL	FrigBit		;in the end this (FrigBit) will be stomped 

	LDA	SIZE+2		;so it looks like the check sum is frigged
	STX	SUM
	SEC			;ditto

ChkSum:

	LDA	START,X		;lets look like we're doing a checksum
	ADC	SUM
	STA	SUM
	INX
	BNE	ChkSum

	PHA			;to match with earlier PHA (spurious)

	CLD			;add paranoia to hackers
	RTS			;with mustard (see matching PHA's earlier)
