
;#########################################################################
;#
;#	@(#)rwtsboot.asm	1.2
;#	6/25/88
;#
;#########################################################################



;****************************************
;
;* Apple RWTS routine			*
;
;* Reads and writes tracks & sectors	*
;
;* Used by boot routine to load rest	*
;* of code				*
;
;****************************************


	*=	$B800

;PRENIBBLE
;Converts 256 'real' bytes to 342 6 bit
;nibbles of the form 00xxxxxx.
;(Not as put on disk).
;ON ENTRY: $3E,$3F point to user data.

BEGINRWTS


HB800          LDX  #$00

               LDY  #$02

HB804          DEY

               LDA  ($3E),Y	;Points to data to be written

               LSR	A

               ROL  HBC00,X

               LSR	A

               ROL  HBC00,X

               STA  HBB00,Y

               INX

               CPX  #$56

               BCC  HB804

               LDX  #$00

               TYA

               BNE  HB804

               LDX  #$55

HB81E          LDA  HBC00,X

               AND  #$3F

               STA  HBC00,X

               DEX

               BPL  HB81E

               RTS



;WRITE routine - Writes prenibbilized data

;from primary & secondary buffers to disk.

;Really writes data on to the disk!

;ON ENTRY: X=slot No.*16

;NB: does it NOW!



HB82A          SEC

               STX  $27

               STX  H0678

               LDA  HC08D,X

               LDA  HC08E,X

               BMI  HB8B4

               LDA  HBC00

               STA  $26

               LDA  #$FF	;Sync byte (Was $AB)

               STA  HC08F,X

               ORA  HC08C,X

               PHA

               PLA

               NOP

               LDY  #$04

PB84A		PHA

		PLA

		JSR  WBYTE+1

               DEY

               BNE  PB84A

               LDA  #$D5	;Data preamble

               JSR  HB8B8

               LDA  #$AA	; "     "

               JSR  HB8B8

               LDA  #$AD	; "     "

               JSR  HB8B8

               TYA

               LDY  #$56

               BNE  HB869

HB866          LDA  HBC00,Y

HB869          EOR  NBUF2-1,Y		;HBBFF,Y

               TAX

               LDA  HBA29,X	;Using write translate table

               LDX  $27

               STA  HC08D,X

               LDA  HC08C,X

               DEY

               BNE  HB866

               LDA  $26

               NOP

HB87E          EOR  HBB00,Y

               TAX

               LDA  HBA29,X	;Using write translate table

               LDX  H0678

               STA  HC08D,X

               LDA  HC08C,X

               LDA  HBB00,Y

               INY

               BNE  HB87E

               TAX

               LDA  HBA29,X

               LDX  $27

               JSR  HB8BB

PB89D          LDA  #$DE	;Closing data marks (was $FF)

               JSR  HB8B8

PB8A2          LDA  #$AA	;   "      "(Was $FF)

               JSR  HB8B8

               LDA  #$EB	;   "      "(Was $FF)

               JSR  HB8B8

               LDA  #$FF

               JSR  HB8B8

               LDA  HC08E,X

HB8B4          LDA  HC08C,X

               RTS





;Write a byte

;NB 32 cycles/byte!

WBYTE

HB8B8          CLC

               PHA

               PLA

HB8BB          STA  HC08D,X

               ORA  HC08C,X

               RTS



;POSTNIBBLE - converts 342 6-bit nibbles

;to 256 8-bit bytes.

;$3E,$3F point to user data buffer.

;ON ENTRY: X=slot No.*16

;ON EXIT : Y=Byte count in secondary buffer

HB8C2          LDY  #$00

HB8C4          LDX  #$56

HB8C6          DEX

               BMI  HB8C4

               LDA  HBB00,Y

               LSR  HBC00,X

               ROL	A

               LSR  HBC00,X

               ROL	A

               STA  ($3E),Y

               INY

               CPY  $26

               BNE  HB8C6

               RTS



