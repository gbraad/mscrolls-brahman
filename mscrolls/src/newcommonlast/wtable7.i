******************************************************
*
*	@(#)wtable7.i	2.5
*	1/30/89
*
******************************************************

*--------------------------------
*
*   WTABLE7
*
*--------------------------------

	XDEF    L.DIREC,SynText
 
*
*  SECTION #7
*
	
L.DIREC 
 
	DC.B   'nort',$68+$80
	DC.B   'northeas',$74+$80
	DC.B   'eas',$74+$80
	DC.B   'southeas',$74+$80
	DC.B   'sout',$68+$80
	DC.B   'southwes',$74+$80
	DC.B   'wes',$74+$80
	DC.B   'northwes',$74+$80
	DC.B   'u',$70+$80
	DC.B   'dow',$6E+$80
	DC.B    $82              ;END OF SECTION

*
*
*  SECTION #8
*
*   Debugbits and DO.SING stuff go here
*
*


	DC.B   'NORMA',$4C+$80
	DC.B   'VERBOS',$45+$80
	DC.B   'BRIE',$46+$80
	DC.B   'VERSIO',$4E+$80
	DC.B   'SAV',$45+$80
	DC.B   'QUI',$54+$80
	DC.B   'SCOR',$45+$80
	DC.B   'RESTOR',$45+$80
	DC.B   'LOA',$44+$80
	DC.B   'RESTAR',$54+$80
	DC.B   'SERIA',$4C+$80
	DC.B   'I',$44+$80
	
	IFNE     YCHEAT
	 
	DC.B   'WHATI',$53+$80
	DC.B   'S',$47+$80
	DC.B   'WHEREI',$53+$80
	DC.B   'ZA',$50+$80
	DC.B   'GOT',$4F+$80
	DC.B   'TEL',$45+$80
	DC.B   'NOU',$4E+$80
	DC.B   'MS',$47+$80
	DC.B   'SC',$4E+$80
	
	ENDC
	
	DC.B   'P',$4E+$80
	DC.B   'PRONOUN',$53+$80
	
	IFNE     YCHEAT
	
	DC.B   'AD',$4A+$80
	
	ENDC

	IFNE	QL128
	DC.B   'LEF',$54+$80
	DC.B   'RIGH',$54+$80
	ENDC
	
	IFNE     YCHEAT
	
	DC.B   'VIDE',$4F+$80
	DC.B   'SUS',$4E+$80
	DC.B   'SUS',$52+$80
	DC.B   'SUSC',$52+$80
    
	ENDC
	
	IFNE	QL128
	DC.B   'CONTRAS',$54+$80
	ENDC
 
	IFNE     YCHEAT
	DC.B   'WOR',$44+$80
	DC.B   'ME',$4D+$80
	DC.B   'MO',$44+$80
	ENDC

      IFNE     AMIGA512&YCHEAT&0

	DC.B    'BPO',$4E+$80
	DC.B    'BPOF',$46+$80
	DC.B    'MIS',$43+$80

      ENDC

      IFNE    Graphics

	DC.B   'GRAPHIC',$53+$80

      IFNE   YCHEAT
	DC.B    'PICTUR',$45+$80
      ENDC

      IFNE	AMIGA512!ST520	  
	DC.B   'TEXTSIZ',$45+$80
      ENDC
	
	IFNE    Mono_Possible
	DC.B   'STIPPL',$45+$80
	DC.B   'DITHE',$52+$80
	DC.B   'FREEHAN',$44+$80
	ENDC

      ENDC

      IFNE    SpoolLen
	DC.B   'PRINTE',$52+$80
      ENDC

      IFNE    FunctionKeys
	DC.B   'FUNCTIO',$4E+$80
	DC.B   'FUNC',$54+$80
      ENDC

	DC.B    'CREDIT',$53+$80
	DC.B    'HIN',$54+$80

      IFNE     Wimp
	DC.B    'FOREGROUN',$44+$80
	DC.B    'BACKGROUN',$44+$80
	DC.B    'SCROL',$4C+$80
      ENDC

      IFNE     YCHEAT
	DC.B	'countword',$73+$80
      ENDC

      IFNE     SpeechAvail
	DC.B    'speec',$68+$80
      ENDC

      IFNE	PCW8256
       DC.B   'STIPPL',$45+$80
       DC.B   'DITHE',$52+$80
      ENDC

      IFNE	IBM
	DC.B	'TITL',$45+$80
	DC.B	'MOR',$45+$80
	DC.B	'PICTUR',$45+$80
	DC.B	'TEX',$54+$80
	DC.B	'FASTE',$52+$80
	DC.B	'SLOWE',$52+$80
      ENDC

      IFNE	YCHEAT
	DC.B	'SY',$6E+$80	;syn - list synonyms of xyzzy
	DC.B	'VNE',$54+$80	;vnet - verify network
      ENDC
       
      IFD	C64Pict

	DC.B    'PI',$43+$80
      
      ENDC
      IFNE	GENERIC
       DC.B	'UND',$6f+$80
      ENDC

      IFD	wantVerbPreFilter
       DC.B	'VER',$42+$80
      ENDC
	
	DC.B    $82              ;END OF SECTION

    IFND	wantNOQUOTES
*
*  SECTION #9
*
	DC.B   'WHER',$45+$80
	DC.B   'WH',$4F+$80
	DC.B   'WHA',$54+$80
	DC.B   'WHE',$4E+$80
	DC.B   'WH',$59+$80
	DC.B   'HO',$57+$80

    ENDC	;!wantNOQUOTES

	DC.B    $82              ;END OF SECTION

*    IFND	wantNOQUOTES
*
*  SECTION #10
*
	DC.B   'A',$4D+$80
	DC.B   'AR',$45+$80
	DC.B   'I',$53+$80
	DC.B   'WER',$45+$80
	DC.B   'WA',$53+$80

*   ENDC	;!wantNOQUOTES
	DC.B    $82              ;END OF SECTION



*************************************************************
*
*  SECTION #11 synonyms
*

SynText
	include	"wtable11misc.i"
	include "wtable11noun.i"



	DC.B    $82                      ;END OF SECTION

*************************************************************
*
*  SECTION #12 
*
	DC.B   'YO',$55+$80
	DC.B    $82              ;END OF SECTION


    IFND	wantNOQUOTES
*
*  SECTION #13
*
	DC.B   'WAN',$54+$80
	DC.B   'WIS',$48+$80
	DC.B   'LIK',$45+$80

    ENDC	;!wantNOQUOTES

	DC.B    $82              ;END OF SECTION


    IFND	wantNOQUOTES
*
*  SECTION #14
*
	DC.B   'THIN',$4B+$80
	DC.B   'THOUGH',$54+$80
	DC.B   'KNO',$57+$80
	DC.B   'KNE',$57+$80
	DC.B   'BORIN',$47+$80
	DC.B   'BORE',$44+$80
	DC.B   'SOLV',$45+$80
	DC.B   'WOR',$4B+$80
	DC.B   'UNDERSTAN',$44+$80
	DC.B   'SE',$45+$80

    ENDC	;!wantNOQUOTES

	DC.B    $82              ;END OF SECTION
*
*  SECTION #15
*
	DC.B   'NO',$54+$80
	DC.B    $82              ;END OF SECTION

*
*  SECTION #16
*
	
	XDEF	INSULT
INSULT
    IFND	wantNOQUOTES
	
	DC.B   'wall',$79+$80
	DC.B   'prat',$74+$80

    IFND	wantNoNastyInsults

	DC.B   'bastar',$64+$80
	DC.B   'pric',$6B+$80
	DC.B   'cun',$74+$80
	DC.B   'so',$64+$80
	DC.B   'shi',$74+$80
	DC.B   'wan',$6B+$80
	DC.B   'wanke',$72+$80

    ENDC	;!wantNoNastyInsults

	DC.B   'idio',$74+$80
	DC.B   'foo',$6C+$80
	DC.B   'oa',$66+$80

    IFND	wantNoNastyInsults

	DC.B   'perver',$74+$80
	DC.B   'tosse',$72+$80
	DC.B   'flowerpo',$74+$80

    ENDC	;!wantNoNastyInsults

    ENDC	;!wantNOQUOTES
	DC.B    $82              ;END OF SECTION
*
*  SECTION #17
*
	DC.B   'HELL',$4F+$80
	DC.B   'H',$49+$80
	DC.B   'HOWD',$59+$80
	DC.B   'HIY',$41+$80
	DC.B   'GREETING',$53+$80
	DC.B    $82              ;END OF SECTION

*************************************************************
*
*  SECTION #18  'sets' or multiples
*

	include	"wtable18.i"


	DC.B    $82              ;END OF SECTION

    IFND	wantNOQUOTES
*
*  SECTION #19
*
	DC.B   'MUS',$54+$80
	DC.B   'CA',$4E+$80
	DC.B   'MA',$59+$80
	DC.B   'WIL',$4C+$80
	DC.B   'SHAL',$4C+$80

    ENDC	;wantNOQUOTES
	DC.B    $82              ;END OF SECTION

    IFND	wantNOQUOTES
*
*  SECTION #20
*
	DC.B   'MUC',$48+$80
	DC.B   'MAN',$59+$80

    ENDC	;!wantNOQUOTES
	DC.B    $82              ;END OF SECTION

*
*  SECTION #21
*
	DC.B   'YE',$53+$80
	DC.B   'N',$4F+$80
	DC.B    $82              ;END OF SECTION
*
* SECTION #22
*
	DC.B   'THA',$54+$80
	DC.B   'WHIC',$48+$80
	DC.B   'WO',$54+$80
	DC.B   $82
	
*
* SECTION #23
*
	DC.B   'ON',$45+$80
	DC.B   $82


	IFD	wantPOSSESSIVE_PRONOUNS

*
* SECTION #24;	possessive pronouns
*

	DC.B	'm',$79+$80
	DC.B	'hi',$73+$80
	DC.B	'he',$72+$80
	DC.B	'thei',$72+$80

	ENDC	;wantPOSSESSIVE_PRONOUNS
	DC.B   $82


	IFD	wantWHICHBITS
*
* SECTION #25;	which one responses

	DC.B	'bot',$68+$80
	DC.B	'neithe',$72+$80
	DC.B	'non',$65+$80
	DC.B	'an',$79+$80

	ENDC	;wantWHICHBITS
	DC.B   $82

	IFD	wantSTATES

*
* SECTION #26;	state adjectives
*

	DC.B	'li',$74+$80
	DC.B	'locke',$64+$80
	DC.B	'close',$64+$80
	DC.B	'broke',$6e+$80
	DC.B	'dea',$64+$80
	DC.B	'edibl',$65+$80
	DC.B	'carrie',$64+$80
	DC.B	'smal',$6c+$80
	DC.B	'larg',$65+$80

	ENDC	;wantSTATES
	DC.B   $82

wantNumbersAsWordsORwantNUMBERS	SET	FALSE

      IFD	wantNumbersAsWords
wantNumbersAsWordsORwantNUMBERS	SET	TRUE
      ENDC	wantNumbersAsWords

      IFD	wantNUMBERS
wantNumbersAsWordsORwantNUMBERS	SET	TRUE
      ENDC	wantNUMBERS

      IFNE	wantNumbersAsWordsORwantNUMBERS
	XDEF		WTABLE.NUMBERS
WTABLE.NUMBERS

	DC.B	'zer',$6f+$80
	DC.B	'on',$65+$80
	DC.B	'tw',$6f+$80
	DC.B	'thre',$65+$80
	DC.B	'fou',$72+$80
	DC.B	'fiv',$65+$80
	DC.B	'si',$78+$80
	DC.B	'seve',$6e+$80
	DC.B	'eigh',$74+$80
	DC.B	'nin',$65+$80
	DC.B	'te',$6e+$80
	DC.B	'eleve',$6e+$80
	DC.B	'twelv',$65+$80
	DC.B	'thirtee',$6e+$80
	DC.B	'fourtee',$6e+$80
	DC.B	'fiftee',$6e+$80
	DC.B	'sixtee',$6e+$80
	DC.B	'seventee',$6e+$80
	DC.B	'eightee',$6e+$80
	DC.B	'ninetee',$6e+$80
	DC.B	'twent',$79+$80
	DC.B	'thirt',$79+$80
	DC.B	'fort',$79+$80
	DC.B	'fift',$79+$80
	DC.B	'sixt',$79+$80
	DC.B	'sevent',$79+$80
	DC.B	'eight',$79+$80
	DC.B	'nint',$79+$80
	DC.B	'hundre',$64+$80
	DC.B	'thousan',$64+$80
	DC.B	$3a+$80			;colon delimiter
	; NOTE if the position of the delimiter changes you must
	; make sure the equate 'COLON' is up to date


	ENDC	;wantNumbersAsWordsORwantNUMBERS
	DC.B	$82			;eo section


