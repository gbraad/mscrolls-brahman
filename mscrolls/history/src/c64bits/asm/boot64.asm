;********************************
;* Load the adventure into memory
;********************************

	.PROCESSOR R6500

	.DEFINE	DES=62H,SRC=64H

; Note, the code has origin 2c0 and so all orgs are really relative to
; this. The code will be relocated after assembly.

; this code will load so that the basic jump vectors at 300-
; will be changed so that basic will jump to LOADADV
;
; to make a finished boot program, take this, then cat 1024 bytes of
; screen info (loading screen) onto end, then cat huffed program with
; devious unhuf code onto end
;
; all this is done for you by makeboot64

; some space for a line of basic in case some coolie loads it as
; a basic prog
	.WORD	2C0H	; load address for DOS

START:
;	.ORG	2C0H

; 1 LOAD"GAME",8,1
	.WORD	800H,1
	.BYTE	93H
	.TEXT	"\042GAME\042,8,1"
	.WORD	0,0

; round up to 300h
;	.ORG	300H
	.ORG	START+40h

; some random vectors
 	.WORD	0E38BH,LOADADV,0A57CH,0A71AH,0A7E4H,0AE86H,0,0
	JMP	0B248H
	.BYTE	0
	.WORD	0EA31H,00266H,0FE47H,0F34AH,0F291H,0F20EH
	.WORD	0F250H,0F333H,0F157H,0F1CAH
	.WORD	0F6EDH,0F13EH,0F32FH,0FE66H,0F4A5H,0F5EDH

; round up to 340h where the code really starts
;	.ORG	340H
	.ORG	START+80H

	.DEFINE	SRCADD=0820H	; just after the screen data
	.DEFINE	DESADD=6000H	; where the huffed code (with undo.o at start)
				; is to go.
	.DEFINE	NUMPAG=64	; set to size of huffed code (+undo.o) as
				; produced by makeboot

LOADADV:
	LDA	#SRCADD%256
	STA	SRC
	LDA	#SRCADD/256
	STA	SRC+1
	LDA	#DESADD%256
	STA	DES
	LDA	#DESADD/256
	STA	DES+1

	LDY	#0
	LDX	#NUMPAG
LOLOOP:
	LDA	(SRC),Y
	STA	(DES),Y
	INY
	BNE	LOLOOP
	INC	SRC+1
	INC	DES+1
	DEX
	BNE	LOLOOP
	
	JMP	6000H

; round up to 400h
;	.ORG	400H
	.ORG	START+140H

	.END
