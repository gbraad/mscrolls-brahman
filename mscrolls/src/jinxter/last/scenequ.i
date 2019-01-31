*
* Magic scenequ.i
*

	INCLUDE	"scenequ2.i"

*MAXWGHT		EQU	34


	IFD	NOUN_EQU_INCLUDED

DEBUG_BAG	EQU	NBAG1
DEBUG_JEANS	EQU	NJEANS
DEBUG_SHIRT	EQU	NSWEATSHIRT
IT		EQU	3
it		EQU	3
them		EQU	4
her		EQU	5
him		EQU	6

	ENDC

ELIZA			EQU	FALSE

*
* NOTE - If you don't want any of the wantXYZZY things, then COMMENT THEM OUT!
* Don't set them FALSE, as they are checked for with IFD conditionals.
*

*wantGETOFFROOMisLEAVE	EQU	TRUE

wantSP.NWAY		EQU	TRUE
wantSP.CUT		EQU	TRUE
wantSP.DISMOUNT		EQU	TRUE
wantSP.SMELL		EQU	TRUE
wantSP.WAVE		EQU	TRUE
wantSP.WAIT		EQU	TRUE
wantSP.BLOW		EQU	TRUE
wantABSOLUTEALL		EQU	TRUE
wantSMELLisSINGV	EQU	TRUE
wantSMELLfromTABLE	EQU	TRUE
wantNUMBERS		EQU	TRUE
wantREADCONJ		EQU	TRUE
wantREADifCLOSED	EQU	TRUE
wantWHICHBITS		EQU	TRUE
wantRADJUNCTS		EQU	TRUE
* wantPOSSESSIVE_PRONOUNS	EQU	TRUE
wantWETROOMS		EQU	TRUE
wantLONGLOOK		EQU	TRUE
wantSP.ASKFOR		EQU	TRUE
wantSP.LOOK_After	EQU	TRUE
wantSP.FIXTO_exceptions	EQU	TRUE
wantSP.FIXTOGETHER_exceptions	EQU	TRUE
wantSP.FIX_exceptions	EQU	TRUE
wantSP.FIX_After	EQU	TRUE
wantMULTIPLES		EQU	TRUE
wantATHING		EQU	TRUE
wantSP.STAND_Before	EQU	TRUE
*	wantSTATES		EQU	TRUE
wantDRYOUT		EQU	TRUE
wantTIMERSAFTER		EQU	TRUE
wantSP.UNWEA_BEFORE	EQU	TRUE
wantRUN			EQU	TRUE
wantFAST_GETWORD	EQU	TRUE
wantSP.STAND		EQU	TRUE
wantSP.UNLOCK_Before	EQU	TRUE
wantSP.WAKE		EQU	TRUE
wantSP.PUTUNDER		EQU	TRUE
wantSP.UNTIE_After	EQU	TRUE
wantSP.OPEN_After	EQU	TRUE

wantGLOBALIO		EQU	TRUE	;global indirect objects
	IFEQ	M68000
wantBIGPROG		EQU	TRUE	; Separate word table.
	ENDC
wantNoChange		EQU	TRUE	;BUY doesn't give change now
wantGO_CONTEXT		EQU	TRUE	;no go x, which x, the x or the x hassle please!
wantSLOW_ROOTNUM	EQU	TRUE	;so synonyms of root word get counted
wantSP.CAST_Before	EQU	TRUE	;catch cast early so singv's work
wantLIGHT_RESTRICTIONS	EQU	TRUE	;light <x in soft || combust obj>
wantSP.LIGHT_After	EQU	TRUE
wantLISTENtoROOM	EQU	TRUE
wantNOQUOTES		EQU	TRUE
wantNOIsNextNoun	EQU	TRUE
wantSP.CLOSE_After	EQU	TRUE
wantSP.WEAR_Before	EQU	TRUE
;wantSP.WEIRDATTIRE	EQU	TRUE
wantSP.P.Refresh	EQU	TRUE
wantAdjacentRooms	EQU	TRUE	;thing lookable next door
;wantSP.TAKE		EQU	TRUE
wantNewSpaceAtSpace	EQU	TRUE
wantEmptyOnto		EQU	TRUE
wantEmptyInto		EQU	TRUE
wantSP.DROP_Before	EQU	TRUE
wantAutomaticWait	EQU	TRUE
wantPLURAL_Override	EQU	TRUE
wantSP.LEAN_After	EQU	TRUE
*	wantSP.P.Description	EQU	TRUE
wantTurnUpsideDown	EQU	TRUE
wantSP.WHICHONE		EQU	TRUE
wantNONPCPAUSE		EQU	TRUE
wantNOCONVERSATION	EQU	TRUE
wantNOTIME		EQU	TRUE
wantSCENDOES		EQU	TRUE	;like SETIN
wantFINDnotGLOBAL   EQU TRUE

*------------------------------------------
*
*
*	CODE VERSIONS
*
*
*------------------------------------------

Version2_V.MELT		EQU	TRUE
Version2_THROW		EQU	TRUE
Version2_DO.FIX		EQU	TRUE
Version2_V.SWIM		EQU	TRUE
Version2_ROOTNUM	EQU	TRUE
Version1_CombineD0D5	EQU	TRUE 
Version2_FNDEND		EQU	TRUE		;only if V2_rootnum
Version2_FIXSYS		EQU	TRUE
Version2_HINTS		EQU	TRUE
Version2_V.PULL		EQU	TRUE
Version2_NoCanReach	EQU	TRUE
Version2_V.HIDE		EQU	TRUE

*------------------------------------------
*
*	Supported Features
*
*------------------------------------------

* Supported features are those conditions that have to be
* taken into account when certain data configurations are 
* allowed in the current scenario.
*
* eg.
*	Support_NakedFlames		;candles, burning etc.
*	Support_TiedObjects		;connected systems dragging about etc.
*	Support_Fluids			;in containers, permeable etc.
*	Support_Wetting			;objects flooding, drying out etc.
*
*	AND MANY MORE (tobe determined)

Support_NakedFlames	EQU	TRUE
