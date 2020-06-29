;
; recieve a disk image from parallel port
;

SAVESP		EQU	$FFD8
PRIMM		EQU	$FF7D
SETNAM		EQU	$FFBD
SETBNK		EQU	$FF68
LOADSP		EQU	$FFD5
INDFET		EQU	$FF74
INDSTA		EQU	$FF77
CLOSE		EQU	$FFC3
BASIN		EQU	$FFCF


SETLFS		EQU	$FFBA
CIA		EQU	$DD00
DAT		EQU	CIA+1
DDR		EQU	CIA+3
ICR		EQU	CIA+13
ENDPT		EQU     $AE
POINT		EQU	$28
TELLOS		EQU	$02B9
FILNAMLEN	EQU	6
ESC		EQU	$1B

	ORG	$4000

START
	LDA	#1			;current track number
	STA	TRACK
	LDA	#0			;current sector number
	STA	SECTOR
	LDA	#$00			;All lines to i/p (port B)
	STA	DDR
	LDX	#>BUFFER		;Init pointer
	LDY	#<BUFFER
	STX	POINT
	STY	POINT+1
	LDA	#POINT
	STA	TELLOS			;Tell INDSTA routine where z-page
					;pointer is.
LOOP
	JSR	GETBYTE			;Get a byte from user port
	CMP	#ESC			;Possible EOF mark?
	BNE	NESC			;=> Naw
	JSR	GETBYTE			;Hmmm, maybe!
	CMP	#$FE			;Well, is it?
	BEQ	OUT			;=>Yeah! ... now go save it to disk
	LDA	#ESC			;Was just an ESC so save it (losing
NESC					;the second one!).
	JSR	PUTBYTE
	JMP     LOOP			;.....and loop till done

OUT
	JSR	PRIMM			;Tell the world we got it all
	ASC	"GOT THE DATA!"	
	DFB	13,10,0
	LDA	#0
	STA	DDR			;Hmmmm, well why not?
	JSR	SAVEDATA		;Go write it to disk
	BRK				;<BING>

PUTBYTE
       LDY	#0			;Offset
	LDX	#1			;Bank
	JSR	INDSTA			;Save data
	INC	POINT			;Do updates
	BNE	GB_1
	INC	POINT+1
	BNE	GB_1
	JSR	PRIMM
	ASC	"FILE TOO BIG!!!"
	DFB	13,10,0
GB_1
	RTS


SAVEDATA
	LDA	#1			;Logical file number
	LDY	#1			;Write mode
	LDX	#8			;Disk drive
	JSR	SETLFS 

	LDA	#FILNAMLEN
	LDX	#>DFNAME
	LDY	#<DFNAME
	JSR	SETNAM
	LDX	#$C			;Bank where name is
        LDA	#1			;Bank where data is
	JSR	SETBNK
	LDX	POINT			;Point to end address
	LDY	POINT+1
	LDA	#<BUFFER		;And the start!
	STA	POINT+1
	LDA	#>BUFFER
	STA	POINT
	LDA	#POINT			;Tell em where to look
	JSR	SAVESP			;Save the file!

	LDA	#1
	JSR	CLOSE			;Close the file
	BCC	SAV_01			;=> no errors
	JSR	PRIMM
	ASC	"PROBLEM WITH THE SAVE!!"
	DFB	13,10,0
	RTS
SAV_01
	JSR	PRIMM
	ASC	"SAVED OK"
	DFB	13,10,0
	RTS

DFNAME	ASC	"@:FIL1"
	DFB	0


;DOBYTE
GETBYTE
WAIT
	LDA	ICR
	AND	#$10
	BEQ	WAIT
	LDA	DAT
	RTS


nsect	byte	21,21,21,21,21,21,21,21		; first 17 tracks have 21
	byte	21,21,21,21,21,21,21,21,21
	byte	19,19,19,19,19,19,19		; next 7 have 19
	.byte	18,18,18,18,18,18
	.byte	17,17,17,17,17

BUFFER