;Read routine - Reads DATA field NOW!

;Reads a sector and puts it in primary

;and secondary buffers.

;ON ENTRY: X=SLOT No.*16



HB8DC          LDY  #$20

HB8DE          DEY

               BEQ  HB942

HB8E1          LDA  HC08C,X

               BPL  HB8E1

HB8E6          EOR  #$D5	;Preamble

               BNE  HB8DE

               NOP

HB8EB          LDA  HC08C,X

               BPL  HB8EB

               CMP  #$AA	;Preamble

               BNE  HB8E6

               LDY  #$56

HB8F6          LDA  HC08C,X

               BPL  HB8F6

               CMP  #$AD	;Preamble

               BNE  HB8E6

               LDA  #$00

HB901          DEY

               STY  $26

HB904          LDY  HC08C,X

               BPL  HB904

		EOR  RTRANS-$96,Y

               LDY  $26

               STA  HBC00,Y

               BNE  HB901

HB913          STY  $26

HB915          LDY  HC08C,X

               BPL  HB915

		EOR  RTRANS-$96,Y

               LDY  $26

               STA  HBB00,Y

               INY

               BNE  HB913

HB925          LDY  HC08C,X

               BPL  HB925

		CMP  RTRANS-$96,Y

               BNE  HB942

HB92F          LDA  HC08C,X

               BPL  HB92F

PB934          CMP  #$DE	;Closing marks (Was $FF)

               BNE  HB942

               NOP

HB939          LDA  HC08C,X

               BPL  HB939

PB93E          CMP  #$AA	;Closing marks (Was $FF)

               BEQ  HB99E

HB942          SEC

               RTS



;Read an Address Field

;ON ENTRY: X = slot No.*16

;ON EXIT :$2F=Vol # found,$2E=Track # found,

;	   $2D=Sector # found,$2C=Checksum found

RDADR

HB944          LDY  #$FC

               STY  $26

HB948          INY

               BNE  HB94F

               INC  $26

               BEQ  HB942

HB94F          LDA  HC08C,X

               BPL  HB94F

HB954          CMP  #$D5	;Address preamble

               BNE  HB948

               NOP

HB959          LDA  HC08C,X

               BPL  HB959

               CMP  #$AA	;Address preamble

               BNE  HB954

               LDY  #$03

HB964          LDA  HC08C,X

               BPL  HB964

               CMP  #$96	;Address preamble

               BNE  HB954

               LDA  #$00

HB96F          STA  $27

HB971          LDA  HC08C,X

               BPL  HB971

               ROL	A

               STA  $26

HB979          LDA  HC08C,X

               BPL  HB979

               AND  $26

               STA  H002C,Y

               EOR  $27

               DEY

               BPL  HB96F

               TAY

               BNE  HB942

HB98B          LDA  HC08C,X

               BPL  HB98B

PB990          CMP  #$DE	;End of Address Field marks (Was $FF)

               BNE  HB942

               NOP

HB995          LDA  HC08C,X

               BPL  HB995

PB99A          CMP  #$AA	;End of Address field marks (Was $FF)

               BNE  HB942

HB99E          CLC

               RTS

;SEEKABS - Move disk arm to desired track

;ON ENTRY: X=Slot No.*16

;	    A=Desired track(halftrack for single phase disk)

;	    $478=Current track

;ON EXIT : $2A and $478=Final track

;	    $27=Prior track (if seek needed)



SEEKABS

HB9A0

		JSR  DAEMON	;Go check keyboard

	        STX  $2B

               STA  $2A

               CMP  H0478

               BEQ  HB9FC

               LDA  #$00

               STA  $26

HB9AD          LDA  H0478

               STA  $27

               SEC

               SBC  $2A

               BEQ  HB9EA

               BCS  HB9C0

               EOR  #$FF

               INC  H0478

               BCC  HB9C5

HB9C0          ADC  #$FE

               DEC  H0478

