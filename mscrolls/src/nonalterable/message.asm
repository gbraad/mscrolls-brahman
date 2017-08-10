* 
* 
*     _    __        _      __                           
*    | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
*    | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
*    | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
*    |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
*                                                        
*  Copyright (c) 2016-2017 Voidware Ltd.  All Rights Reserved.
* 
*  This file contains Original Code and/or Modifications of Original Code as
*  defined in and that are subject to the Voidware Public Source Licence version
*  1.0 (the 'Licence'). You may not use this file except in compliance with the
*  Licence or with expressly written permission from Voidware.  Please obtain a
*  copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
*  it before using this file.
* 
*  THIS IS NOT FREE SOFTWARE.
*  
*  The Original Code and all software distributed under the Licence are
*  distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
*  OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
*  WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
*  PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* 
*  Please see the Licence for the specific language governing rights and 
*  limitations under the Licence.
* 
*  contact@strandgames.com
* 
*  This software was originally written and owned By Magnetic Scrolls Ltd.
*  It is being released under a Public Source Licence for historical reference
*  and educational purposes.
*


******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

* NB!! for other include's etc see bottom of file

	include	"equates.asm"

*--------------------------------
*

MsgNum	SET	1

Define	MACRO	* message name
M.\1	EQU	MsgNum
MsgNum	SET	MsgNum+1
	XDEF	M.\1
	ENDM

Also	MACRO	* message 'synonym'
M.\1	EQU	MsgNum-1
	XDEF	M.\1
	ENDM

	Define	YCNDTT   		;   1
	Define	YSI      		;   2
	Define	UNABLE   		;   3
	Define	ITSOFT   		;   4
	Define	CGGO     		;   5
	Define	BRKS     		;   6
	Define	SNPS     		;   7
	Define	NOMURDER      		;   8
	Define	NHAP     		;   9
	Define	IDFY     		;   10
	Define	CSTFH    		;   11
	Define	BUT      		;   12
	Define	WT       		;   13
	Define	ON       		;   14
	Define	IN       		;   15
	Define	HN       		;   16
	Define	ICNDTW   		;   17
	Define	NOW      		;   18
	Define	FXTO     		;   19
	Define	DNH      		;   20
	Define	ARDY     		;   21
	Define	CLSE     		;   22
	Define	OPN      		;   23
	Define	THEVY    		;   24
	Define	NT       		;   25
	Define	DBR      		;   26
	Define	CTMA     		;   27
	Define	NEDBL    		;   28
	Define	POIS     		;   29
	Define	SICK     		;   30
	Define	ILL      		;   31
	Define	EDBL     		;   32
	Define	GOOD     		;   33
	Define	VGOOD    		;   34
	Define	MGCL     		;   35
	Define	SOME     		;   36
	Define	ANY      		;   37
	Define	NCONT    		;   38
	Define	DCONTA   		;   39
	Define	FOOL     		;   40
	Define	DEAD     		;   41
	Define	DROPD    		;   42
	Define	TAKEN    		;   43
	Define	ALCAR    		;   44
	Define	CGTW     		;   45
	Define	INW      		;   46
	Define	JMP      		;   47
	Define	VLT      		;   48
	Define	TAKOF    		;   49
	Define	CNTS     		;   50
	Define	WEARNG   		;   51
	Define	2BIG     		;   52
	Define	NEF      		;   53
	Define	LIT      		;   54
	Define	MLTS     		;   55
