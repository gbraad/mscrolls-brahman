;***********************************************
;* C1541 FAST LOADER - This bit lives in the drive

		ORG	$300

;* 'Serial' port is here.

PORT	EQU	$1800
PORTDDR EQU	PORT+2

;* Bit definitions for serial port
DATBIT EQU	$2
CLKBIT	EQU	$8
IODIR	EQU	$10        ; $USED TO BE $10
FODBITS EQU	DATBIT!IODIR

PORTINT EQU	PORT+$E
;* Equates for where the LED lives.. and which bit it is.

LEDPORT EQU	$1C00		
LEDBIT	EQU	$8

ACK1	EQU	0       		; Acknowledge patterns.
ACK2	EQU	CLKBIT

SECAREA EQU	$700

IDERR	EQU	$0B

;* 1541 zero page bits that we're interested in...
;*
DISKID	EQU	$12
READID	EQU	$16


DRIVE	EQU	$7F
TRACK	EQU	$80
SECTOR	EQU	$81
ERRIND EQU	$04
DATA	EQU	$8B

BUFFER	EQU	$F9
ERRFLAG EQU	$298
RDSECT	EQU	$D460

DIRTRACK EQU	1
DIRSECT  EQU	2	; Follows on end of loader

;* This causes boot sector to be validated.

HDRMSG	.byte	"MS"

	LDA	#1
	JSR	DELAY	

;* Dummy server - get a byte, then send that page of memory.

DISKIO
	LDA	#2
	STA	PORTINT
	SEI
	LDA	#IODIR!DATBIT!CLKBIT	; Set up serial DDR (in case)
	STA	PORTDDR
	LDA	#ACK2
	STA	PORT

;* Now read in the rest of the code....
	LDA	#1			; To $400
	STA	BUFFER
	LDY	#1
	TYA				; Track 1 sector 1

	JSR	RSECTOR

	JSR	LOAD_DIR			; Now load the directory

       JSR	MAINLOOP
	LDA	#$82			;Reenable serial int
	STA	PORTINT
	CLI				;Restore Ints.
	RTS

;* Hang around for an arbitrary length of time

DELAY
	LDY	#0
	LDX	#0
DEL_01
	DEX
	BNE	DEL_01
	DEY
	BNE	DEL_01
	SEC
	SBC	#1
	BNE	DEL_01
	RTS

RSECTOR
	STY	TRACK
	STA	SECTOR
	LDA	#0
	STA	DRIVE

	CLI
	LDA	#$80
	STA	ERRFLAG
	JSR	RDSECT

	SEI

	LDX	ERRIND			; Was there an error?
	CPX	#1    			; Frig 1 to 0
	BNE	WERR10
	DEX
WERR10
	CPX	#IDERR			; Was it a 'wrong disk' error
	BNE	WERR20

	LDA	READID
	LDY	READID+1
	CMP	ID1
	BNE	WERR15
       CPY	ID1+1
	BNE	WERR20
	BEQ	WERR17
WERR15
	CMP	ID2
	BNE	WERR20
	CPY	ID2+1
	BNE	WERR20
WERR17
	LDX	#1		; Its a 'flip disk' error.
	STX	ERRIND
WERR20
	CPX	#0		; Return result

	RTS

; Send a 'duff file request' error.
FILEERR
	LDX	#'F'
       RTS

;*	
;* FILERD is passed file number in X, sector in A
;* Returns Z if no error occured,Else error code in X
;*
FILERD
	LDY	WHERE,X
	STY	DISKNO

	CLC
	ADC	SSECT,X           ; Add on the file's start sector
	LDY	STRACK,X          ; ... and load the file's start track.
	BEQ	FILEERR		  ; This file doesn't exist
TS_001
	CMP	SPT,Y             ; Is the sector on this track??
	BCC	GOTTS		; Branch if it is.
	SEC                       ; Otherwise... knock off the # of sectors
	SBC	SPT,Y             ; ... for this track.
	INY                       ; ... and increment the track number
	BNE	TS_001	         	;In fact, unconditional

GOTTS
	PHA
	LDA	DISKNO	
	BEQ	TS_010
      	CMP	#1
	BNE	TS_005
	LDA	ID1
	STA	DISKID
	LDA	ID1+1
	STA	DISKID+1
	JMP	TS_010
TS_005
	LDA	ID2
	STA	DISKID
	LDA	ID2+1
	STA	DISKID+1

TS_010
       PLA
	JMP	RSECTOR

;*
;* Send the byte passed in X to the serial port.
;*
SBYTE
	LDA	#ACK2 		; Initial config for write.
	STA	PORT
	STX	DATA		; Save data to be sent
	LDX	#7              ; # of bits -1
	LDA	PORT            ; Get port status
	AND	#~FODBITS       ; Clear data/iodir
	ROL	DATA            ; get data bit into carry
	BCC	SB_005	
	ORA	#DATBIT		; Put data into word		
