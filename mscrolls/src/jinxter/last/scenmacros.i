		IFND	Have_included_scenmacros.i

Have_included_scenmacros.i	EQU	1234

*
* Scenario dependant macros
*

ESCAPE		MACRO	*Event name
		BeSure	LuckyB
		CALL	LuckyB
		XREF	E\1
		DC.W	E\1
		ENDM


		ENDC
