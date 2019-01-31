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


		F_DEF	Copyright,823
		F_DEF	Credits,5
		F_DEF	Score1,6
		F_DEF	Score2,7
		F_DEF	Removedisk,645

		F_DEF	Tryagain,632
		F_DEF	Thanks,642
		F_DEF	Enter,638
		F_DEF	Prot1,633
		F_DEF	Prot2,634
		F_DEF	Prot3,635
		F_DEF	Prot4,636
		F_DEF	Prot5,637

		F_DEF	Poison1,9
		F_DEF	Poison2,10
		F_DEF	Poison3,11
