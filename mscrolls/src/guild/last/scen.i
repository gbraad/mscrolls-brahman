**********************************************************************
*
*	%W%
*	%G%
*
**********************************************************************

* scen.i - macro and equates for fixed scenario messages
*          such as protection msgs+score/copyright/credits

FIXED_SCN	MACRO	* use
		SCN	FS_\1
		ENDM

F_DEF		MACRO	* label,value
FS_\1		EQU	\2
		ENDM


		F_DEF	Copyright,266
		F_DEF	Credits,267
		F_DEF	Score1,268
		F_DEF	Score2,269
		F_DEF	Removedisk,270

		F_DEF	Tryagain,271
		F_DEF	Thanks,272
		F_DEF	Enter,273
		F_DEF	Prot1,274
		F_DEF	Prot2,275
		F_DEF	Prot3,276
		F_DEF	Prot4,277
		F_DEF	Prot5,278
		F_DEF	Poison1,378
		F_DEF	Poison2,379
		F_DEF	Poison3,380
