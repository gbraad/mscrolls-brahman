** an include file that details the best methods of displaying the pictures.
	XDEF	DITHERDISP,BESTDISP

	ALIGN
DITHERDISP:
	DC.W	$B2
	DC.W	$E0
	DC.W	$D0
	DC.W	$C0
	DC.W	$B8
	DC.W	$C8
	DC.W	$C8
	DC.W	$DB
	DC.W	$D0
	DC.W	$F0
	DC.W	$E0
	DC.W	$E8
	DC.W	$110
	DC.W	$C0
	DC.W	$C8
	DC.W	$E0
	DC.W	$D0
	DC.W	$C8
	DC.W	$E0
	DC.W	$B4
	DC.W	$C0
	DC.W	$B0
	DC.W	$B8
	DC.W	$E0
	DC.W	$C8
	DC.W	$C0
	DC.W	$C0
	DC.W	$D0
	DC.W	$BD		;organ room

BESTDISP
* on getting a picture the best method of displaying it...
* 0 = stip 1 = dith
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	0
