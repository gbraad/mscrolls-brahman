***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************


	SECTION	"ADV",code

	include	"nounequ1.i"

ICON	MACRO	* item id, resource name
	DC.W	N\1
	DC.B	'\2',0
	CNOP	0,10
	ENDM


	XDEF	ItemIcons

ItemIcons


	DC.L	0			;terminate with noun # of zero

	END

* produced on Tue Jun 12 09:38:16 2018

* checksum = 7ce2b788