HB9C5          CMP  $26

               BCC  HB9CB

               LDA  $26

HB9CB          CMP  #$0C

               BCS  HB9D0

               TAY

HB9D0          SEC

               JSR  HB9EE

               LDA  HBA11,Y

               JSR  HBA00

               LDA  $27

               CLC

               JSR  HB9F1

               LDA  HBA1D,Y

               JSR  HBA00

               INC  $26

               BNE  HB9AD

HB9EA          JSR  HBA00

               CLC

HB9EE          LDA  H0478

HB9F1          AND  #$03

               ROL	A

               ORA  $2B

               TAX

               LDA  HC080,X

               LDX  $2B

HB9FC          RTS



;Arm move delay subroutine

;delays a specified No.of Usec intervals

;ON ENTRY: A=No.of 100Usec intervals

;	    $46,$47 =Motor on time count

;	    (from device characteristics table($EF,$D8))

;	    $478=Current track



HBA00

		JSR	DMON

		RTS

;		DS	$0D

		.byte	0,0,0,0,0,0,0,0,0,0,0,0,0



;Arm move delay table



HBA11          DFB  $1,$30,$28,$24,$20

		DFB  $1E,$1D

               DFB  $1C

               DFB  $1C

               DFB  $1C

               DFB  $1C

		DFB  $1C

HBA1D		DFB  $70

PBA1E		DFB  $2C,$26

		DFB  $22

               DFB  $1F

		DFB  $1E,$1D

               DFB  $1C

               DFB  $1C

               DFB  $1C

               DFB  $1C

		DFB  $1C



;Write translate table

HBA29          DFB  $96,$97,$9A,$9B,$9D,$9E,$9F

		DFB  $A6,$A7,$AB,$AC,$AD,$AE,$AF,$B2

		DFB  $B3,$B4,$B5,$B6,$B7,$B9,$BA,$BB

		DFB  $BC,$BD,$BE,$BF,$CB,$CD,$CE,$CF

		DFB  $D3,$D6,$D7

HBA4B		DFB  $D9,$DA,$DB,$DC,$DD

		DFB  $DE,$DF,$E5,$E6,$E7,$E9,$EA,$EB

		DFB  $EC,$ED,$EE,$EF,$F2,$F3,$F4,$F5

		DFB  $F6,$F7,$F9,$FA,$FB,$FC,$FD,$FE

		DFB  $FF



;		DS	$2D

		.byte	0,0,0,0,0,0,0,0,0,0
		.byte	0,0,0,0,0,0,0,0,0,0
		.byte	0,0,0,0,0,0,0,0,0,0
		.byte	0,0,0,0,0,0,0,0,0,0
		.byte	0,0,0,0,0



;Read translate table

;HBA96

RTRANS

	DFB  $0,$1,$98,$99,$02,$3,$9C,$4,$5,$6

	DFB  $A0,$A1,$A2,$A3,$A4,$A5,$7,$8

	DFB  $A8,$A9,$AA,$09,$0A,$B,$C,$D

	DFB  $B0,$B1,$0E,$0F,$10,$11,$12,$13

	DFB  $B8,$14,$15,$16,$17,$18,$19,$1A

	DFB  $C0,$C1,$C2,$C3,$C4,$C5,$C6,$C7

	DFB  $C8,$C9,$CA,$1B,$CC,$1C,$1D,$1E

	DFB  $D0,$D1,$D2,$1F,$D4,$D5,$20,$21

	DFB  $D8,$22,$23,$24,$25,$26,$27,$28

	DFB  $E0,$E1,$E2

HBAE3	DFB  $E3,$E4,$29,$2A,$2B

	DFB  $E8,$2C,$2D,$2E,$2F,$30,$31,$32

	DFB  $F0,$F1,$33,$34,$35,$36,$37,$38

	DFB  $F8,$39,$3A,$3B,$3C,$3D,$3E,$3F



HBB00

