**************************************************************
*
*	%W%
*	%G%
*
**************************************************************

* Jinxter protection list (may live UNDER the stack!)

* Doug 16/11/87

	SECTION	"ADV",code

	include	"equates.i"
	include	"nounequ1.i"
	include	"verbequ.i"
	include	"adjequ.i"

; NE if last macro was WordTable
wt	set	1	; pretend

Position	MACRO	* word(as per text),page#,col#,line#,word#
* \1
	ifeq	wt
	fail	two positions in a row
	endc

	DC.W	(\3<<14)+(\2<<9)+(\4<<4)+\5
wt	set	0

	ENDM

WordTable	MACRO	* section,word	(as per matchbuf)

	ifeq	wt
	DC.W	(\1<<10)+\2
wt	set	1
	endc

	ENDM

	IFNE	M68000

Position2	MACRO	* word(as per text),page#,col#,line#,word#
* \1
	DC.W	(\3<<14)+(\2<<9)+(\4<<4)+\5
wt	set	0

	ENDM

WordTable2	MACRO	* section,word	(as per matchbuf)

	ifeq	wt
	DC.W	(\1<<10)+\2
wt	set	1
	endc

	ENDM

	ENDC

	IFEQ	M68000
Position2	MACRO
		ENDM
WordTable2	MACRO
		ENDM
	ENDC

	XDEF	ProtList,ProtSize,ProtEnd


	DS.B	32				;spare stack

