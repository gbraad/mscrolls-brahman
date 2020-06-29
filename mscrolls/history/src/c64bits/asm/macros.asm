
;#########################################################################
;#
;#	%W%
;#	%G%
;#
;#########################################################################





	.IF	~VM
.MACRO	CONV_P   	
	TXA	

	.ENDM
	.ELSE

.MACRO	CONV_P   	
	JSR	CONPAGE
	.ENDM
	.ENDIF

;***************************************************************************
;* Function GET_PC_WORD
;*
;*  Gets a word from the 68K address space.
;*  PC contains the address of the word that we want (in virtual space),
;*  stored in 6502 fashion, low byte first.
;*
;*  RETURNS : A   High byte of word fetched
;*            X   Low byte of word fetched
;*
;*  N.B. Unlike 'real' 68K's, this doesnt bomb out on odd address fetches,
;*  so this routine can be used for get byte as well as word - but it doesnt
;*  map each byte individually - If they're in different pages, watch it!
;*
;*  N.N.B ADDR is ferked, and is a 'real' address on exit
;*
;*****************************************************************************

.MACRO	GETPCW	
	LDY	#1
	LDA	(PC),Y
	TAX
	DEY
	LDA	(PC),Y
	.ENDM


;***********************************************
;*
;*  ADD2PC bumps up the program counter by 2
;*  
;* Note that PC uses 6502 hi/low byte ordering.
;***********************************************

.MACRO	ADD2PC

	INC	PC
	INC	PC
	BNE	1$		; Overflow?
	JSR	RPCCN2
1$:
       .ENDM


.MACRO	MODULE
	.IF	MODNAMES
	.TEXT	?1		;Huh,so much for literal parameter passing!
	.ELSE
	.ENDIF
	.ENDM

;****************************************************************************
;* These macros are to handle the inadequacies of the RTS stuff when it comes
;* to low and high parts of addresses and coping with relocating them.
;*
;* What we do is this:
;*
;* There is a macro for each possible use of {ADDRESS/256} or {ADDRESS%256}
;* (eg: LDX #LABEL/256 or ADC #LABEL%256).
;*
;* The macros lay down in the _text PSECT a simple structure of opcode and
;* operand as if everything's sane and normal (some hope!). However, all is
;* not as it seems since the operand is in fact a constant to be added to
;* the final operand after linking. Why? Well,consider ADC #(xyzzy/256)+2h!
;* (Yes, it does happen [quite a lot!])
;*
;* Additionally, the macros lay down information in the _data PSECT which
;* is used after linking to suss what we really wanted to do in the first
;* and where it was that we wanted to do it.
;* This information is comprised of 6 bytes made up as follows:
;*
;*
;*
;* Bytes 1 and 2	: These define the value of PC+1 at the time the
;*			  macro was invoked (low,hi).
;*
;* Byte  3 		: This defines the action required as follows:
;*			  0 =>  LDX #>xyzzy
;*			  1 =>  LDX #<xyzzy
;*			  2 =>  LDA #>xyzzy
;*			  3 =>  LDA #<xyzzy
;*			  4 =>  LDY #>xyzzy
;*			  5 =>  LDY #<xyzzy
;*			  6 =>  ADC #>xyzzy
;*			  7 =>  ADC #<xyzzy
;*			  8 =>  SBC #>xyzzy
;*			  9 =>  SBC #<xyzzy
;*			 10 =>	CMP #>xyzzy
;*			 11 =>	CMP #<xyzzy
;*			 12 =>  DFB >xyzzy
;*			 13 =>	DFB <xyzzy
;*
;* Bytes 4 and 5	: These define the 16 bit address (low,hi)
;*			  of the label we want to use.
;*
;* After linking, the information in the _data PSECT and the operand field
;* is used by a sneaky program called 'rtsfrig' to fod the correct values
;* in the object code.
;*
;* So, a typical macro call may go something like this:
;*
;* 			LDXLSB LABEL,5
;* which loads X with the low 8 bits of the 16 bit address of LABEL and adds
;* 5 to it.
;*
;* NB the second parameter is optional and defaults to zero, ie no constant
;*    is added.
;*    There is no provision for subtracting constants. eg: LDA #>xyzzy-3 is not
;*    catered for.
;*
;*****************************************************************************

.MACRO	LDXLSB			;Label (,optional constant)
1$:
	LDX	#00h		;If no constant parameter
	.PSECT	_data
	.ADDR	1$+1		;PC value at operand (low,hi)
	.BYTE	0		;Action
	.ADDR	(?1)		;16 bit address of label (hi,low)
	.PSECT	_text
	.ENDM

.MACRO	LDXMSB
1$:
	LDX	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	1
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	LDALSB
1$:
	LDA	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	2
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	LDAMSB
1$:
	LDA	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	3
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	LDYLSB
1$:
	LDY	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	4
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	LDYMSB
1$:
	LDY	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	5
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	ADCLSB
1$:
	ADC	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	6
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	ADCMSB
1$:
	ADC	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	7
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	SBCLSB
1$:
	SBC	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	8
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	SBCMSB
1$:
	SBC	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	9
	.ADDR	(?1)
	.PSECT	_text
	.ENDM

.MACRO	CMPLSB
1$:
	CMP	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	10
	.ADDR	(?1)
	.PSECT	_text
	.ENDM

.MACRO	CMPMSB
1$:
	CMP	#00h
	.PSECT	_data
	.ADDR	1$+1
	.BYTE	11
	.ADDR	(?1)
	.PSECT	_text
	.ENDM

.MACRO	BYTELSB
1$:
	.BYTE	0
	.PSECT	_data
	.ADDR	1$
	.BYTE	12
	.ADDR	(?1)
	.PSECT	_text
	.ENDM

.MACRO	BYTEMSB
1$:
	.BYTE	0
	.PSECT	_data
	.ADDR	1$
	.BYTE	13
	.ADDR	(?1)
	.PSECT	_text
	.ENDM


.MACRO	ASC
	.TEXT	?1
.IF	?0 = 2
	.BYTE (?2|80h)
.ELSE
.ENDIF
	.ENDM


.MACRO		ZEQU					;label
.define ?1	=	ZBASE
.ENDM


.MACRO		ZDEF					;label,size
.define	?1	=	ZBASE
.set	ZBASE	=	ZBASE+?2
.ENDM