SB_005
 	STA	PORT		; Set up data
	EOR	#CLKBIT         ; Add clock..
	STA	PORT            ; Hit it!
SB_010
	EOR	#CLKBIT         ; Prepare to flip clock ..
	ROL	DATA
SB_012
	BIT	PORT            ; Wait for acknowledge..
	BPL	SB_012

	STA	PORT            ; Remove the clock..
	AND	#~FODBITS       ; Clear data/iodir
	BCC	SB_022                               
	ORA	#DATBIT         ; A is now ready to be stomped
SB_022
	BIT	PORT		;Acknowledge has been removed.
	BMI	SB_022
	STA	PORT            ; So give new data..
	EOR	#CLKBIT
	STA	PORT            ; .. plus clock
	DEX                     ; All done?
	BNE	SB_010
	EOR	#CLKBIT         ; Set up without clock..
SB_090
	BIT	PORT            ; Wait for ACK to be asserted
	BPL	SB_090
	STA	PORT    	; Take away clock
SB_095
	BIT	PORT
	BMI	SB_095		; Wait for ACK to be removed

; CLOCK IS SET TO ACK2 ON EXIT

	RTS			; .. all done!

;*
;* Fetch byte from serial port.
;*
GBYTE
	JSR	TOGLED
	CLI  	
GB_05
	LDA	PORT
	BPL	GB_05
	SEI

	LDA	#ACK1		; Initial setup for port
	STA	PORT
	LDX	#8		; Initialise bit count.
	LDY	#ACK2		; Preload acknowledge.
GB_010				; Wait for ATN (ACK) to be asserted
	LDA	PORT
	BMI	GB_010

	LDA	#$E0
GB_011
	CLC
	ADC	#1	
	BNE	GB_011
	LDA	PORT

	STY	PORT		; Acknowledge it.
	ROR	A		; Data bit into carry...
	ROL	DATA            ; Add the data

GB_020
	LDA	PORT            ; Wait for ATN to be removed
	BPL	GB_020
	LDA	#ACK1		; Then remove our acknowledge
	STA	PORT
	DEX			; All done?
	BNE	GB_010          ; Loop if not.

GB_099
	LDA	PORT
	BMI	GB_099

	LDA	#ACK2
	STA	PORT

	JSR	TOGLED

	LDA	DATA		; Return resulting data in A.
	RTS

MAXFILES	EQU	64

;*************************************************
;* Load the directory sector off disk into temp space,
;* then put it into the STRACK and SSECT arrays.
;*

LOAD_DIR
	LDY	#0
	TYA
LDD_10
	STA	DIRAREA,Y	
	INY
	BNE	LDD_10

	LDY	#DIRTRACK
	LDX	#SECAREA/256-3 ; Read into directory area
	STX	BUFFER
	LDA	#DIRSECT	;
	JSR	RSECTOR

	LDY	#0
	LDX	#0
DIR_XFR
	LDA	SECAREA,X
	BEQ	DIR_99
	STA	STRACK,Y
	INX
	LDA	SECAREA,X
	STA	SSECT,Y
	INX			; Skip over # sectors.
	INX
	LDA	SECAREA,X
	STA	WHERE,Y
	INX
	INY
	BNE	DIR_XFR
DIR_99
      	RTS

SPT
	DFB	21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21
	DFB	00,19,19,19,19,19,19
	DFB	18,18,18,18,18,18
	DFB	17,17,17,17,17

MAINLOOP
	LDA	#SECAREA/256-3		; Was 4
	STA	BUFFER

	JSR	GBYTE			; receive the page number
	CMP	#$FF
	BEQ	DOEXIT
	PHA
	JSR	GBYTE
	TAY
	PLA
	TAX
	TYA
	JSR	FILERD
	PHP
	JSR	SBYTE
       PLP
	BNE	MAINLOOP

	LDA	#SECAREA/256			; $700-$7FF
	JSR	WBUFF
	JMP	MAINLOOP

DOEXIT
	RTS
;*
;* Outputs the page of memory passed in A
;*
WBUFF
	STA	FODIT+2			; Page number --> self-mod instr.
	LDY	#0			; initialise byte count

	JSR	TOGLED			; Led ON
FODIT
	LDX	$1000,Y			; Get byte from buffer
	JSR	SBYTE			; Send it.
	INY				; next byte..
	BNE	FODIT			; loop until all done.
					
;* Fall thru and turn LED off

TOGLED
       LDA	LEDPORT			; Toggle the old LED again.
	EOR	#LEDBIT
	STA	LEDPORT
	RTS

DISKNO	DFB	0
	
ID1	.byte	"GO"
ID2	.byte	"DS"

;* This data at the end...

	ORG	$500

DIRAREA EQU	*	

SSECT	EQU	*
STRACK	EQU	SSECT+MAXFILES
WHERE	EQU	STRACK+MAXFILES

