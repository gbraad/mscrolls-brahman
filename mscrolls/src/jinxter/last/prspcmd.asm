******************************************************
*
*	%W%
*	%G%
*
******************************************************

		SECTION		"ADV",CODE

	include	"equates.i"
	include	"macros.i"
	include	"verbequ.i"
	include	"nounequ1.i"
	include	"nounequ2.i"
	include	"scenequ.i"


	XREF	VERB,CHEAT,CONVNPC,SEATED,RND.D1,SUBJECT
	XREF	SUBJECT2,NOUN,NOUNBUF,NPCCMDPT,PR20,NPCQUEST
	XREF	NPCMODEL,NPC.LBF1,P.SAY,P.BOTN
	XREF	DO.SCN,PRTTXT,P.SAY




    IFND	wantNOQUOTES

	XDEF	PRSPCMD,PR.SPEC,PRSPDO

* Base scenario version

*-----------------------------------
*
* PRSPCMD - this deals with giving NPC's commands
*
* in The Pawn the horse stuff was here, in the Guild Miller/Gatekeeper bits
*
* we get here when stuff like 'miller, stop windmill' is hit
* 
	
PRSPCMD

	
	CMP.B	#SECT.DIR,D6	;IS A STRAIGHT DIRECTION?
	BNE.S	10$
	MOVE.W	#VNGO,VERB(A4)
	BRA.S	20$
 
10$
	MOVE.W	D5,VERB(A4)
	
20$
	LEA	OrderNPC(PC),A0
	MOVE.W	CONVNPC+4(A4),D0	;noun # of npc in question
	MOVE.W	VERB(A4),D5		;verb in question

 
30$
	MOVE.W	(A0)+,D1		;get verb no. we're looking at
	BMI.S	40$			;forced match (-1)
	CMP.W	D1,D5
	BEQ.S	40$		
	ADDQ.W	#2,A0			;keep in phase with table
	BRA.S	30$

* ok, verb matches, does npc?

40$
	MOVE.W	(A0)+,D1		;get noun# we're looking for
	BEQ.S	90$			;not in table, exit 'definitely not'
					;test here 'cos 0 is allowed verb#
	BMI.S	50$			;forced match?
	CMP.W	D1,D0			;right npc?
	BNE.S	30$			;nope...
50$

	MOVE.L	CONVNPC(A4),A0		;this is probably pointless..

	MOVE.W	D0,SUBJECT(A4)		;so verbs/verbsubs know its an NPC
	MOVE.B	#1,SUBJECT2(A4)		;one day Hugh will tell me....
	MOVE.L	(A7)+,D0		;GET IT (I think this is just a stack pop - Ken)

	CLR.W	NOUN(A4)		;ZAP NOUNBUFF FOR NEW CMD
	CLR.W	NOUNBUF(A4)
	BRA	NPCCMDPT		;go parse command
	
* exit via here if not doing anything/have done all that's going to happen
* this is a quiet exit - no more will be printed

80$
	MOVE.L	(A7)+,D0	;DOWN
	DOGO	PR20

* exit via here to let 'definitely not' happen

90$
	DOGO	P.SAY