ProtList

	Position	out,1,1,1,5
	WordTable	SECT.VERB,VNEXIT
	Position	guardian,1,1,2,6
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	house,1,1,3,1
	WordTable	SECT.NOUN,NYOUR_HOUSE-1
	WordTable	SECT.NOUN,NXAMS_HOUSE-1
	Position	to,1,1,5,2
	WordTable	SECT.CONJ,$0001
	Position	len,1,1,6,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	can,1,1,8,1
	WordTable	SECT.ADJ,ADJ_can-4
	WordTable	SECT.NOUN,NCANOWORMS-1
	Position	of,1,1,9,2,
	WordTable	SECT.SPEC,$000b
	Position	weather,1,1,11,2
	WordTable	SECT.ADJ,ADJ_weather-4
	Position	green,1,1,13,2
	WordTable	SECT.ADJ,ADJ_green-4
	Position	ferg,1,1,16,1
	WordTable	SECT.ADJ,ADJ_ferg-4
	WordTable	SECT.NOUN,NAQUITANIAN_FERG-1
	Position	of,1,1,18,4
	WordTable	SECT.SPEC,$000b
	Position	bracelet,1,2,1,3
	WordTable	SECT.NOUN,NBRACELET-1
	Position	and,1,2,2,4
	WordTable	SECT.SPEC,$0000
	Position	her,1,2,3,2
	WordTable2	SECT.SPEC,$0005
	Position2	her,1,2,3,4
	WordTable	SECT.SPEC,$0005
	Position	wossname,1,2,6,1
	WordTable	SECT.SPEC,$0003
	Position	look,1,2,7,1
	WordTable	SECT.VERB,VNLOOK
	Position	bracelet,1,2,8,3
	WordTable	SECT.NOUN,NBRACELET-1
	Position	are,1,2,9,2
	WordTable	SECT.TOBE,$0001
	Position	were,1,2,11,2
	WordTable	SECT.TOBE,$0003
	Position	right,1,2,14,2
	WordTable	SECT.ADJ,ADJ_right-4
	Position	right,1,2,15,5
	WordTable	SECT.ADJ,ADJ_right-4
	Position	job,1,2,16,6
	WordTable	SECT.NOUN,NJOB-1
	Position	wossname,1,2,17,3
	WordTable	SECT.SPEC,$0003
	Position2	wossname,1,2,18,1
	WordTable2	SECT.SPEC,$0003
	Position	guardian,1,2,19,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	of,1,2,20,3
	WordTable	SECT.SPEC,$000b
	Position	help,1,2,22,4
	WordTable	SECT.VERB,VNHELP
	Position	but,1,2,23,2
	WordTable	SECT.SPEC,$0009
	Position	on,1,2,24,5
	WordTable	SECT.CONJ,$0003
	Position	and,1,2,25,3
	WordTable	SECT.SPEC,$0000
	Position	up,1,2,26,1
	WordTable	SECT.DIR,$0008
	Position	wind,1,2,27,1
	WordTable	SECT.VERB,VNWIND
	WordTable	SECT.NOUN,NWWIND0-1
	Position	moose,1,2,29,3
	WordTable	SECT.ADJ,ADJ_moose-4
	Position	in,1,3,1,9
	WordTable	SECT.CONJ,$0005
	Position	this,1,3,2,4
	WordTable	SECT.QUANT,$0006
	Position	with,1,3,4,2
	WordTable	SECT.CONJ,$0000
	Position	drop,1,3,5,3
	WordTable	SECT.VERB,VNDROP
	Position	len,1,3,6,1
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	with,1,3,7,4
	WordTable	SECT.CONJ,$0000
	Position	which,1,3,8,6
	WordTable	SECT.REL,$0001
	Position2	len,1,3,10,3
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position	for,1,3,11,5
	WordTable	SECT.CONJ,$0006
	Position	touch,1,3,12,4
	WordTable	SECT.VERB,VNTOUCH
	Position	over,1,3,14,1
	WordTable	SECT.CONJ,$0008
	Position	are,2,1,1,4
	WordTable	SECT.TOBE,$0001
	Position	from,2,1,2,5
	WordTable	SECT.CONJ,$0002
	Position	cheese,2,1,3,3
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	len,2,1,5,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	wossname,2,1,6,2
	WordTable	SECT.SPEC,$0003
	Position	the,2,1,8,4
	WordTable	SECT.QUANT,$0000
	Position2	thingy,2,1,11,4
	WordTable2	SECT.VERB,VNTHINGY
	WordTable2	SECT.ADJ,ADJ_thingy-4
	WordTable2	SECT.NOUN,NINT_SOL-1
	Position	of,2,1,14,3
	WordTable	SECT.SPEC,$000b
	Position	but,2,1,15,1
	WordTable	SECT.SPEC,$0009
	Position2	with,2,1,18,3
	WordTable2	SECT.CONJ,$0000
	Position	thingy,2,1,19,3
	WordTable	SECT.VERB,VNTHINGY
	WordTable	SECT.ADJ,ADJ_thingy-4
	WordTable	SECT.NOUN,NINT_SOL-1
	Position	that,2,1,20,3
	WordTable	SECT.REL,$0000
	Position	cheese,2,1,21,2
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	sandwich,2,1,21,3
	WordTable	SECT.NOUN,NSANDWICH-1
	Position	over,2,1,22,5
	WordTable	SECT.CONJ,$0008
	Position	bread,2,1,23,1
	WordTable	SECT.ADJ,ADJ_bread-4
	WordTable	SECT.NOUN,NPUDDING-1
	WordTable	SECT.NOUN,NPUDDING2-1
	Position2	cheese,2,1,24,3,
	WordTable2	SECT.ADJ,ADJ_cheese-4
	WordTable2	SECT.NOUN,NCHEESE-1
	Position	len,2,2,1,4
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position2	doodah,2,2,2,1
	WordTable2	SECT.VERB,VNDOODAH
	WordTable2	SECT.ADJ,ADJ_doodah-4
	WordTable2	SECT.NOUN,NINT_AQUA-1
	Position2	the,2,2,3,5,
	WordTable2	SECT.QUANT,$0000
	Position	wossname,2,2,4,1,
	WordTable	SECT.SPEC,$0003
	Position	but,2,2,5,2
	WordTable	SECT.SPEC,$0009
	Position	len,2,2,6,1
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	guardian,2,2,7,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	cheese,2,2,8,3
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position2	the,2,2,10,2
	WordTable2	SECT.QUANT,$0000
	Position	this,2,2,12,3,
	WordTable	SECT.QUANT,$0006
	Position2	the,2,2,13,4,
	WordTable2	SECT.QUANT,$0000
	Position2	the,2,2,14,6
	WordTable2	SECT.QUANT,$0000
	Position2	the,2,3,2,1
	WordTable2	SECT.QUANT,$0000
	Position	an,2,3,4,1
	WordTable	SECT.QUANT,$0003
	Position	for,2,3,6,4
	WordTable	SECT.CONJ,$0006
	Position	sit,2,3,8,3
	WordTable	SECT.VERB,VNSIT
	Position	trees,2,3,9,1,
	WordTable	SECT.NOUN,NTREES-1
	Position	small,2,3,12,3,
	WordTable	SECT.ADJ,ADJ_small-4
	Position	but,2,3,14,4,
	WordTable	SECT.SPEC,$0009
	Position	the,2,3,15,2
	WordTable	SECT.QUANT,$0000
	Position	through,2,3,16,1
	WordTable	SECT.CONJ,$000e
	Position	no,2,3,17,3
	WordTable	SECT.YESNO,$0001
	Position	the,2,3,19,4
	WordTable	SECT.QUANT,$0000
	Position	bread,2,3,20,6
	WordTable	SECT.ADJ,ADJ_bread-4
	WordTable	SECT.NOUN,NPUDDING-1
	WordTable	SECT.NOUN,NPUDDING2-1
	Position	roll,2,3,21,1,
	WordTable	SECT.NOUN,NPLUM-1
	Position	are,2,3,22,3
	WordTable	SECT.TOBE,$0001
	Position	with,2,3,23,2
	WordTable	SECT.CONJ,$0000
	Position	the,2,3,24,1
	WordTable	SECT.QUANT,$0000
	Position	with,2,3,25,5
	WordTable	SECT.CONJ,$0000
	Position	that,2,3,27,2
	WordTable	SECT.REL,$0000
	Position	her,2,3,30,1
	WordTable	SECT.SPEC,$0005
	Position	the,2,3,31,2
	WordTable	SECT.QUANT,$0000
	Position	job,3,1,5,1,
	WordTable	SECT.NOUN,NJOB-1
	Position	stone,3,1,8,2
	WordTable	SECT.ADJ,ADJ_stone-4
	WordTable	SECT.NOUN,NWALL-1
	Position	key,3,1,11,1
	WordTable	SECT.ADJ,ADJ_key-4
	WordTable	SECT.NOUN,NKEY1-1
	WordTable	SECT.NOUN,NKEY2-1
	WordTable	SECT.NOUN,NKEY3-1
	WordTable	SECT.NOUN,NKEY4-1
	Position	green,3,1,14,4,
	WordTable	SECT.ADJ,ADJ_green-4
	Position	bracelet,3,1,17,2
	WordTable	SECT.NOUN,NBRACELET-1
	Position	the,3,1,21,2
	WordTable	SECT.QUANT,$0000
	Position	stand,3,1,22,2
	WordTable	SECT.VERB,VNSTAND
	Position2	of,3,1,26,3
	WordTable2	SECT.SPEC,$000b
	Position2	jannedor,3,1,28,2
	WordTable2	SECT.NOUN,NWITCH-1
	Position2	of,3,2,5,2
	WordTable2	SECT.SPEC,$000b
	Position2	the,3,2,8,3
	WordTable2	SECT.QUANT,$0000
	Position2	but,3,2,12,3
	WordTable2	SECT.SPEC,$0009
	Position2	old,3,3,2,4
	WordTable2	SECT.ADJ,ADJ_old-4
	Position2	go,3,3,7,4
	WordTable2	SECT.VERB,VNGO
	Position	out,3,3,8,1
	WordTable	SECT.VERB,VNEXIT
	Position	it,3,3,12,5
	WordTable	SECT.SPEC,$0003
	Position	can,3,3,14,1
	WordTable	SECT.ADJ,ADJ_can-4
	WordTable	SECT.NOUN,NCANOWORMS-1
	Position	in,4,1,5,4
	WordTable	SECT.CONJ,$0005
	Position	the,4,1,8,1
	WordTable	SECT.QUANT,$0000
	Position	len,4,1,9,3
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	two,4,1,11,4
	WordTable	SECT.ADJ,ADJ_two-4
	WordTable	SECT.NUM,$0002
	Position	the,4,1,13,6
	WordTable	SECT.QUANT,$0000
	Position	at,4,1,16,3
	WordTable	SECT.CONJ,$000b
	Position	that,4,1,18,4
	WordTable	SECT.REL,$0000
	Position	the,4,1,22,1
	WordTable	SECT.QUANT,$0000
	Position	with,4,1,25,1
	WordTable	SECT.CONJ,$0000
	Position	the,4,1,29,2
	WordTable	SECT.QUANT,$0000
	Position	and,4,1,31,5
	WordTable	SECT.SPEC,$0000
	Position	of,4,2,1,4
	WordTable	SECT.SPEC,$000b
	Position	the,4,2,5,2
	WordTable	SECT.QUANT,$0000
	Position	all,4,2,7,4
	WordTable	SECT.SPEC,$0001
	Position	up,4,2,8,5
	WordTable	SECT.DIR,$0008
	Position	up,4,2,12,3
	WordTable	SECT.DIR,$0008
	Position	of,4,3,2,2
	WordTable	SECT.SPEC,$000b
	Position	a,4,3,5,2
	WordTable	SECT.QUANT,$0001
	Position	on,4,3,8,2
	WordTable	SECT.CONJ,$0003
	Position	the,4,3,11,1
	WordTable	SECT.QUANT,$0000
	Position	len,4,3,12,2,
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	len,4,3,13,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position2	len,4,3,14,1
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position2	house,4,3,17,4
	WordTable2	SECT.NOUN,NYOUR_HOUSE-1
	WordTable2	SECT.NOUN,NXAMS_HOUSE-1
	Position2	against,5,1,3,1
	WordTable2	SECT.CONJ,$0011
	Position2	any,5,1,11,8
	WordTable2	SECT.WBITS,$0003
	Position2	which,5,1,19,2
	WordTable2	SECT.REL,$0001
	Position2	out,5,1,24,4
	WordTable2	SECT.VERB,VNEXIT
	Position2	cheese,5,2,1,1,
	WordTable2	SECT.ADJ,ADJ_cheese-4
	WordTable2	SECT.NOUN,NCHEESE-1
	Position	this,5,2,11,3
	WordTable	SECT.QUANT,$0006
