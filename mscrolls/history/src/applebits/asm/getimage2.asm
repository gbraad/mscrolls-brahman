
;**************************************************************************
;
;	@(#)getimage2.asm	1.1
;	6/24/88
;
;**************************************************************************


;
; Proggy for AppleII to build a disk from an image file on a remote system.
; Accepts data in a predetermined format via Apple Super Serial Card,
; buffers it in RAM before writing buffer to disk as necessary.
;
; Data format is as follows:
;
; Input is expected to be in 4-bit chunks (low then high) because of Apple's
; inherant 4-bit protocols.
;
; Protocol is as follows:
;
; A 'packet header' preceeds each packet and comprises
; a 0 followed by the packet number (packet numbers have an 8-bit resolution,
; 1-255). This sequence is unique since any other 0's are doubled to 00
; by the remote sender.
;
; A 'packet' is made up of a packet header followed by 256 bytes of data
; and a 16-bit checksum. The checksum is calculated simply by summing
; the 256 data bytes.
;
; The remote system expects this end to send 3, 23 (arbitrary) in response 
; to receiving a valid checksum. Any other values imply a checksum error
; and the remote system will resend the last packet.
;
; XON/XOFF flow control controls the remote system and is expected at
; specified points rather than checked for on each reception.
;
; Apple Disc format: 35 Tracks of 16 Sectors each. Bytes/sector 256
;
; ******************************************************************
;
; NB: This version sends a fixed response in place of echoing the checksum
;     due to hassles sending certain values via the SSC.
;
;     There is a problem with sending certain values via the SSC.
;
; Paul 18/6/88.

RWTS		=	$03D9			;Dos's Read Write Track Sector 
						;routine vectors through here.
CSW		=	$0036			;Charout vector.
KSW		=	$0038			;Charin vector.
Pointer		=	$0000			;Zero-Page pointer address
TempStore	=	$03			;Temp Zero-Page
Buffer		=	$0800			;Data buffer start
WRITE		=	2			;Command for RWTS
SlotNumber	=	2			;Slot Number of serial card
COUT1		=	$FDF0			;Charout to screen
MaxPage		=	$8F			;No further than this!
CR		=	$0D


		*=		$9000

;
; GetImage	-	Receives Disk Image via Apple Super Serial Card,
;			buffers data and writes it on a sector-by-sector
;			basis to disk.
;

GetImage

	TSX					;Get entry point stack level
	STX	Stack				;Save it
	LDA	#1
	STA	Attempt
	JSR	InitSSC				;Init Serial card
	JSR	InitBuffers			;Init ptrs to data buffer
	JSR	InitIOB				;Init I/O block

; Fall through into main loop

MainLoop
Retry
	JSR	GetByte				;Get packet number
	BCC	Retry				;=> out of sync, retry
	STA	Packetnumber
	JSR	XOFF
	JSR	RECPACK				;"Receiving packet "
	LDA	Packetnumber
	JSR	HEXBYTE				;packet #
	JSR	TRYNUM				;"Try "
	LDA	Attempt				;# of tries at this packet
	INC	Attempt
	JSR	HEXBYTE
	LDA	#CR
	JSR	CHROUT
	JSR	XON


GetNextPacket
	LDY	#0

GetNextByte
	JSR	GetByte				;Get data
	STA	(Pointer),Y			;Buffer the data
	JSR	CalcChkSum			;Update the checksum
	INY
	BNE	GetNextByte
	JSR	EchoChkSum			;Go Echo the Checksum to Vax
	BCC	GNP_01				;=> All okay
	JSR	XON
	JMP	Retry
GNP_01
	JSR	NextPage			;Bump buffer page #
	BCS	GNP_02				;=> buffer full
	JSR	XON				;Keep it coming
	JMP	MainLoop			;Back for next packet
GNP_02
	JSR	XOFF				;Woooah dragon
	JSR	WriteBuffer			;Send buffer to disk
	BCS	AllDone				;=> finished, exit
	JSR	XON
	JMP	MainLoop
AllDone
	JSR	XON
ExitClean

	LDY	#0
ExC_01
	LDA	ExitData,Y
	PHP
	AND	#$7F
	JSR	PrintA
	INY
	PLP
	BPL	ExC_01
	LDX	Stack
	TXS
	BRK					;Exit to monitor
	RTS

Packetnumber	.byte	0

;
; NextPage	-	Bump buffer ptr and check for a full buffer
;			Return carry CLR if ok, else carry SET if buffer
;			full.
;


NextPage
	LDA	#1				;reset retry count
	STA	Attempt

	INC	Pointer+1			;Bump it
	LDA	Pointer+1
	CMP	#MaxPage+1			;Too big?
	RTS					;NB Exit with carry set up!!


;
; EchoChkSum	-	Receive checksum from vax & send our idea of the
;			checksum to the vax.
;			Returns carry set if checksums do not agree
;			Else, carry clear.
;



EchoChkSum
	JSR	GetByte				;Get chksum from vax (low)
	STA	RxChksumlow			;Stash it
	JSR	GetByte
	STA	RxChksumhigh			;and high
	LDA	ChkSumLow			;Send ours
	CMP	RxChksumlow
	BEQ	ECS_01				;So far so good
ECS_00
	JSR	ZapChecksum			;Ready for next packet
	LDA	#5				;These values can be any-
	JSR	PrintA2				;thing but 3 & 23
	LDA	#14
	JSR	PrintA2
	SEC					;Flag error
	RTS
ECS_01
	LDA	ChkSumHigh
	CMP	RxChksumhigh
	BNE	ECS_00				;Summit wrong
	LDA	#3				;Arbitrary 'all ok' values
	JSR	PrintA2
	LDA	#23
	JSR	PrintA2
	JSR	ZapChecksum			;ready for next packet
	CLC
	RTS


;
; ZapChecksum	-	Zero the checksum counter ready for next packet
;			and bump the overall count.
;

ZapChecksum
	LDA	Cksumtotallow
	CLC
	ADC	ChkSumLow
	STA	Cksumtotallow
	LDA	Cksumtotalhigh
	ADC	ChkSumHigh
	STA	Cksumtotalhigh
	LDA	#0
	STA	ChkSumLow
	STA	ChkSumHigh
	RTS

;
; GetByte	-	Returns a byte of data received via serial card as
;			two 4-bit nibbles.
;			Also indicates whether packet hdr was received.
;			Handles '00' convention transparently.
;			
;			Returns data in A. Preserves X,Y
;			Carry SET => packet hdr received
;			carry CLR => normal data or checksum
;

GetByte
	JSR	GetSub				;Get next byte from serial
	BNE	GB_01				;!= 0, allow it
	JSR	GetSub				;Else, get next
	BEQ	GB_01				;== 0, return 0
	SEC					;=> packet hdr
	RTS					;Return packet #
GB_01
	CLC					;=> != 0
	RTS

GetSub
	JSR	GetNibble			;Get low half of the byte
	AND	#$0F				;Only interested in bot 4 bits
	PHA
	JSR	GetNibble			;Get high nibble
	ASL	A				;Shift it to its rightful place
	ASL	A
	ASL	A
	ASL	A
	STA	TempStore			;Stash it
	PLA					;Recover low nibble
	ORA	TempStore			;Combine the two
	RTS

;
; CalcChkSum	-	Keeps track of received data using a 16-bit
;			checksum.
;			Takes Acc as received data.
;

ChkSumLow		.byte	0
ChkSumHigh		.byte	0
RxChksumlow		.byte	0
RxChksumhigh		.byte	0
Cksumtotallow		.byte	0
Cksumtotalhigh		.byte	0

CalcChkSum
	CLC
	ADC	ChkSumLow
	STA	ChkSumLow
	LDA	ChkSumHigh
	ADC	#0
	STA	ChkSumHigh
	RTS


;
; WriteBuffer	-	Writes data buffer to disk in contiguous
;			sector-by-sector fashion.
;
;			Returns carry set if end of disk written
;			Else, carry clear.
;

MaxTrack	=	35			;(0-34)
MaxSector	=	16			;(0-15)


WriteBuffer

	LDY	#IOB%256
	LDA	#IOB/256
	JSR	RWTS
	BCC	WRS_00				;=> All Okay
	JSR	DiskDone			;End of disk?
	BCS	WRS_02				;=> yep
;	LDA	ERROR
;	JSR	ReportError			;A forthcoming attraction
	JMP	WriteBuffer			;Try again
WRS_00
	INC	SECTOR				;Bump sector #
	LDA	SECTOR
	CMP	#MaxSector			;Need to bump track # ?
	BCC	WRS_01				;=> not yet
	AND	#$0F
	STA	SECTOR				;Zero sector #
	INC	TRACK				;and bump track #
WRS_01
	INC	PAGENO
	LDA	PAGENO
	CMP	#MaxPage+1			;Written it all?
	BCC	WriteBuffer			;=> nope, continue
	JSR	InitBuffers			;Re-init
	CLC
WRS_02
	RTS

DiskDone
	LDA	TRACK
	CMP	#MaxTrack
	RTS

;
;IOB - I/O block for RWTS
;

IOB
	.byte	001,$60,01

VOLUME	.byte	00

TRACK	.byte	00
SECTOR	.byte	00
	.byte	DCT%256
	.byte	DCT/256
	.byte	00		;Page # buffer
PAGENO	.byte	00		;for sector.
	.byte	00		;Unused
	.byte	00		;Unused
COMMAND
	.byte	02		;01=>Read,02=>write
ERROR	.byte	00		;Error code on exit
;010h=>Write protected,020h=>Volume mismatch
;040h=>Drive error,080h=>Read error.
ACTVOL	.byte	00		;Actual volume # found
PSLOT	.byte	$60		;Last accessed slot
PDRIVE	.byte	01		;Last accessed drive

DCT
DEVTPC	.byte	00		;Device type code
PPTC	.byte	01		;Phases/track
MONTC	.byte	$EF		;Time count
	.byte	$D8		;  "    "




;
; GetFromSerial	-	return i/p from serial line in A
;


GetFromSerial

	JMP	(KSW)				;Get i/p from stdin


XON
	LDA	#$11				;XON (^Q)
	JMP	PrintA2				;Implied RTS

XOFF
	LDA	#$13				;XOFF (^S)
	JMP	PrintA2				;Implied RTS


;
;PrintA - Send contents of A to serial card
;

PrintA

	ORA	#$80				;High bit - It's an Apple
PrintA2
	JMP	(CSW)				;To prove summit's happenin'




;
; GetNibble	-	Receive a nibble of data from serial line.
;			Returns result in A
;

SSCError	=	$678+SlotNumber		;Where card stashes errors
SOUND		=	$C030

GetNibble
	JSR	GetFromSerial
	AND	#$7F				;Mask out top bit
	CMP	#$7F				;$7F => EOF
	BNE	GNB_02
	JSR	WriteBuffer			;flush
	JSR	CHECKIS				;"Checksum is "
	LDA	Cksumtotalhigh
	JSR	HEXBYTE
	LDA	Cksumtotallow
	JSR	HEXBYTE
	LDA	#CR
	JSR	CHROUT
	JMP	ExitClean			;Exit to monitor
GNB_02
	RTS



Stack	.byte	0				;Stack level at entry



;
; InitSSC	-	Set up 'Super Serial Card' as we want it
;			NB Assumes SSC DIP switches are in certain
;			positions, Baud rate etc
;

InitSSC
	LDY	#0
ISR_01
	LDA	InitData,Y
	PHP
	AND	#$7F				;No point really
	JSR	PrintA
	INY
	PLP
	BPL	ISR_01				;=> More to do
	RTS


InitData
	.byte	$0D
	.byte	$04
	.byte	"PR#2"
	.byte	$0D
	.byte	$04				;EOT
	.byte	"IN#2"
	.byte	$0D

;	.byte	""
;	.byte	"12B"				;4800 Baud!
;	.byte	$0D			

;	.byte	""				;8 bits data 1 stop
;	.byte	"0D"
;	.byte	$0D
;
;	.byte	""
;	.byte	"0P"				;No parity bits
;	.byte	$0D

	.byte	""				;Disable screen echo
	.byte	"E D"
	.byte	$0D
	.byte	""				;Set 'raw' mode
	.byte	"Z"
	.byte	$8D				;Hi-bit set CR

ExitData
	.byte	$0D
	.byte	$04
	.byte	"PR#0"
	.byte	$0D
	.byte	$04
	.byte	"IN#0"
	.byte	$8D
	

InitBuffers
	LDA	#Buffer%256			;Buffer Address
	STA	Pointer				;Set up Z-page ptr
	STA	PAGENO-1			;Tell IOB too
	LDA	#Buffer/256
	STA	Pointer+1
	STA	PAGENO
	RTS

InitIOB
	LDA	#WRITE
	STA	COMMAND				;Set to write mode
	LDA	#0				;Zero Track # etc
	STA	TRACK
	STA	SECTOR
	STA	VOLUME				;Match any volume
	RTS




;
;CHROUT - Print char in A
;

CHR_X		.byte	0
CHR_Y		.byte	0

CHROUT
	STX	CHR_X
	STY	CHR_Y
	JSR	CASEUP	
	ORA	#$80
	JSR	FILTER
;	JSR	TOSCREEN
	JSR	COUT1
	LDX	CHR_X
	LDY	CHR_Y
	RTS

;Convert lower to upper case
CASEUP
	CMP	#'a'
	BCC	CU_01
	CMP	#$7b		;z+1
	BCS	CU_01
	AND	#$5F
CU_01
	RTS

FILTER
	CMP	#$A0			;' '|$80
	BCC	FIL_01
FIL_00
	RTS
FIL_01
	CMP	#$88
	BEQ	FIL_00
	CMP	#$89
	BEQ	FIL_00
	CMP	#$8D
	BEQ	FIL_00
	LDA	#$80
	RTS


;
; HEXBYTE - OUTPUTS character in A as a Hex byte.
;

HEXBYTE
	PHA
	LSR	A
	LSR	A
	LSR	A
	LSR	A
	JSR	HEXNIBL
	PLA
;  fall through into....

;
; Hexnibl outputs a single character 0-9, 'A' to 'F'
;
HEXNIBL
	AND	#$F
	CLC
	ADC	#'0'		; Make it a decimal char
	CMP	#$3A		;'9'+1		; is it too big (a-f)
	BMI	HEXOK
	CLC
	ADC	#$7		;'A'-'9'-1	; Make it a sensible alpha char.
HEXOK	JMP	CHROUT



RECPACK
	LDY	#0
RP_01
	LDA	RECDATA,Y
	PHP
	JSR	CHROUT
	INY
	PLP
	BPL	RP_01
	RTS

RECDATA	.byte	"RX PACKET "
	.byte	$a0

Attempt		.byte	1

TRYNUM
	LDY	#0
TN_01
	LDA	TRYDATA,Y
	PHP
	JSR	CHROUT
	INY
	PLP
	BPL	TN_01
	RTS

TRYDATA	.byte	" TRY"
	.byte	$a0

CHECKIS
	LDY	#0
CI_01
	LDA	CHECKDATA,Y
	PHP
	JSR	CHROUT
	INY
	PLP
	BPL	CI_01
	RTS

CHECKDATA
	.byte	"CHECKSUM IS"
	.byte	$a0