NBUF1	
;		DS	$100
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0



HBC00

NBUF2
;		DS	$56

		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0



;Write address field during init

;Writes No. of autosync bytes contained in Y

;ON ENTRY: X=Slot No.*16

;	    Y=No.of autosync to write

;	    $3E=$AA,$3F=Sector No.

;	    $41=Volume No.,$44=Track No.





HBC56          SEC

               LDA  HC08D,X

               LDA  HC08E,X

               BMI  HBCBD

               LDA  #$FF	;Was $FE

               STA  HC08F,X

               CMP  HC08C,X

               PHA

               PLA

HBC69          JSR  HBCC3

               JSR  HBCC3

               STA  HC08D,X

               CMP  HC08C,X

               NOP

               DEY

               BNE  HBC69

               LDA  #$D5

               JSR  HBCD5

               LDA  #$AA

               JSR  HBCD5

               LDA  #$96

               JSR  HBCD5

               LDA  $41	;Volume

               JSR  HBCC4

               LDA  $44	;Track

               JSR  HBCC4

               LDA  $3F	;Sector

               JSR  HBCC4

               LDA  $41	;Calc checksum

               EOR  $44	;

               EOR  $3F	;

               PHA

               LSR	A

               ORA  $3E	;$3E MUST contain $AA

				;(Timing critical!!).

               STA  HC08D,X

               LDA  HC08C,X

               PLA

               ORA  #$AA

               JSR  HBCD4

PBCAD          LDA  #$DE

               JSR  HBCD5

PBCB2          LDA  #$AA

               JSR  HBCD5

               LDA  #$EB

               JSR  HBCD5

               CLC

HBCBD          LDA  HC08E,X

               LDA  HC08C,X

HBCC3          RTS



;Write double byte subroutine

;(Writes a byte as 2 nibbles in address field.)

;Encodes address information into even

;and odd bits and writes it at 32 cycle

;intervals.

;NB Timing critical!!

HBCC4          PHA

               LSR	A

               ORA  $3E

               STA  HC08D,X

               CMP  HC08C,X

               PLA

               NOP

               NOP

               NOP

               ORA  #$AA

HBCD4          NOP

HBCD5          NOP

               PHA

               PLA

               STA  HC08D,X

               CMP  HC08C,X

               RTS



;	DS	$21

		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0




;Main entry point to RWTS

;ON ENTRY: A & Y point to IOB

RWTS_ENTRY

               STY  $48

               STA  $49

               LDY  #$02

               STY  H06F8

               LDY  #$04

               STY  H04F8

               LDY  #$01

               LDA  ($48),Y

               TAX

               LDY  #$0F

               CMP  ($48),Y

               BEQ  HBD34

               TXA

               PHA

               LDA  ($48),Y

               TAX

               PLA

               PHA

               STA  ($48),Y

               LDA  HC08E,X

HBD25          LDY  #$08

               LDA  HC08C,X

HBD2A          CMP  HC08C,X

               BNE  HBD25

               DEY

               BNE  HBD2A

               PLA

               TAX

;SAMESLOT - Enter read mode and read

;with delays to see if disk is spinning.

;Saves result of test and turns motor

;on just in case.

HBD34          LDA  HC08E,X

               LDA  HC08C,X

               LDY  #$08

HBD3C          LDA  HC08C,X

               PHA

               PLA

               PHA

               PLA

               STX  H05F8

               CMP  HC08C,X

               BNE  HBD4E

               DEY

               BNE  HBD3C

HBD4E          PHP

               LDA  HC089,X

               LDY  #$06



HBD54          LDA  ($48),Y

               STA  CSWL,Y

               INY

               CPY  #$0A

               BNE  HBD54

               LDY  #$03

               LDA  ($3C),Y

               STA  $47	;Motor on time (from IOB)

               LDY  #$02

               LDA  ($48),Y

               LDY  #$10

               CMP  ($48),Y

               BEQ  HBD74

               STA  ($48),Y

               PLP

               LDY  #$00

               PHP

