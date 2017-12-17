***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************

******************************************************
*
*	%W%
*	%G%
*
******************************************************


*--------------------------------
*
* SRC.ADV.VERBEQUATES
*
*--------------------------------

VerbDEF		MACRO	
VN\1		EQU	VerbDEFTemp
VerbDEFTemp	SET	VerbDEFTemp+1
		ENDM

VerbDEFTemp	SET	0


VNLOOK	EQU	0
VNEXAMINE	EQU	1
VNGET	EQU	2
VNTAKE	EQU	3
VNCARRY	EQU	4
VNDROP	EQU	5
VNLEAVE	EQU	6
VNGO	EQU	7
VNWALK	EQU	8
VNENTER	EQU	9
VNRUN	EQU	10
VNSHIFT	EQU	11
VNMOVE	EQU	12
VNLIFT	EQU	13
VNRAISE	EQU	14
VNLOWER	EQU	15
VNPUSH	EQU	16
VNPULL	EQU	17
VNSMELL	EQU	18
VNTASTE	EQU	19
VNEAT	EQU	20
VNDRINK	EQU	21
VNPUT	EQU	22
VNINSERT	EQU	23
VNINVENTORY	EQU	24
VNFIT	EQU	25
VNEMPTY	EQU	26
VNFILL	EQU	27
VNJUMP	EQU	28
VNTHROW	EQU	29
VNPUNCH	EQU	30
VNKICK	EQU	31
VNFIGHT	EQU	32
VNATTACK	EQU	33
VNKILL	EQU	34
VNBREAK	EQU	35
VNSMASH	EQU	36
VNDAMAGE	EQU	37
VNREMOVE	EQU	38
VNSCRAPE	EQU	39
VNLIGHT	EQU	40
VNUNLIGHT	EQU	41
VNEXTINGUISH	EQU	42
VNOPEN	EQU	43
VNCLOSE	EQU	44
VNSUSS	EQU	45
VNSTATUS	EQU	46
VNDEBUG	EQU	47
VNTWIST	EQU	48
VNTURN	EQU	49
VNSAY	EQU	50
VNSHOUT	EQU	51
VNYELL	EQU	52
VNCRY	EQU	53
VNCALL	EQU	54
VNFEEL	EQU	55
VNTOUCH	EQU	56
VNWEAR	EQU	57
VNREAD	EQU	58
VNHIT	EQU	59
VNSTEAL	EQU	60
VNBASH	EQU	61
VNUNLOCK	EQU	62
VNPICK	EQU	63
VNTIE	EQU	64
VNFIX	EQU	65
VNJOIN	EQU	66
VNUNTIE	EQU	67
VNFEED	EQU	68
VNBITE	EQU	69
VNUSE	EQU	70
VNCROSS	EQU	71
VNSLIDE	EQU	72
VNBLOW	EQU	73
VNPRESS	EQU	74
VNGIVE	EQU	75
VNCLIMB	EQU	76
VNBURN	EQU	77
VNIGNITE	EQU	78
VNFOD	EQU	79
VNEXIT	EQU	80
VNASK	EQU	81
VNQUESTION	EQU	82
VNMELT	EQU	83
VNLISTEN	EQU	84
VNSEARCH	EQU	85
VNFIND	EQU	86
VNGRAB	EQU	87
VNRUB	EQU	88
VNWAIT	EQU	89
VNWAKE	EQU	90
VNBUY	EQU	91
VNPURCHASE	EQU	92
VNLOCK	EQU	93
VNTEAR	EQU	94
VNRIP	EQU	95
VNCUT	EQU	96
VNKNOCK	EQU	97
VNPOINT	EQU	98
VNSHINE	EQU	99
VNEXITS	EQU	100
VNCOVER	EQU	101
VNUNCOVER	EQU	102
VNSIT	EQU	103
VNSTAND	EQU	104
VNTELL	EQU	105
VNOFFER	EQU	106
VNDISMOUNT	EQU	107
VNHELP	EQU	108
VNLAUGH	EQU	109
VNSHOW	EQU	110
VNLOOP	EQU	111
VNDIG	EQU	112
VNBET	EQU	113
VNSTOP	EQU	114
VNPLAY	EQU	115
VNSWIM	EQU	116
VNFLUSH	EQU	117
VNPISS	EQU	118
VNSLEEP	EQU	119
VNCHEW	EQU	120
VNREFLECT	EQU	121
VNHOLD	EQU	122
VNPLACE	EQU	123
VNPOSITION	EQU	124
VNILLUMINATE	EQU	125
VNCATCH	EQU	126
VNCUE	EQU	127
VNSHAKE	EQU	128
VNROLL	EQU	129
VNFISH	EQU	130
VNLOOK_FOR	EQU	131
VNSTICK	EQU	132

	IFD	VNLOOK_FOR

VNMAKELOCAL	EQU	VNLOOK_FOR

	ENDC


* produced on Tue Oct 17 01:03:28 2017

* checksum = c73eb969
