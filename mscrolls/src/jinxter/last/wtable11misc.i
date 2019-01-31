******************************************************
*
*	@(#)wtable11misc.i	1.2
*	8/21/87
*
******************************************************

*
*  SECTION #11
*

	DC.B   $6E+$80
	DC.B   'n',$65+$80
	DC.B   $65+$80
	DC.B   's',$65+$80
	DC.B   $73+$80
	DC.B   's',$77+$80
	DC.B   $77+$80
	DC.B   'n',$77+$80
	DC.B   $75+$80
	DC.B   $64+$80
	DC.B   $69+$80
	DC.B   $6C+$80
	DC.B   'usin',$67+$80
	DC.B   'bu',$74+$80
	DC.B   $47+$80
	DC.B   'D',$52+$80
	DC.B   'INSID',$45+$80
	DC.B   $4F+$80
	DC.B   'SHU',$54+$80
	DC.B   $52+$80
	DC.B   $46+$80
	DC.B   'W',$49+$80
	DC.B   'ACCEP',$54+$80
	DC.B   'OU',$54+$80
	DC.B	'coul',$64+$80
	DC.B   'D',$4F+$80
	DC.B   'DOE',$53+$80
	DC.B   'DI',$44+$80
	DC.B   'MIGH',$54+$80
	DC.B   'WOUL',$44+$80
	DC.B   'SHOUL',$44+$80
*	DC.B   'REPAI',$52+$80
	DC.B   'YE',$41+$80
	DC.B   'YEA',$48+$80
	DC.B   'NOP',$45+$80
*	DC.B   'BEHIN',$44+$80
	DC.B   'IN',$56+$80
	DC.B   'ARE',$4E+$80
	DC.B   'IS',$4E+$80
	DC.B   'LEA',$50+$80
	DC.B   'LAUGHIN',$47+$80
	DC.B   'INVEN',$54+$80
	DC.B   'GOIN',$47+$80
	DC.B   'WEARIN',$47+$80
*	DC.B   'MERG',$45+$80
	DC.B   'BELO',$57+$80
	DC.B   'ATTAC',$48+$80
*	DC.B   'BLEN',$44+$80
	DC.B   'ROUN',$44+$80            ;PEOPLE R LAZY
	DC.B   'SNIF',$46+$80            ;=SMELL

* included by wtable7.asm

    IFD	wantNUMBERS

	DC.B	$30+$80
	DC.B	$31+$80
	DC.B	$32+$80
	DC.B	$33+$80
	DC.B	$34+$80
	DC.B	$35+$80
	DC.B	$36+$80
	DC.B	$37+$80
	DC.B	$38+$80
	DC.B	$39+$80

    ENDC	;wantnumbers

	DC.B	'upside_dow',$6e+$80
	DC.B	'wossnam',$45+$80		;it
	DC.B	'wossit',$73+$80		;them
	DC.B	'wossernam',$45+$80		;her
	DC.B	'wossisnam',$45+$80		;him
	DC.B	'ir',$74+$80			;it
