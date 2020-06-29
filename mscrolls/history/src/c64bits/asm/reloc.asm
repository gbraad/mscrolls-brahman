;**************************************************
;
; reloc.asm (Ken 13/9/86)
;
;**************************************************


;**************************************************
;
; This is the 6502 that sits on the end of the
; runtime stuff.
;
; It gets unhuffed over the start of the unhuff 
; code and then relocate (ok, moves) the runtime
; code to where it will run.
;
; Assumtions:
; The SIZE (in BYTES) of the bit to move is at SP+3,4
; This code NEEDS to KNOW where the runtime bit is to run
; The start address (ie address to jump to) is
; going to be in the last 2 bytes (68000 order!)
; of the runtime bit (makerun4.c will do this 
; using the JMP <start> which is the first 3 bytes
; normally)
;
;**************************************************

;this is the NextBit value from undo.asm

;RealBit		=	$605F
RealBit		=	$6062

		*=	RealBit-$46	;-NextBit+DoMove



; address to move code to to execute
; NB the initial JMP is lost so add 3!!!!

MoveTo		=	$A03		;for C64 (I think)
Destination	=	$14+0		;yet more machine dependant bits
Size		=	$14+2
Source		=	$14+4

DoMove
	LDA	#MoveTo			;point at start of code
	STA	Destination
	LDA	#MoveTo/256
	STA	Destination+1

	TSX				;get no. of BYTES to move
	LDA	$103,X
	STA	Size+1
	LDA	$102,X
	STA	Size

	SEC
	LDA	#NextBit		;work out where it unhuffed to
	SBC	Size
	STA	Source
	LDA	#NextBit/256
	SBC	Size+1
	STA	Source+1

	LDY	#0
Loop
	LDA	(Source),Y		;move it!
	STA	(Destination),Y
	INC	Source
	BNE	Loop_1
	INC	Source+1
Loop_1
	INC	Destination
	BNE	Loop_2
	INC	Destination+1
Loop_2
	DEC	Size
	BNE	Loop
	DEC	Size+1
	BPL	Loop

StartVect	=	DoMove-2

	LDA	StartVect		;now run program
	PHA
	LDA	StartVect+1
	PHA
	RTS
	
	

; This is the last bit of code !

; this is the bit of code which should everlap
; the Z flag is set when it gets executed (I hope!!)

TheStart

	BEQ	DoMove		;Always taken
	.byte	$4C		;one byte padding (just stomped a
				;3 byte instruction)

; this SHOULD be the save value as the NextBit in undo.asm
NextBit


; DO NOT add *ANYTHING* after these 3 bytes !!!!!!!!!!