***
*
* OrderNPC - table of things u can tell NPC's to do
*
* format:
*
* Order	NPC (noun#),VERB
*
* where 'any' lets anything go
*
**

Order	MACRO	* npc,verb,noun1,conj,noun2,code

      IFNC	'any','\2'
	DC.W	VN\2
      ENDC
      IFC	'any','\2'
	DC.W	-1
      ENDC

      IFNC	'any','\1'
	DC.W	N\1
      ENDC
      IFC	'any','\1'
	DC.W	-1
      ENDC

	ENDM


OrderNPC

	DC.W	$FFFF,0		;NB two word terminator

*--------------------------------
*
*	PR.SPEC
*	
* SPECIALS FOR NPC INTERACTION
* LIVE HERE
*
*	ENTRY:
*	 D2=PERSON
*	 CONVNPC+4 = NPC IN CONVERSATION
*	 CONVNPC   = DATA ON NPC
*
* we get here when u ask an npc a question eg, guru,why are u laughing

PR.SPEC
	
	LEA	L.PRSP(PC),A0
02$
	MOVE.W	(A0)+,D1
	BEQ.S	90$		;NOT IN LIST
	CMP.W	CONVNPC+4(A4),D1
	BEQ.S	10$		;OK
	ADD.L	#12,A0		;SKIP TO NEXT SECTION
	BRA.S	02$		;AGAIN
10$
	MOVE.W	(A0)+,D1
	BMI.S	15$		;SKIP
	CMP.W	NPCQUEST(A4),D1
	BNE.S	02$		;FAIL
15$
	MOVE.W	(A0)+,D1	;GET MODEL
	BMI.S	20$		;OPTIONAL
	CMP.W	NPCMODEL(A4),D1	;SAME?
	BNE.S	02$		;FAIL
20$
	MOVE.W	(A0)+,D1	;GET MAIN NOUN
	BMI.S	30$		;OPTIONAL
	CMP.W	#2,D1		;3RD OR NOUN?
	BLT.S	25$		;NO, IS PERSON CHK
	CMP.W	NPC.LBF1(A4),D1	;AS IN BUFFER?
	BNE.S	02$		;NO, FAIL
	BRA.S	30$		;OK PASS
25$
	CMP.W	D2,D1		;AS PERSON?
	BNE.S	02$		;FAIL
30$
	MOVE.W	(A0)+,D1	;GET VERB
	BMI.S	40$		;OPTIONAL
	CMP.W	VERB(A4),D1
	BNE.S	02$		;FAIL
40$
	MOVE.W	(A0)+,D1	;INDIRECT NOUN
	BMI.S	50$		;OPTIONAL
	CMP.W	NOUNBUF(A4),D1
	BNE.S	02$		;NOPE
50$
*
*
*  SUCCESS!
*
	MOVE.W	(A0),D1		;DISP
	LEA	PRSPDO(PC),A0
	ADD.W	D1,A0
	JSR	(A0)
	MOVE.L	(A7)+,D0	;GET RET
	DOGO	PR20		;EXIT

90$
	RET

*--------------------------------
*
*
*
* FORMAT FOR SP.SPECIALS
*
* /NPC (ESSENTIAL)
* /QUEST (OPTIONAL=-1) ,
* /MODEL (OPTIONAL=-1),
* /DIRECT OBJECT (OPTIONAL=-1)
*         0=IST PERSON
*         1=2ND PERSON
*      NOUN= MAIN NOUN
* /VERB (OPTIONAL =-1)
* /INDIRECT OBJECT (OPTIONAL=-1)
* /ADDR OF HANDLER (REL TO PRSPDO)
*
*--------------------------------
	
L.PRSP

* TABLE OF SPECIALS

	IFNE	0		;use this one as an example
	
	       DC.W	NGURU
	       DC.W	4		;WHY?
	       DC.W	-1		;NO MODEL
	       DC.W	1		;YOU
	       DC.W	VNLAUGH		;LAUGHING
	       DC.W	-1		;NONE
	       DC.W	PRSP1-PRSPDO

	ENDC

	       DC.W    0             	;TERMINATE
	
	ENDC	;!wantNOQUOTES

*--------------------------------
*	
* NON-STANDARD REPLYS
*
* Does things like the SNOWMAN always failing to communicate speil
*

	XDEF	SP.ODREP
	XREF	AskedNTimes,BaseTMess

SP.ODREP

	CMP.W	#NBUS_DRIVER,D0
	BNE.S	10$
* Driver grunts and points at sign behind him, bus swerves
	SCN	314
	BRA.S	80$

10$
	CMP.W	#NCROWD,D0
	BNE.S	30$
* The passengers look the other way and ignore you
	SCN	315
	BRA.S	80$

30$
	CMP.W	#NINSPECTOR,D0
	BNE.S	40$
	TEST_W	AskedNTimes(A4)		;MI=> seen ticket
	BPL.S	32$
	SCN	104
* The inspector is busy with the other passengers right now.

	BRA.S	80$
32$
	CMP.W	#BaseTMess+1,AskedNTimes(A4)	;Asked for ticket yet?
	BGT.S	80$			;=> Yep, (timer will do messages)
	SCN	278
* Hang on, I'll be with you in a tick.

	BRA.S	80$

40$
	CMP.W	#NCLOCKMAKER,D0		;Addressing clockmaker?
	BNE.S	90$
	SCN	789
* The old codger is a trifle deaf. He can't hear you.

80$
	POPSP
	SETNE

90$
	RET
 
*--------------------------------
*
*  SP.SAY, CALLED BY P.SAY(2)
*          DOES STUFF LIKE
*          GURU, BETWEEN FITS..
*
* ACTUAL STUFF GOES HERE
*
*--------------------------------
	XDEF	SP.SAY
 
SP.SAY
	
	RET
	


*--------------------------------
*	
* ROUTINES TO HANDLE SPECIAL
* QUESTIONS and commands

* label used to calc address of these bits of code

PRSPDO

*--------------------------------
*
* Specials required by above tables can live down here..
*
*--------------------------------

	END
