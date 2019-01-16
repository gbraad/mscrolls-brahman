***************************************************************************
*
*
*	fred23jr output from file '../jinxter'*
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