M.MELTS EQU	M.MLTS   		;   55
	XDEF	M.MELTS
	Define	EXP      		;   56
	Define	NLIT     		;   57
	Define	YOU      		;   58
	Define	ARE      		;   59
	Define	CNTSEE   		;   60
	Define	THERE    		;   61
	Define	HERE     		;   62
	Define	WNTLT    		;   63
	Define	ISWEAR   		;   64
	Define	SEEMS    		;   65
	Define	HNICE    		;   66
	Define	PSH      		;   67
	Define	BYH      		;   68
	Define	CNTGT    		;   69
	Define	TTR      		;   70
	Define	IN2W     		;   71
	Define	WNTF     		;   72
	Define	CAR      		;   73
	Define	MVG      		;   74
	Define	LWG      		;   75
	Define	SML      		;   76
	Define	LKE      		;   77
	Define	RHI      		;   78
	Define	YSSR     		;   79
	Define	MB       		;   80
	Define	DEF0     		;   81
	Define	RAN      		;   82
	Define	ALRDF    		;   83
	Define	DUST     		;   84
	Define	I2DARK   		;   85
	Define	NOMOUT  		;   86
	Define	JNSP     		;   87
	Define	JNCP     		;   88
	Define	DITHER	;BMSP     		;   89
	Define	PCWSTIPPLE	;OTH      		;   90
	Define	WHAT     		;   91
	Define	BTHE     		;   92
	Define	WNHA     		;   93
	Define	BZ       		;   94
	Define	PLSENTFIL     		;   95
	Define	THE      		;   96
	Define	AN       		;   97
	Define	AND      		;   98
	Define	IS       		;   99
	Define	NTCAR    		;   100
	Define	WRSYN    		;   101
	Define	VSHP     		;   102
	Define	SHP      		;   103
	Define	WET      		;   104
	Define	NNA      		;   105
	Define	OYSB     		;   106
	Also	TEXTUR
	Also	TEXTURE
	Define	SMTH     		;   107
	Define	ROUGH    		;   108
	Define	HOT      		;   109
	Define	COLD     		;   110
	Define	SLIPPE	 		;   111
	Define	FURRY    		;   112
	Define	CLAMY    		;   113
	Define	SOFT     		;   114
	Define	HARD     		;   115
	Define	WARM     		;   116
	Define	TXSP1    		;   117
	Define	TXSP2    		;   118
	Define	TXSP3    		;   119
	Define	TXSP4
	Define	TXSP5
	Define	OBLOAD    		;   120
	Define	OBSAVE    		;   121
	Define	GOT      		;   122
	Define	FXTIT    		;   123
	Define	CON      		;   124
	Define	ANYT     		;   125
	Define	FAPART   		;   126
	Define	SEPAWY   		;   127
	Define	NOT      		;   128
	Define	DFLESS   		;   129
	Define	DFND     		;   130
	Define	DEFET    		;   131
	Define	TSOFT    		;   132
	Define	THANK    		;   133
	Define	UNCON    		;   134
	Define	ASLEP    		;   135
	Define	AETARG   		;   136
	Define	OFF      		;   137
	Define	FINSH    		;   138
	Define	LOCKED   		;   139
	Define	FUN      		;   140
	Define	INTER    		;   141
	Define	CRUNCH   		;   142
	Define	WNTMVE   		;   143
	Define	AHEM     		;   144
	Define	CNTHEA  		;   145
	Define	FXPERM   		;   146
	Define	FITS     		;   147
	Define	COP0     		;   148
	Define	WUTHNK   		;   149
	Define	MAYBE    		;   150
	Define	IC       		;   151
	Define	IUSTND   		;   152
	Define	QUITE    		;   153
	Define	COP6      		;   154		;was 'IAM'
	Define	COP7     		;   155
	Define	USELF    		;   156
	Define	COP9     		;   157
	Define	DSTO     		;   158
	Define	UARE     		;   159
	Define	COP12 					;was 'WISHFU'
	Define	IDONT    		;   161
	Define	WELDNT   		;   162
	Define	COP15    		;   163
	Define	IFUSAY   		;   164
	Define	COP17    		;   165
	Define	TAPDCE   		;   166
	Define	NODO1    		;   167
	Define	NODO2    		;   168
	Define	NODO3    		; 169
	Define	NODO4    		;   170
	Define	NODO5    		;   171
	Define	NODO6    		;   172
	Define	NODO7    		;   173
	Define	NOTYET   		;   174
	Define	NOTIMP   		;   175
	Define	OUTWEI	 		;   176
	Also	OUTWEIRD
	Define	SHIPS    		;   177
	Define	RBEARD   		;   178
	Define	MUNCH    		;   179
	Define	PUFF     		;   180
	Define	NEMPTY   		;   181
	Define	ONLY     		;   182
	Define	ATMOMT   		;   183
	Define	IT       		;   184
	Define	HE       		;   185
	Define	SHE      		;   186
	Define	HER      		;   187
	Define	HIM      		;   188
	Define	THEY     		;   189
	Define	THEM     		;   190
	Define	MSTBE    		;   191
	Define	HASSLE   		;   192
	Define	EH       		;   193
	Define	TTHERE   		;   194
	Define	SILNT    		;   195
	Define	WWAY     		;   196
	Define	VPSYC    		;   197
	Define	PUTON    		;   198
	Define	AYKILL   		;   199
	Define	BEATRB   		;   200
	Define	CONTO    		;   201
	Define	ANYTH    		;   202
	Define	TASTES   		;   203
	Define	REMOVE   		;   204
	Define	NINSID	 		;   205
	Define	AINSID  		;   206
	Define	HITS     		;   207
	Define	OUCH     		;   208
	Define	WAKES    		;   209
	Define	ASLEEP   		;   210
	Define	TPASS    		;   211
	Define	ANOY     		;   212
	Define	FIDDLE   		;   213
	Define	GIVEUP   		;   214
	Define	NOVEL    		;   215
	Define	NMOUTH   		;   216
	Define	PUSH     		;   217
	Define	TOGTY    		;   218
	Define	THEMAL  		;   219
	Define	INTOWO  		;   220
	Define	NCONCE 		;   221
	Define	SAYTO   		;   222
	Define	IMPOLI 		;   223
	Also	IMPOLITE
	Define	THOUGH  		;   224
	Define	HELLO    		;   225
	Define	HRD2BE  		;   226
	Define	KNOWNO  		;   227
	Define	REALWT  		;   228
	Define	YDESIR  		;   229
	Define	CAREWN  		;   230
	Define	FALSEI   		;   231
	Define	TRUEI    		;   232
	Define	ERROR    		;   233
	Define	FERG     		;   234
	Define	FOR      		;   235
	Define	GIVE     		;   236
	Define	FLOOK    		;   237
	Define	CANTH    		;   238
	Define	NO.TNG   		;   239
	Define	REBUIL   		; 240
	Define	SOMETH    		; 241
	Define	WHICH     		; 242
	Define	BTINA     		; 243
	Define	ACON      		; 244
	Define	NWHERE    		; 245
	Define	IDNATA   		; 246
	Define	SAYS      		; 247
	Define	PLUSE     		; 248
	Define	MAD       		; 249
	Define	HYF       		; 250
	Define	NOIDEA    		; 251
	Define	WYLTK     		; 252
	Define	NOTOBV    		; 253
	Define	STQ       		; 254
	Define	IDNK      		; 255
	Define	BECOF     		; 256
	Define	BECAUS   		; 257
	Define	EMMM      		; 258
	Define	TNNICE    		; 259
	Define	FSALE     		; 260
	Define	AYOURS    		; 261
	Define	NTENDE   		; 262
	Define	STRANG   		; 263
	Define	NDEAL     		; 264
	Define	SO        		; 265
	Define	PUT       		; 266
	Define	ONGND     		; 267
	Define	CNTCAR    		; 268
	Define	TO        		; 269
	Define	LIGHT     		; 270
	Define	IDNTT     		; 271
	Define	QSUIT     		; 272
	Define	CNTUNL    		; 273
	Define	STEADF 		; 274
	Define	APEARS    		; 275
	Define	2MUCH     		; 276
	Define	CLOSED    		; 277
	Define	1ATTIM   		; 278
	Define	ROPED     		; 279
	Define	TEAR      		; 280
	Define	SHRED     		; 281
	Define	TOO       		; 282
	Define	TOUGH     		; 283
	Define	ENOUGH    		; 284
	Define	STRONG    		; 285
	Define	NOCUT     		; 286
	Define	NEASY     		; 287
	Define	MINDRD    		; 288
	Define	NTREAD    		; 289
	Define	CLDSHR    		; 290
	Define	TEMPTR    		; 291
	Define	IYAUPT   		;292
	Also	IYAUPTI
	Define	ICDOAN   		;293
	Also	ICDOANY
	Define	CANCOP   		;294
	Define	OR        		;295
	Define	SHUTUP    		;296
	Define	IDBELY   		;297
	Also	IDBELYU
	Define	MBEITI   		;298
	Define	ITSGOD   		;299
	Define	YURMIS   		;300
	Define	FORYOU    		;301
	Define	MYPROB   		;302
	Define	ALWAYS 		;303
	Define	IMFINE    		;304
	Define	VERYWE  		;305
	Define	NONE      		;306
	Define	NOTMUC   		;307
	Define	IHAVNT    		;308
	Define	ENGH      		;309
	Define	COULDD   		;310
	Define	PERHAP   		;311
	Also	PERHAPS
	Define	POSSIB  		;312
	Define	UNLIKE  		;313
	Define	NOCHAN 		;314
	Define	NOTREA 		;315
	Define	NEVER     		;316
	Define	USEBRA  		;317
	Also	USEBRAIN
	Define	ISPOSS    		;318
	Define	IDKYC     		;319
	Define	ASKSOM   		;320
	Define	SLIM     		;321
	Define	DONTWR   		;322
	Also	DONTWRY
	Define	FUTURE    		;323
	Define	WLATER    		;324
	Define	DWAM      		;325
	Define	DEV       		;326
	Define	RIGHTS    		;327
	Define	SHOULD  		;328
	Define	IHNI      		;329
	Define	ICHYT     		;330
	Define	ISNTRE  		;331
	Define	TWBT      		;332
	Define	INTHAI    		;333
	Define	GQUEST    		;334
	Define	FORGOT    		;335
	Define	UNLOCK  		; 336
	Define	NWAY      		; 337
	Define	IAM       		; 338
	Define	WIERD     		; 339
	Define	GOTNON   		; 340
	Define	BIDHIT    		; 341
	Define	NONEED    		; 342
	Define	NOONE     		; 343
	Define	JSTWHT    		; 344
	Define	CLICK     		; 345
	Define	SQLCH    		; 346
	Define	THUD     		; 347
	Define	VERY     		; 348
	Define	EXTREM		; 349
	Define	QUITE_2    		; 350
	Define	HEAVY    		; 351
	Define	LIGHT_2    		; 352
	Define	LARGE    		; 353
	Define	SMALL    		; 354
	Define	TOO_2      		; 355
	Define	FNOI     		; 356
	Define	HFUN     		; 357
	Define	BROKEN   		; 358
	Define	GIVE_2     		; 359
	Define	ABSKIS   		; 360
	Define	JUSONE   		; 361
	Define	NOTIE    		; 362
	Define	NORCH    		; 363
	Define	FHERE    		; 364
	Define	NOTAKE   		; 365
	Define	NORIDE   		; 366
	Define	WHAK     		; 367
	Define	FALOFF   		; 368
	Define	TALKME   		; 369
	Define	ALRDY    		; 370
	Define	YBLUKY   		; 371
	Define	TRICKQ   		; 372
	Define	MTMAGI  		; 373
	Define	NOTSEA  		; 374
	Define	ONFEET   		; 375
	Define	NSO      		; 376
	Define	CSOT     		; 377
	Define	CSJA     		; 378
	Define	NOLIE    		; 379
	Define	SITUP    		; 380
	Define	ASO      		; 381
	Define	NOSEAT   		; 382
	Define	CTEXT    		; 383
	Define	ONEOF    		; 384
	Define	GLARE    		; 385
	Define	MAGFOR 		; 386
	Define	LEAP     		; 387
	Define	HAS      		; 388
	Define	VANI     		; 389
	Define	DEADDN   		; 390
	Define	IFONLY   		; 391
	Define	BURNAW  		; 392
	Define	DEADED   		; 393
	Define	LETOFF   		; 394
	Define	PDR2     		; 395
	Define	USURE    		; 396
	Define	QUIT     		; 397
	Define	ENSURE   		; 398
	Define	IAGREE      		; 399
	Define	4BUFS       		; 400
	Define	JCHKG       		; 401
	Define	WISLI       		; 402
	Define	NO          		; 403
	Define	MINE        		; 404
	Define	FIND        		; 405
	Define	GETOFF      		; 406
	Define	NOTMY       		; 407
	Define	THIEF       		; 408
	Define	WPRI        		; 409
	Define	COULD       		; 410
	Define	ABOUT       		; 411
	Define	WORTH       		; 412
	Define	WLESS       		; 413
	Define	PROBAB      		; 414
	Define	OFANY       		; 415
	Define	ME          		; 416
	Define	PLESS       		; 417
	Define	BADW        		; 418
	Define	ASCONT      		; 419
	Define	HAPPY       		; 420
	Define	HOHO        		; 421
	Define	UMEAN       		; 422