*	Position	two,5,2,15,1
*	WordTable	SECT.ADJ,ADJ_two-4
*	WordTable	SECT.NUM,$0002
	Position	of,5,2,20,2
	WordTable	SECT.SPEC,$000b
	Position	cheese,5,3,3,1
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	no,5,3,7,1
	WordTable	SECT.YESNO,$0001
	Position	up,5,3,11,3
	WordTable	SECT.DIR,$0008
	Position	buy,5,3,14,5
	WordTable	SECT.VERB,VNBUY
	Position	hand,6,1,1,4
	WordTable	SECT.NOUN,NHAND-1
	Position	cheese,6,1,3,4
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	guardian,6,1,6,4
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	at,6,1,15,4
	WordTable	SECT.CONJ,$000b
	Position	and,6,1,19,2
	WordTable	SECT.SPEC,$0000
	Position	the,6,2,1,5
	WordTable	SECT.QUANT,$0000
	Position	long,6,2,6,1
	WordTable	SECT.ADJ,ADJ_long-4
	Position	look,6,2,11,1
	WordTable	SECT.VERB,VNLOOK
	Position	with,6,2,16,1
	WordTable	SECT.CONJ,$0000
	Position	fifty,6,2,20,2
	WordTable	SECT.NUM,$0017
	Position	the,6,3,1,6
	WordTable	SECT.QUANT,$0000
	Position	out,6,3,3,1
	WordTable	SECT.VERB,VNEXIT
	Position	a,7,1,1,6
	WordTable	SECT.QUANT,$0001
	Position2	len,7,1,6,3
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position2	guardian,7,1,8,2
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position2	for,7,1,13,1
	WordTable2	SECT.CONJ,$0006
	Position2	go,7,1,15,4
	WordTable2	SECT.VERB,VNGO
	Position2	front,7,2,4,1
	WordTable2	SECT.ADJ,ADJ_front-4
	Position2	guardian,7,2,5,1
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position	len,7,2,10,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	with,7,2,13,2
	WordTable	SECT.CONJ,$0000
	Position	len,7,2,16,4
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	and,7,2,22,2
	WordTable	SECT.SPEC,$0000
	Position	len,7,2,28,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	ask,7,3,1,5,
	WordTable	SECT.VERB,VNASK
	Position	guardian,7,3,2,2
	WordTable	SECT.NOUN,NGUARDIAN-1
	Position	with,7,3,13,3
	WordTable	SECT.CONJ,$0000
	Position	help,7,3,16,3
	WordTable	SECT.VERB,VNHELP
	Position	and,7,3,19,2
	WordTable	SECT.SPEC,$0000
	Position	house,7,3,20,1
	WordTable	SECT.NOUN,NYOUR_HOUSE-1
	WordTable	SECT.NOUN,NXAMS_HOUSE-1
	Position	of,7,3,23,2
	WordTable	SECT.SPEC,$000b
	Position	all,8,2,1,3
	WordTable	SECT.SPEC,$0001
	Position	cheese,8,2,2,4
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	shop,8,2,2,5
	WordTable	SECT.ADJ,ADJ_shop-4
	WordTable	SECT.NOUN,NPSEUDOCMAKERS-1
	WordTable	SECT.NOUN,NCLOCKMAKERS-1
	Position	door,8,2,3,1
	WordTable	SECT.NOUN,NAUTODOORS-1
	WordTable	SECT.NOUN,NGREEN_DOOR-1
	WordTable	SECT.NOUN,NKITCHEN_DOOR-1
	WordTable	SECT.NOUN,NWOODEN_DOOR-1
	WordTable	SECT.NOUN,NSLIDINGDOOR-1
	WordTable	SECT.NOUN,NRED_DOOR-1
	WordTable	SECT.NOUN,NWITCH_DOOR1-1
	WordTable	SECT.NOUN,NWITCH_DOOR-1
	WordTable	SECT.NOUN,NCMAKERS_DOOR-1
	WordTable	SECT.NOUN,NDOOR-1
	WordTable	SECT.NOUN,NDOOR1-1
	WordTable	SECT.NOUN,NFRONT_DOOR-1
	WordTable	SECT.NOUN,NAIRLOCK_DOOR-1
	WordTable	SECT.NOUN,NPODOOR-1
	WordTable	SECT.NOUN,NPUBDOOR-1
	WordTable	SECT.NOUN,NBATHROOMDOOR-1
	WordTable	SECT.NOUN,NKITCHENDOOR-1
	WordTable	SECT.NOUN,NLEATHERDOOR-1
	WordTable	SECT.NOUN,NTELEGRAPH_DOOR-1
	WordTable	SECT.NOUN,NSOLID_DOOR-1
	WordTable	SECT.NOUN,NOVENDOOR-1
	WordTable	SECT.NOUN,NTRAPDOOR1-1
	WordTable	SECT.NOUN,NOVEN-1
	Position	down,8,3,1,3
	WordTable	SECT.DIR,$0009
	Position	can,8,3,1,5
	WordTable	SECT.ADJ,ADJ_can-4
	WordTable	SECT.NOUN,NCANOWORMS-1
	Position	to,8,3,2,4
	WordTable	SECT.CONJ,$0001
	Position	cheese,9,1,2,2
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	to,9,2,2,5
	WordTable	SECT.CONJ,$0001
	Position	put,9,2,3,1
	WordTable	SECT.VERB,VNPUT
	Position	then,9,2,4,1
	WordTable	SECT.SPEC,$0008
	Position	ask,9,3,1,1,
	WordTable	SECT.VERB,VNASK
	Position	cheese,9,3,2,1
	WordTable	SECT.ADJ,ADJ_cheese-4
	WordTable	SECT.NOUN,NCHEESE-1
	Position	tell,10,1,1,4
	WordTable	SECT.VERB,VNTELL
	Position	are,10,1,2,1
	WordTable	SECT.TOBE,$0001
	Position	some,10,2,1,2
	WordTable	SECT.QUANT,$0002
	Position	on,10,2,2,3
	WordTable	SECT.CONJ,$0003
	Position2	a,10,2,4,2
	WordTable2	SECT.QUANT,$0001
	Position2	read,10,2,5,3
	WordTable2	SECT.VERB,VNREAD
	Position2	for,10,3,2,4
	WordTable2	SECT.CONJ,$0006
	Position2	guardian,11,1,1,2
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position2	len,11,1,2,1
	WordTable2	SECT.NOUN,NGUARDIAN-1
	Position	all,11,1,9,1
	WordTable	SECT.SPEC,$0001
	Position	and,11,2,2,2
	WordTable	SECT.SPEC,$0000
*	Position	version,11,2,4,1
*	WordTable	SECT.CONT,$0003
	Position	it,11,2,8,3
	WordTable	SECT.SPEC,$0003
	Position	tell,11,2,11,1
	WordTable	SECT.VERB,VNTELL
	Position	wossname,11,2,13,2
	WordTable	SECT.SPEC,$0003
*	Position	hint,11,2,14,2
*	WordTable	SECT.CONT,$0020
	Position	wossname,11,3,2,3
	WordTable	SECT.SPEC,$0003
	Position	answer,11,3,6,1
	WordTable	SECT.VERB,VNANSWER
	Position	all,11,3,8,2
	WordTable	SECT.SPEC,$0001
	Position	and,11,3,10,1
	WordTable	SECT.SPEC,$0000
	Position	to,11,3,14,1
	WordTable	SECT.CONJ,$0001

ProtSize	EQU	(*-ProtList)/4

	DS.B	8

ProtEnd

	END
