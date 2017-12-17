***************************************************************************
*
*
*	fred23jr output from file '../guild'*
*
***************************************************************************

		SECTION	"ADV",code

PIC_LIST	MACRO	* list of resource names
		DC.B	'\1',0
		IFNC	'\2',''
		DC.B	'\2',0
		ENDC
		IFNC	'\3',''
		DC.B	'\3',0
		ENDC
		IFNC	'\4',''
		DC.B	'\4',0
		ENDC
		IFNC	'\5',''
		DC.B	'\5',0
		ENDC
		IFNC	'\6',''
		DC.B	'\6',0
		ENDC
		IFNC	'\7',''
		DC.B	'\7',0
		ENDC
		IFNC	'\8',''
		DC.B	'\8',0
		ENDC
		IFNC	'\9',''
		DC.B	'\9',0
		ENDC
		ENDM

PIC_TAB		MACRO	* room name
		IFD	PIC_\1
		DC.W	PIC_\1-*
		ENDC
		IFND	PIC_\1
		DC.W	0		; 0 => no pics for this room
		ENDC
		ENDM




	XDEF	PictureResources

	CNOP	0,2

PictureResources

	include	"roompicttab.i"

* produced on Tue Oct 17 01:03:28 2017

* checksum = bf489a63