M.LASTMSG	EQU	MsgNum-1	;422   ;TABLE OF 35 MSG'S BETWEEN
	XDEF	M.LASTMSG

	Define	IKAAIT    		;   *M.LAST*MSG+1
	Define	CHIPS     		;   *M.LAST*MSG+2
	Define	GAWAY     		;   *M.LAST*MSG+3
	Define	IDCARE    		;   *M.LAST*MSG+4
	Define	WMYTT     		;   *M.LAST*MSG+5
	Define	IWILL     		;   *M.LAST*MSG+6
	Define	HOWHEL    		;   *M.LAST*MSG+7
	Define	NTRUST    		;   *M.LAST*MSG+8
	Define	MYOB      		;   *M.LAST*MSG+9
	Define	EATHIM    		;   *M.LAST*MSG+10
	Define	YOBBO     		;   *M.LAST*MSG+11
	Define	SMIND     		;   *M.LAST*MSG+12
	Define	TRYSOM   		;   *M.LAST*MSG+13
	Define	WDYAIT    		;   *M.LAST*MSG+14
	Define	PARDON    		; M.LASTMSG+15
	Define	IMPAT     		;   *M.LAST*MSG+16
	Define	WIPLSE    		;   *M.LAST*MSG+17
	Define	YMBJ      		;  M.LASTMSG+18
	Define	DBSURE    		;  M.LASTMSG+19
	Define	YOUPRO   		;   *M.LAST*MSG+20
	Define	READLN    		;   *M.LAST*MSG+21
	Define	IJHA      		;   *M.LAST*MSG+22
	Define	IYRW      		;   *M.LAST*MSG+23
	Define	MAYU      		;   *M.LAST*MSG+24
	Define	INDEST    		;   *M.LAST*MSG+25
	Define	UPTOU     		;   *M.LAST*MSG+26
	Define	HISCUM    		;   *M.LAST*MSG+27
	Define	MEETAG   		;   *M.LAST*MSG+28
	Define	ADVGRE   		;   *M.LAST*MSG+29
	Define	HIAGAI   		;   *M.LAST*MSG+30
	Define	GURUHI    		;   *M.LAST*MSG+31
	Define	WDYW      		;   *M.LAST*MSG+32
	Define	DEVHI     		;   *M.LAST*MSG+33
	Define	DEVHI2    		;   *M.LAST*MSG+34
	Define	WUDOC     		;   *M.LAST*MSG+35