HBD74          ROR	A

               BCC  HBD7C

               LDA  HC08A,X

               BCS  HBD7F

HBD7C          LDA  HC08B,X

HBD7F          ROR  $35

               PLP

               PHP

               BNE  HBD90

               LDY  #$07

HBD87          JSR  HBA00

               DEY

               BNE  HBD87

               LDX  H05F8

;Get destination track and go there

;using MYSEEK

;(also,delays for motor to come up to

;speed).

HBD90          LDY  #$04

               LDA  ($48),Y

               JSR  HBE5A

               PLP

               BNE  HBDAB

               LDY  $47

               BPL  HBDAB



;Motor on time delay

HBD9E		LDA  #$01

		JSR  DMN_00

               INC  $46

               BNE  HBD9E

               INC  $47

               BNE  HBD9E



;Get command code (read,write,init or whatever)

TRYTRK

HBDAB          LDY  #$0C

               LDA  ($48),Y

               BEQ  HBE0B

               CMP  #$04

               BEQ  HBE0D

               ROR	A

               PHP

               BCS  HBDBC

               JSR  HB800



;Initialise retries at 48 and read address field

HBDBC          LDY  #$30

               STY  H0578

HBDC1          LDX  H05F8

               JSR  HB944

               BCC  HBDED

HBDC9          DEC  H0578

               BPL  HBDC1

HBDCE          LDA  H0478

               PHA

               LDA  #$60

               JSR  HBE95

               DEC  H06F8

               BEQ  HBE04

               LDA  #$04

               STA  H04F8

               LDA  #$00

               JSR  HBE5A

               PLA

HBDE7          JSR  HBE5A

               JMP  HBDBC



;Verify that we're on the right track

RDRIGHT

HBDED          LDY  $2E

               CPY  H0478

               BEQ  HBE10

               LDA  H0478

               PHA

               TYA

               JSR  HBE95

               PLA

               DEC  H04F8

               BNE  HBDE7

               BEQ  HBDCE



;Clean up stack & status reg

DRVERR

HBE04          PLA

               LDA  #$40

HBE07          PLP

               JMP  PBE47+1



;Branch to ALLDONE

HBE0B          BEQ  HBE46



;FORMDSK

HBE0D



;Check volume No.found against vol No.wanted

;If vol 0 specified then map to any No.

RTTRK

HBE10          LDY  #$03

               LDA  ($48),Y

               PHA

               LDA  $2F

               LDY  #$0E

               STA  ($48),Y

               PLA

               BEQ  HBE26

               CMP  $2F

               BEQ  HBE26

               LDA  #$20

               BNE  HBE07



;Check to see if sector is correct

;Uses ILEAV table for s/w sector interleaving

;If wrong sector tries again.

CRCTVOL

HBE26          LDY  #$05

               LDA  ($48),Y

               TAY

               LDA  HBFB8,Y

               CMP  $2D

               BNE  HBDC9

               PLP

               BCC  HBE51

               JSR  HB8DC

               PHP

               BCS  HBDC9

               PLP

               LDX  #$00

               STX  $26

               JSR  HB8C2

               LDX  H05F8





;Skip over SEC in HNDLERR (HNDLERR starts

;at $38 in BIT instruction!($38=SEC))

;HNDLERR - sets carry(flags error),

;stores A in IOB as return code,turns

;off motor.

ALLDONE

HBE46          CLC

PBE47          BIT  $38

               LDY  #$0D

               STA  ($48),Y

               LDA  HC088,X

               RTS

;Write a sector using WRITE16

WRITE

HBE51          JSR  HB82A	;WRITE16

               BCC  HBE46

               LDA  #$10

               BCS  PBE47+1



;Housekeeping before going to SEEKABS

MYSEEK

