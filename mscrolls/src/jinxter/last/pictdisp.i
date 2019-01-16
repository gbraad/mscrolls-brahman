** an include file that details the best methods of displaying the pictures.
	XDEF	DITHERDISP,BESTDISP

	ALIGN
DITHERDISP:
	DC.W	$D1
	DC.W	$D8
	DC.W	$D0
	DC.W	$B0
	DC.W	$92
	DC.W	$E0
	DC.W	$C0
	DC.W	$E0
	DC.W	$B2
	DC.W	$B2
	DC.W	$E0
	DC.W	$E0
	DC.W	$D2
	DC.W	$90
	DC.W	$D0
	DC.W	$C0
	DC.W	$D1
	DC.W	$B9
	DC.W	$B0
	DC.W	$A4
	DC.W	$A4
	DC.W	$C0
	DC.W	$B0
	DC.W	$B0
	DC.W	$C0
	DC.W	$E0
	DC.W	$D0
	DC.W	$90
	DC.W	$B0

BESTDISP
* on getting a picture the best method of displaying it...
* 0 = stip 1 = dith
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	1
	DC.B	0
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
	DC.B	0
	DC.B	0
	DC.B	0
	DC.B	0