* NEW messages!!!!

	Define	 CONTSOME
	Define	 SOMESLOSH
	Define	 SPILLAND
	Define	 VIGOUR
	Define	 NOBODYBET
	Define	 REFUSEBET
	Define	 CANTALLOW
	Define	 RESTRAIN
	Define	 ZZZZZZ
	Define	 THINKAWHILE
	Define	 TIGHTLYHELD
	Define	 FISHING

	Define	 NOTFAINTEST
	Define	 GOTHERE
	Define	 CURSGLANCE
	Define	 PERSUADES
	Define	 NOIMPORTANCE
	Define	 UNFORTUNE
	Define	 BEYONDREACH
	Define	 POINTLESS
	Define	 NEEDCLOSE
	Define	 ABSNOIMPORT
	Define	 SWHAT
	Define	 SWHERE
	Define	 SWHO
	Define	 SDOYOUWANT
	Define	 NOENTER
	Define	 TRYAGAIN
	Define	 MOREPOINTS
	Define	 EFFECTEDBY
	Define	 NEVERARRIVE
	Define	ENTERHINT
	Define	ON_STOP
	Define	DONEPLING
	Define	GRAPHICS
	Define	TEXTSIZE
	Define	FREEHAND
	Define	STIPPLE
	Define	NOPRINTER
	Define	PRINTER
	Define	OPEN_FAILED
	Define	ENTERFILENAME
	Define	BADFUNCT
	Define	ENTERFUNCT

	Define	YOUMEAN
	Define	SURECONTINUE

	Define	SPACE				;shorter that MSG	SPACE
	Define	LINE				;shorter than MSG	LINE
	Define 	NOTINTERESTED
	Define	QUOTE
        
	IFNE	IBM
	Define	TEXTONLY		;for text only IBM version
	ENDC