HBE5A          PHA

               LDY  #$01

               LDA  ($3C),Y

               ROR	A

               PLA

               BCC  HBE6B

               ASL	A

               JSR  HBE6B

               LSR  H0478

               RTS

HBE6B          STA  $2A

               JSR  HBE8E

               LDA  H0478,Y

               BIT  $35

               BMI  HBE7A

               LDA  H04F8,Y

HBE7A          STA  H0478

               LDA  $2A

               BIT  $35

               BMI  HBE88

               STA  H04F8,Y

               BPL  HBE8B

HBE88          STA  H0478,Y

HBE8B          JMP  HB9A0





;Put slot No.in Y from X/16

XTOY

HBE8E          TXA

               LSR	A

               LSR	A

               LSR	A

               LSR	A

               TAY

               RTS



;Set track No.

HBE95          PHA

               LDY  #$02

               LDA  ($48),Y	;Points at IOB

               ROR	A

               ROR  $35

               JSR  HBE8E

               PLA

               ASL	A

               BIT  $35

               BMI  HBEAB

               STA  H04F8,Y

               BPL  HBEAE

HBEAB          STA  H0478,Y

HBEAE          RTS



;Sector translate table.

;Sector interleaving done with s/w

HBFB8

		DFB  $00,$0D,$0B,$09,$07,$05,$03,$01

		DFB  $0E,$0C,$0A,$08,$06,$04,$02,$0F







H002C          EQU    $002C

CSWL           EQU    $0036

H0302          EQU    $0302

H0478          EQU    $0478

H04F8          EQU    $04F8

H0578          EQU    $0578

H05F8          EQU    $05F8

H0678          EQU    $0678

H06F8          EQU    $06F8

HC01A          EQU    $C01A

HC080          EQU    $C080

HC081          EQU    $C081

HC088          EQU    $C088

HC089          EQU    $C089

HC08A          EQU    $C08A

HC08B          EQU    $C08B

HC08C          EQU    $C08C

HC08D          EQU    $C08D

HC08E          EQU    $C08E

HC08F          EQU    $C08F

HCFCE          EQU    $CFCE





;DAEMON - Called periodically,this will

;	   check for any keypresses and

;	   buffer them for later action.



KBD		EQU	$C000

KBDSTRB	EQU	$C010

KBDBCNT	DFB	0

KBDBUF	
;		DS	$32

		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		.byte 0,0,0,0,0,0,0,0,0,0



DAEMON

	PHA

	TXA

	PHA

	LDA	KBD		;Read Keyboard

	BPL	NO_KEY

	STA	KBDSTRB

	LDX	KBDBCNT		;Get index into buffer

	STA	KBDBUF,X

	INX

	CPX	#$32		;Buffer full?

	BNE	DAM_01

	LDX	#$00

DAM_01

	STX	KBDBCNT

NO_KEY

	PLA

	TAX

	PLA

	RTS



;DM0N - Called as a substitute for the

;	 RWTS delay routines,this will

;	 simulate the desired delays

;	 and read the keyboard for

;	 type-ahead which it buffers.

;	 NB:Time critical!



DMN_00	CMP	($00),Y		;5 cycles

	CMP	($00),Y

DMON

	PHA

	LDA	KBD		;Read Keyboard

	BPL	DMN_03

	STA	KBDSTRB		;Clear strobe

	LDX	KBDBCNT

	STA	KBDBUF,X

	INX

	CPX	#$32		;Buffer full?

	BNE	DMN_01

	LDX	#$00

DMN_01

	STX	KBDBCNT

	LDX	#7		;42 usec delay

DMN_02

	DEX

	BNE	DMN_02

	PLA

	SEC

	SBC	#$01		;Again?

	BNE	DMN_00+2

	RTS

DMN_03

	LDX	#10		;60 usec delay

DMN_04

	DEX

	BNE	DMN_04

	PLA

	SEC

	SBC	#$01

	BNE	DMN_00

	NOP

	NOP

	NOP

	RTS


