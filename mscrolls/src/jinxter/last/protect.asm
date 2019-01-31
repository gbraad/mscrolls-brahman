******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

****************************************
*
*   PROTECT.ASM
* 
*   AJS -7/7/1986
*
* Jinxter version 17/11/87 - Doug
*
****************************************


* NB !!!!!! the SCN calls in this code should really be MSG calls
*           to be fixed... ('cos various bits are NOT scenario dependent)

* This routine when called will.
* 1.  ask you for the word on page n, line n word n
* 2.  compare it with the known word
* 3.  if okay - continue
* 4.  if not okay allow 3 tries.
* 5.  crash!!!!

	include	"equates.i"
	include	"macros.i"
	include	"scen.i"

	XDEF    PROTECT2,PROTECT
	XREF    POSWRD,SCR_IGNO,PRTNUM,PRTTXT,INBUFF,SEARCH,INPUT,FNDWRD
	XREF    DO.SCN,PROT_FLG,MATCHBUF,CLOCK,RNDGEN

	XREF	ProtSize,ProtList,INSIST,NOUNBUF,ProtData

PROTECT:

	CMP.W    #100,CLOCK(A4)
	BHI.S    PROTECT2
ProtExit
	RET

PROTECT2:

	TEST_B  PROT_FLG(A4)                    ;already done it?
	BNE.S   ProtExit
10$
	ST      PROT_FLG(A4)                    ;set flag saying done
	PUSH_L  D0-D7/A0-A6               ;save reggies:


15$:
	IFNE	M68000

	CALL     RNDGEN                   ;returns random in d0
	AND.W	 #$3FF,D0
	CMP.W	 #ProtSize,D0		  ; is it still too big?
	BGE	 15$			  ; Yes, so retry.

	ENDC

	IFEQ	M68000

	LEA	ProtData(A4),A0		;point at our fake long word

	ENDC

	CALL_S   PRINTLINE                ;print the question..
	                                  ;and return with a pointer in
	                                  ;A0 TO sect no. word no.
	                                  ;the word.
	PULL_L  D0-D7/A0-A6         ;restore life.
	RET


PRINTLINE:

* this will ask the question...

*
* format of protlist is:
*
* word of col# (2 bits), page# (5 bits), line# (5 bits) and word# (4 bits)
* word of section (top 6 bits) and word (bottom 10 bits)
*


* D0 Contains the index no...

	IFNE	M68000

	MOVE.L  #ProtList,A0             ;the list of the tables
	ADD.L   A4,A0
	LSL.W   #2,D0                   ;multiply by 4 for index
	ADD.W   D0,A0                   ;a0 now points to correct word.

	ENDC

* fake up a matchbuf in NOUNBUF to give to INSIST
* (I know nounbuf is ok here 'cos protection called from screen level)

	LEA	NOUNBUF(A4),A1		;INSIST wants A1 -> fake matchbuf

	MOVE.B  2(A0),D0		;section no.
	LSR.B   #2,D0                   ;shift it right 2 coz its 6 bits
	AND.W   #$3F,D0                 ;6 Bits
	MOVE.W  D0,(A1)+
	MOVE.W  2(A0),D0                ;word no.
	AND.W   #$3FF,D0                ;10 Bits
	MOVE.W  D0,(A1)+
* Get the first one.


	MOVE.B  (A0),D6                 ;the page no.
	LSR.B   #1,D6                   ;shift it down 1
	AND.W   #$1F,D6                 ;we only want 5 bits
	FIXED_SCN   Prot1		;... page no
	DO    PRTNUM                  ;print out the number

	MOVE.B	(A0),D6			;get column no.
	LSR.B	#6,D6			;from top two bits
	AND.W	#3,D6
	FIXED_SCN	Prot5		;', column '
	DO    PRTNUM                  ;print out the number

	MOVE.W  (A0),D6                 ;get the line no.
	LSR.W   #4,D6                   ;move it down 4
	AND.W   #$1F,D6                 ;only want bottom 5 bits
	FIXED_SCN   Prot2		;...line

	DO    PRTNUM                  ;print it 


	MOVE.W  (A0)+,D6                ;Word no.  
	AND.W   #$0F,D6			;we only want 4 bits 

	FIXED_SCN   Prot3		;word no...
	DO    PRTNUM

	FIXED_SCN   Prot4		;ta alot now enter de word!




GETCOMP:

* This will get the word that the punter has typed in.
* and compare it with the know word no. sect no.
* if okay it will return (up to 3 tries.)
* else crash.!!!!



        MOVE.W  #3,D0              ;three tries.
10$
        FIXED_SCN     Enter                ;<"^Please enter word :  ">

  IFNE   Wimp!IBM
      ST       SCR_IGNO(A4)        ;Ignore input from pulldown scrolls for a bit
  ENDC

      CALL     INPUT               ;Get a line from keyboard

  IFNE   Wimp!IBM
      SF       SCR_IGNO(A4)         ;Stop ignoring scrolls
  ENDC

      LEA.L    INBUFF(A4),A6        ;Get addr of filename
      MOVE.W   D0,-(A7)
      CALL     FNDWRD               ;lose initial spaces
      MOVE.W   (A7)+,D0
      TEST_B   (A6)                 ;We do have something?
      BEQ.S    10$                  ;NE => yes
      MOVE.L   A6,POSWRD(A4)        ;Make sure rest of world knows


20$

* A6 points to the word....

        MOVE.W  D0,-(A7)
        CALL    SEARCH              ;return d5 = word d6= section.
        MOVE.W  (A7)+,D0
        LEA     MATCHBUF(A4),A0
	LEA	NOUNBUF(A4),A1
        MOVE.W  (A0)+,D6            ;section
        CMP.W   (A1),D6            ;section okay?
        BNE.S   30$
        MOVE.W  (A0)+,D6            ;Word
        CMP.W   2(A1),D6            ;word okay?
        BNE.S   30$
        FIXED_SCN     Thanks                 ;thank you for your time 
        RET


30$:
        SUBQ.W  #1,D0               ;have they run out of luck?
        BEQ.S   DEATH
        FIXED_SCN     Tryagain                 ;<"^Sorry please try again">
        BRA     10$
DEATH:
* for now..

      IFNE    YCHEATMORE
	PRINT   <'Your Amiga would have just crashed'>
	RET
      ENDC


	FIXED_SCN	Removedisk	;Please remove disk & power off
10$
	BRA.S   10$

	END