* use MsgSCNBASE instead
*SCNBASE	EQU	MsgNum-1
*	XDEF	SCNBASE

*
*--------------------------------
*

	include	"macros.asm"

      IFEQ	M68000

	XDEF	MessageCode
	XREF	M_STACK,PLURAL,PRTTXT,P.MSG

Space	MACRO	* routine name
	XREF	\1
	XDEF	DO_Num_\1
DO_Num_\1	EQU	current_DO_Num
current_DO_Num	SET	current_DO_Num-1
	DC.W	\1
	ENDM

current_DO_Num	SET	$07FF

	XDEF	SpaceTable

SpaceTable

	Space	CHAROUT
	Space	GETWRD
	Space	VALID.NP
	Space	CHK4PT
	Space	GETNPC
	Space	GETRELN
	Space	GET_CONT
	Space	DBS
	Space	ISARE
	Space	ISAREN
	Space	P.BTN
	Space	P.SCN
	Space	P.MSG
	Space	P.NOUN
	Space	P.SUB
	Space	P.SUBV
	Space	SETOUT
	Space	P.TN
	Space	P.TNSTOP
	Space	UTIL.BS
	Space	FULLSTOP
	Space	BSFULLSTOP
	Space	CARRIED
	Space	CALCNET
	Space	CHAR.SUB
	Space	CHKNPC
	Space	CLRADJ
	Space	D72
	Space	DBEXIT
	Space	DEAD
	Space	DECODE
	Space	FIXCHK
	Space	FIXCONT
	Space	FIXIREL
	Space	FIXSYS
	Space	FNDWRD
	Space	GETADJPH
	Space	GETNOUNS
	Space	GETVECT
	Space	INCNOUN2
	Space	ISDOFRE
	Space	MNPH
	Space	NOERR
	Space	P.FROM
	Space	P.LIST
	Space	P.OBJPRN
	Space	P.PRN
	Space	P.QN
	Space	P.SAY
	Space	P.TNIN
	Space	P.TSNOUN
	Space	P.VAGE
	Space	P.VERB
	Space	PR20
	Space	R.NOUN
	Space	R.NOUN2
	Space	RUSURE
	Space	SCREEN
	Space	SEARCH
	Space	SEARCH2
	Space	TOPLOC
	Space	UTEXIT
	Space	VALID
	Space	YCANT
	Space	YCANTNOW
	Space	YESERR
	Space	YCANTW
	Space	P.SAY2
	Space	GETNOUN2
	Space	P.LISTSTOP
	Space	VALID.WO
MinSpaceEntry	EQU	current_DO_Num

	XDEF	MinSpaceEntry
	
DoSpaceSave

	LEA	SpaceTable(PC),A0
	EXT.W	D1
	LSL.W	#1,D1
	NEG.W	D1
	ADD.W	-2(A0,D1.W),D1
	LEA	-2(A0,D1.W),A0
	MOVE.L	A0,M_STACK(A4)
	BTST	#3,(A1)				;NE =>bra
	PULL_L	D1/A0/A1
	BEQ.S	10$
	POPSP
10$
	MOVE.L	M_STACK(A4),-(A7)
	RET




MessageCode

	MOVE.L  (A7),M_STACK(A4)
	PUSH_L  D1/A0/A1

	MOVE.L	M_STACK(A4),A1
	MOVE.W	-(A1),D1
	AND.W	#$07FF,D1

	CMP.W	#current_DO_Num,D1
	BPL	DoSpaceSave

	CMP.W   #1,PLURAL(A4)
	BLT.S   05$                ;NORMAL
	SUB.W   #2,PLURAL(A4)     ;ADJUST
	BRA.S   10$                ;FIXED
05$
	CLR.W   PLURAL(A4)
	BTST    #0,(A0)
	BEQ.S   10$
	NOT.W   PLURAL(A4)         ;MOVE IN -1
10$
	BTST	#3,(A1)			;kill BRA via CALL bit
	BEQ.S	20$			;EQ => call
	CALL	P.MSG
	CLR.W   PLURAL(A4)   
	PULL_L  D1/A0/A1
	POPSP				;was a BRA
	RET
20$
	CALL	P.MSG
	CLR.W   PLURAL(A4)   
	PULL_L  D1/A0/A1
	RET

      ENDC
  
  
   END
