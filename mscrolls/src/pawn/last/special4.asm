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
*	@(#)special4.asm	2.2
*	2/8/87
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.SPECIALS4
*
*--------------------------------




	include	"equates.asm"
	include	"verbequ.asm"
	include	"nounequ1.asm"
	include	"nounequ2.asm"
	include	"roomequ.asm"
	include	"macros.asm"
	include	"scoreref.asm"


      XDEF     SP.POINT,SP.SHINE,SP.DRAGO,SP.KRON3,SP.MIX,SP.SPLIT,SP.DEMON
      XDEF     SP.DEVIL,SP.DEVL2,SP.ALCH,SP.EXIT,SP.SNOW,SP.PRIN2,SP.CLOSE
      XDEF     SP.OPEN,SP.THROW

      XDEF RESETDRAGON    

      XREF     F_DEVIL,DO.SCORE
      XREF     F_ALCH,F_COLOUR,F_DRAG,F_DRAG2,F_DRAG3,F_KMEET3,F_KMEET2
      XREF     P.LOC,P.SCN,GETDATA
      XREF     DEAD,SETOUT,CARRIED,SETMODE
      XREF     FIXCONT,YCANT,CHKNPC,LINE,CARRIED
      XREF     NOUNBUF,NOUNBUF2,NOUNBUF3
      XREF     WHOOPS,ROOM,SUBJECT,TIMER5,COUNT.DE,VERB,PRINC.CO
      XREF     SNOWLOC,PRTTXT,DO.SCN
      XREF   REMASTER
    
SP.POINT 

	CMP.W   #NSHAPES,D0
	BEQ.S   10$
	CMP.W   #NSHADOWS,D0
	BNE.S   90$
 
10$
	MOVE.L  (A7)+,D7            ;ZAP STACK
	BSET    #7,F_DRAG(A4)        ;BIT 7 = HAS POINTED
	BEQ.S   SP.PNTSHN           ;CAN ONLY GIVE FIRST MSG ONCE!
90$
	RET
 
SP.SHINE
 
	CMP.W   #NWHITE,D5
	BNE.S   90$
	CMP.W   #NSHAPES,D0
	BEQ.S   10$
	CMP.W   #NSHADOWS,D0
	BNE.S   90$ 
10$
	MOVE.L  (A7)+,D7
	BSET    #6,F_DRAG(A4)       ;BIT 6 = HAS 'ILLUMINATED' THEM
	BEQ.S   SP.PNTSHN
90$
	RET
 
 
SP.PNTSHN
 
	MOVEQ   #79,D1              ;OH THERE.. SAYS DRAGON
	CMP.B   #$C0,F_DRAG(A4)
	BEQ.S   10$
	MOVEQ   #78,D1              ;WHERE? I DONT SEE.. SAYS DRAGON
10$
	BRA     P.SCN
 
*--------------------------------
 
 
SP.DRAGO
 
	CMP.W   #RNCHAMBER2,ROOM(A4)
	BNE.S   90$
	TEST_B  F_DRAG2(A4)
	BMI.S   SP.DRAGON2
 
	MOVE.W  #NADVENT,D0        ;CARRYING ADVENTURER?
	CALL    CARRIED
	BEQ.S   90$                ;YES = NO NEED TO DO REST
 
	BSET    #0,F_DRAG2(A4)
	BNE.S   05$                ;A FREE MOVE!
	SCN     76
90$:
	RET

05$
	BSET    #1,F_DRAG2(A4)
	BEQ.S   90$
10$
	TEST_W  F_DRAG(A4)           ;DONE A POINT/SHINE?
	BEQ.S   DRAGKILL             ;DEAR ME...
	BSET    #2,F_DRAG2(A4)
	BEQ.S   90$
	CMP.B   #$C0,F_DRAG(A4)      ;DONE BOTH?
	BNE.S   DRAGKILL             ;ANOTHER WAY TO DIE....
	BSET    #3,F_DRAG2(A4)
	BEQ.S   90$
	
	SCN     222

; Get here if killed by the dragon!
DRAGKILL
	SCN     77                  ;DRAGON GETS U
    TEST_B  REMASTER(A4)
    BEQ.S   90$

    TEST_W  F_KMEET3(A4)        ; is kronos dead?
    BPL.S   10$                 ; no, zapped to ledge
    MOVE.W  #RNWORKSHOP,ROOM(A4)
    BRA.S   20$                 ; if dead zapped to workshop
10$    
    MOVE.W  #RNLEDGE2,ROOM(A4)
20$    
    CALL_S  RESETDRAGON
90$    
	BRA     DEAD

RESETDRAGON    
    TEST_B  REMASTER(A4)
    BEQ.S   90$
    CLR.B   F_DRAG(A4)
    CLR.B   F_DRAG3(A4)

    ;;  kmeet3 also stores whether kronos is dead, so dont reset this!
    TEST_W  F_KMEET3(A4)
    BMI.S   90$
    CLR.W   F_KMEET3(A4)
    CLR.B   F_DRAG2(A4)
90$    
    RET
    
SP.DRAGON2 
 
	TEST_B  F_DRAG3(A4)
	BMI.S   DRAGKILL 
   
	MOVE.W  SUBJECT(A4),D1
	MOVE.W  #NWAND,D0
	GETINFO        
	CMP.W   8(A0),D1            ;MUST HAVE THE WAND!  
	BNE.S   80$                 ;KILLED OFF BY DRAGON
	MOVE.W  #NHAT2,D0           ;POINTY HAT
	GETINFO        
	CMP.W   8(A0),D1
	BNE.S   80$
	BTST    #2,6(A0)            ;POINTY HAT WORN?
	BEQ.S   80$
	MOVE.W  #NCLOAK,D0
	GETINFO        
	CMP.W   8(A0),D1
	BNE.S   80$
	BTST    #2,6(A0)            ;CLOAK WORN?
	BEQ.S   80$
 
	BSET    #0,F_DRAG3(A4)
	BNE.S   60$                 ;ONLY ONE TURN! 
	MOVEQ   #80,D1
	BRA     P.SCN
 
60$
	BSET    #1,F_DRAG3(A4)
	BNE.S   80$
	RET
 
80$
	SCN     236                 ;you're not kronos!
	BRA     DRAGKILL
 
*--------------------------------
 
SP.MIX.CHK
 
10$
	MOVE.W  (A1)+,D0
	BEQ.S   90$
	CMP.W   #NRED,D0
	BNE.S   20$
	BSET    #2,(A5)            ;(A5)=F_COLOUROURS
20$
	CMP.W   #NBLUE,D0
	BNE.S   30$
	BSET    #1,(A5)
30$
	CMP.W   #NGREEN,D0
	BNE.S   10$
	BSET    #0,(A5)
	BRA.S   10$
 
90$
	RET
 
 
 
SP.MIX
 
	PUSH_L  D0/D1/D5/A0/A5 ;SAVE STACK
 
	LEA     F_COLOUR(A4),A5       ;SAVE SOME MEMORY!
	CLR.B   (A5)               ;ASSUME NONE TO BEGIN WITH
 
	LEA     NOUNBUF(A4),A1     ;CHECK NOUNBUF 1
	CALL_S  SP.MIX.CHK
	LEA     NOUNBUF2(A4),A1    ;CHK NOUNBUF 2
	CALL_S  SP.MIX.CHK
	LEA     NOUNBUF3(A4),A1    ;CHK NOUNBUF 3
	CALL_S  SP.MIX.CHK
 
	CMP.B   #7,(A5)            ;MIXED EM ALL?
	BEQ.S   10$                ;YES - MAKE WHITE!
 
	CMP.B   #3,(A5)            ;MIXED JUST BLUE AND GREEN?
	BEQ.S   20$                ;OK CYAN FOR A BIT
 
	CMP.B   #6,(A5)            ;JUST RED AND BLUE?
	BEQ.S   30$                ;OK MAGENTA
 
	CMP.B   #5,(A5)            ;JUST RED AND GREEN?
	BEQ.S   40$                ;YELLOW
  
90$
	PULL_L  D0/D1/D5/A0/A5
	RET
 
10$
	MOVEQ   #NRED,D0           ;I KNOW RED IS LESS THAN $7F!
	CALL    SETOUT             ;KILL IT
	MOVEQ   #NBLUE,D0          ;DITTO BLUE
	CALL    SETOUT
	MOVEQ   #NGREEN,D0         ;AND GREEN
	CALL    SETOUT
 
	MOVE.W  #NWHITE,D0         ;WHITE IS *NOT* LESS THAN $7F!!
	GETINFO        
	CLR.L   6(A0)              ;PUT IT ON INV ELSE SCREW CONTAINERS
	MOVE.L  (A7)+,D0           ;POP STACK
	SCN     72                 ;'COLOURS MELD TOGETHER...
 
	ST      WHOOPS(A4)         ;DONT LOOK FROM PROCESS!!  
	 
	BRA.S   90$
 
20$
	MOVE.W  #277,D1            ;CYAN MSG
	BRA.S   85$                ;'BUT QUICKLY RETURN TO THEIR ORIGINAL FORM
 
30$
	MOVE.W  #281,D1            ;MAGENTA MSG
	BRA.S   85$
40$
	MOVE.W  #279,D1            ;YELLOW MSG
85$
	SCN     284                ;MINGLE A LITTLE...
	CALL    P.SCN              ;NEAT HOW 'SCN' DOESN'T STOMP D1 EH?
	SCN     283                ;RETURN TO ORIGINAL
	 
	MOVE.L  (A7)+,D0           ;POP STACK - AVOID CHK4PART MSG
	ST      WHOOPS(A4)         ;DONT LOOP FROM PROCESS!!!!   
	BRA     90$
 
*--------------------------------
 
SP.SPLIT
 
	CMP.W   #NWHITE,D0
	BNE.S   90$
	CMP.W   #NPRISM,D5
	BNE.S   90$
	CALL    SETOUT  
	SCORE   SPLCO               ;HMMM ???
	SCN     73
	CALL    LINE
	MOVE.L  (A7)+,D1            ;FIX STACK
	CLR.L   D1
	MOVE.W  ROOM(A4),D1
	MOVEQ   #NRED,D0   
	CALL_S  10$
	MOVEQ   #NBLUE,D0    
	CALL_S  10$  
	MOVEQ   #NGREEN,D0  
10$
	GETINFO        
	MOVE.L  D1,6(A0)
90$
	RET
 
*--------------------------------
 
SP.KRON3
 
	CMP.W   #RNWORKSHOP,ROOM(A4)       ;WE IN KRONOS' LAIR?
	BNE.S   90$                        ;NOPE...
	MOVE.W  F_KMEET3(A4),D1               ;HAS THIS HAPPENED BEFORE?
	BMI.S   90$                        ;KRONOS HAS BEEN KNOBBLED
	BNE.S   60$
 
	ADDQ.W  #1,F_KMEET3(A4)               ;SO WE KNOW WHERE WE ARE AT
	SCN     347                        ;KRONOS GLARES AT U
	TEST_B  F_KMEET2(A4)                  ;HAS KR GIVEN US CHEST?
	BMI.S   10$                        ;YES = OK
	SCN     348                        ;NO = U DIE
    BRA.S   28$                        ;dead!
 
10$
	MOVE.W  #NADVENT,D0
	GETINFO        
	CALL    CARRIED
	BEQ.S   25$                       ;HO HO HO - U FELL FOR IT 3 SOULS...
20$
	MOVE.W  #NBOTTLE1,D0
	GETINFO        
	CALL    CARRIED
	BEQ.S   60$                       ;IF POTION BOTTLE CARRIED U GET A GO
26$
	SCN     209                   ;ZAPPED TO START
    CALL    RESETDRAGON    
	MOVE.W  #RNSTART,D1
	BRA     SETMODE
25$
	SCN     34                   ;HO HO - FELL FOR PLY N-1 (OR 2??) ((OR 3!?)
	SCN     348
28$    
    MOVE.W  #RNSTART,ROOM(A4)
    CALL    RESETDRAGON
	BRA     DEAD
60$
	CMP.W   #1,F_KMEET3(A4)          ;ONE GO TO THROW BOTTLE
	BNE.S   26$                   ;TOO LATE - KILLED OR ZAPPED TO START
	ADDQ.W  #1,F_KMEET3(A4)
90$
	RET
*--------------------------------
 
 
SP.THROW
*                        -EDS HUGH
 
	CMP.W   #NBOTTLE1,D0              ;THROWING POTION BOTTLE?
	BNE.S   20$                       ;NO
	LEA     LIST.THROW(PC),A1
10$
	MOVE.L  (A1)+,D1
	BEQ.S   80$                       ;NOT IN TABLE
	CMP.W   D1,D5                     ;AT HIM?
	BNE.S   10$                       ;NO, NEXT
	MOVE.L  (A7)+,D2                  ;DUMP RET
	SWAP    D1                        ;GET ADDR/MSG
	BCLR    #15,D1                    ;HIBIT?
	BNE     P.SCN                     ;NO, IS A MSG
	CALL    SETOUT                    ;NO BOTTLE
	JMP     -4(A1,D1.W)               ;GO THERE
80$
	BTST    #6,4(A5)                  ;AT AN NPC?
	BEQ.S   90$                       ;NO, EXIT
	CALL    SETOUT
	SCN     195                       ;YOU MISS, IT SEEPS AWAY
70$
	MOVE.L  (A7)+,D1                  ;DUMP RET
90$
	RET
20$
	CMP.W   #NKRONOS,D5              ;SOMETHING AT K?
	BNE.S   90$
	SCN     93                       ;HIS SHIELD PROTECTS
	BRA.S   70$
    
 
LIST.THROW
 
	DC.W     SP.THROW.ADVENT-*,NADVENT
	DC.W     SP.THROW.KRONOS-*,NKRONOS
	DC.W     194+HI,NGURU
	DC.W     SP.THROW.GUARDS-*,NGUARDS
	DC.W     SP.THROW.HORSE-*,NHORSE
	DC.L     0
 
SP.THROW.ADVENT  

    ;;;  !! BUG reported here. CHKNPC will pop a ret, but we have
    ;; already done so!!
	BTST    #1,(A5)             ;ADV DEAD ALREADY?  
	BNE     CHKNPC              ;AND PRINT ERROR
  
 
SP.THROW.HORSE
 
	CALL    SETOUT              ;BYE BYE BOTTLE
	SCN     192                 ;HORSE SHOOTS IT IN MID AIR
	RET 
 
SP.THROW.GUARDS
	 
	SCN     196                 ;GUARD TRIES TO CATCH IT
	BRA     DEAD
  
 
SP.THROW.KRONOS
 
*      CMP.W   #1,F_KMEET3(A4)        ;HMMMM....
*      BNE.S   10$
	SCORE   KILLK
*      MOVE.W  #191,D1            ;MSG DONE BY TIMER
*      CALL    P.SCN              ;SO LEAVE IT OUT
	BSET    #1,(A5)            ;KRONOS IOS DEAD!!!!
	CALL    SETOUT             ;BOTTLE NOW OUT!
	MOVE.W  #-1,F_KMEET3(A4)      ;SO WE DONT GET ZAPPED NEXT GO!
	MOVE.B  #1,TIMER5(A4)      ;REST OF MSGS
10$
	RET
 
 
*--------------------------------
 
 
SP.DEMON
 
	LEA     COUNT.DE(A4),A6
	CMP.W   #RNHELL,ROOM(A4)
	BNE.S   10$
	MOVE.W  #-1,(A6)
10$
 
	CMP.W   #RNCAVERN6,ROOM(A4)
	BNE.S   90$ 
	ADDQ.W  #1,(A6)
	BEQ.S   30$
	CMP.W   #3,(A6)
	BNE.S   90$
20$
	SCN     211
	SCN     69
	BRA     DEAD
30$
	SCN     210
90$
	RET

*--------------------------------
 
SP.ALCH
 
	LEA     F_ALCH(A4),A6
	CMP.W   #RNLAB,ROOM(A4)
	BNE     90$
	BSET    #0,(A6)
	BNE.S   20$
	SCN     74
	MOVEQ   #75,D1
10$
	BRA     P.SCN
20$
	BSET    #1,(A6)
	BNE.S   30$
	MOVE.W  #223,D1
	BRA.S   10$
30$
	BTST    #2,(A6)             ;LEFT ROOM YET?
	BEQ.S   50$                 ;NOPE TIME TO GET THROWN OUT? 
	BSET    #3,(A6)
	BNE.S   40$
	MOVE.W  #229,D1
	BRA.S   10$
40$
	BTST    #4,(A6)
	BNE.S   50$ 
	MOVE.W  #NLUNCH,D0
	GETINFO        
	MOVE.W  SUBJECT(A4),D1
	CMP.W   8(A0),D1
	BEQ.S   50$
	MOVE.W  #NRICE,D0
	GETINFO        
	CMP.W   8(A0),D1
	BEQ.S   50$
	SCN     223
45$
	SCN     228                 ;OK THEY THROW U OUT
	CLR.B   1(A6)               ;F_ALCHH2
	MOVE.W  #RNPASSAGE,ROOM(A4)
	MOVE.W  #NPASSAGE,D0
	BRA     P.LOC
50$
	BSET    #4,(A6)
	BTST    #5,(A6)
	BNE.S   99$
	ADDQ.B  #1,1(A6)
	CMP.B   #4,1(A6)
	BNE.S   99$
	CLR.B   1(A6)
	BRA.S   45$
 
90$    
	TEST_B  1(A6)
	BEQ.S   99$
	BSET    #2,(A6)
99$
	RET
 
 
 
*--------------------------------
 
SP.DEVIL
 
 
	CMP.W   #RNHELL,ROOM(A4)
	BNE     90$
	BSET    #0,F_DEVIL(A4)
	BNE.S   10$
	SCN     231                ;Eyes say 'Hello'
	RET

10$
	BTST    #1,F_DEVIL(A4)       ;LEFT?
	BEQ.S   90$
	BTST    #7,F_DEVIL(A4)       ;BEEN GIVEN POTION BOTTLE?
	BEQ.S   90$
	BTST    #2,F_DEVIL(A4)        ;GOT SOUL?
	BNE.S   20$                 ;YES
	SCN     234                 ;'DEATH WILL NOW REIGN...'
	BRA     DEAD
20$
*      MOVE.W  #NBAND,D0           ;DONE WI 'GIVE DEVIL AEROSOUL'
*      GETINFO        
*      CALL    SETOUT    
*      SCN     233
	MOVE.W  #NAEROSOUL,D0
	GETINFO        
	CALL    CARRIED
	BEQ.S   90$
	SCN     339                 ;REAL GORY MSG 'SUPPING WARM BLOOD'
	BRA     DEAD
  
90$
	RET
 
  
*--------------------------------
 
 
SP.DEVL2
 
	CMP.W   #NDEVIL,D5          ;COPOUT GENERATED FROM DEVIL?
	BNE.S   90$
	MOVE.L  (A7)+,D1            ;STACK..
	MOVE.L  (A7)+,D1            ;STACK.. TWICE COS WE GOT HERE FROM SP.ASK
	BSET    #7,F_DEVIL(A4)        ;FIRST MEET?
	BNE.S   10$                 ;BNE = NOPE
	SCN     232                 ;BARGAIN TIME...
	SCORE   DEVIL               ;POINTS
	ST      WHOOPS(A4)          ;PREVENT 'ASK DEVIL ABOUT PEOPLE'
	MOVE.W  #NBOTTLE1,D0        ;POTION BOTTLE
	GETINFO                     ;PUT IT ON BRIDGE THEN
	MOVE.W  #RNBRIDGE3,8(A0)
	CLR.W   6(A0)
10$ 
	MOVE.W  #RNBRIDGE3,ROOM(A4)
	SCN     235
90$
	RET
 
 
*--------------------------------
 
 
SP.SNOW
	
	MOVE.W  #NWATER,D0
	GETINFO        
	MOVE.W  SUBJECT(A4),D1
	CMP.W   8(A0),D1
	BEQ.S   20$                 ;WATER&SNOW MUST BE CONTAINED!
	MOVE.W  #NSNOW,D0
	GETINFO        
	CMP.W   8(A0),D1
	BEQ.S   20$
	BTST    #0,5(A0)            ;SNOW PSUDO?
	BNE.S   40$                 ;YES=LEAVE IT
 
	CLR.L   D1
10$
	GETINFO        
	TEST_W  6(A0)
	BEQ.S   15$
	MOVE.W  8(A0),D0
	BRA.S   10$
15$
	MOVE.W  8(A0),D1            ;TOP LOC OF SNOW
	CMP.W   SUBJECT(A4),D1
	BEQ.S   30$                 ;OK LEAVE IT
	CMP.W   ROOM(A4),D1
	BEQ.S   30$
 
20$
	MOVE.W  #NWATER,D0
	GETINFO        
	CALL    FIXCONT
	CALL    SETOUT              ;OUT
 
25$
	MOVE.W  #NSNOW,D0
	GETINFO        
	MOVE.L  #SNOWLOC,6(A0)
	BSET    #0,5(A0)
40$
	RET
 
30$
	MOVE.W  #NWATER,D0
	GETINFO        

* Bug spotted by Guy...

*       BTST    #7,6(A0)            ;OUT?
*       BEQ.S   40$                 ;NO=CANNOT BE MELTED SNOW THEN!
	MOVE.W  ROOM(A4),D1
	CMP.W   #RNPLATEAU,D1
	BEQ.S   40$                 ;WONT MELT ON PLATEAUS
	CMP.W   #RNPLATEAU2,D1
	BEQ.S   40$  
	CMP.W   #RNPLATEAU3,D1 
	BEQ.S   40$   
	CMP.W   #RNPLATEAU4,D1  
	BEQ.S   40$  
	MOVE.L  A0,A5
	MOVE.W  #NSNOW,D0
	GETINFO        
	MOVE.L  6(A0),6(A5)
	BRA.S   25$
 
 
*--------------------------------
 
 
SP.EXIT
 
	CMP.W   #NMAZE,D0
	BNE.S   90$
	CMP.W   #RNMAZE,ROOM(A4)
	BNE.S   90$
	TEST_B  D1                  ;WAS NOUN SPECIFIED?
	BNE.S   90$                 ;SINGV LEAVES VERB # HERE!
	SCORE   MAZE                ;PROBABLY 0!
	MOVEQ   #RNCAVERN4,D1
	CALL    SETMODE
	MOVE.L  (A7)+,D0
90$
	RET
 
*--------------------------------
*
* SP.PRIN2 - COUNTS COPOUTS
*
*--------------------------------
  
 
SP.PRIN2
	
	CMP.W   #NPRINCESS,D5      ;ASKING PRINCESS?
	BNE.S   90$                ;NOPE = FORGET IT!
 
	ADDQ.W  #1,PRINC.CO(A4)
	CMP.W   #3,PRINC.CO(A4)
	BNE.S   90$
	CMP.W   #RNJAIL,ROOM(A4)
	BEQ.S   10$
	CMP.W   #RNLANDING,ROOM(A4)
	BNE.S   20$
10$ 
	MOVE.W  #203,D1             ;KILLS PRINCESS ON SLOPE
15$
	MOVE.W  #NPRINCESS,D0
	GETINFO        
	CALL    SETOUT              ;BYE BYE PRINCESS
	MOVE.L  (A7)+,D0            ;KILL STACK  TWICE COS OF SP.ASK
	MOVE.L  (A7)+,D0            ;KILL STACK
	BRA     P.SCN
20$
	MOVE.W  #205,D1             ;SHE PISSES OFF
	BRA.S   15$
 
90$
	RET
 
 
*--------------------------------
 
 
SP.CLOSE
 
	CMP.W   #NWINDOW1,D0
	BNE.S   70$
	PUSH_L  D0/A0
	MOVE.W  #NWINDOW2,D0       ;DO OTHERONE!
	GETINFO        
	BCLR    #4,(A0)            ;ZAP 'OPEN'
	BSET    #5,(A0)            ;DO CLOSE
	PULL_L  D0/A0
70$
	CMP.W   #NWINDOW2,D0  
	BNE.S   80$
	PUSH_L  D0/A0
	MOVE.W  #NWINDOW1,D0       ;DO OTHERONE!  
	GETINFO        
	BCLR    #4,(A0)            ;ZAP 'OPEN'
	BSET    #5,(A0)            ;DO CLOSE
	PULL_L  D0/A0
80$
	CMP.W   #VNCLOSE,VERB(A4)
	BNE.S   90$
	CMP.W   #NLIFTDOOR,D0
	BNE.S   90$
	MOVE.L  (A7)+,D1            ;POP
	BRA     YCANT 
90$
	RET
 
SP.OPEN
 
	CMP.W   #NNOTE,D0
	BNE.S   40$
	PUSH_L  D0/A0
	MOVE.W  #NSEAL,D0
	GETINFO        
	BSET    #2,(A0)            ;BREAK SEAL
	PULL_L  D0/A0
	RET
 
40$
	CMP.W   #NWINDOW1,D0
	BNE.S   50$
	PUSH_L  D0/A0
	MOVE.W  #NWINDOW2,D0       ;DO OTHERONE!
	GETINFO        
	BCLR    #5,(A0)            ;ZAP 'CLOSE'
	BSET    #4,(A0)            ;DO OPEN
	PULL_L  D0/A0
50$
	CMP.W   #NWINDOW2,D0  
	BNE.S   60$
	PUSH_L  D0/A0
	MOVE.W  #NWINDOW1,D0       ;DO OTHERONE!  
	GETINFO        
	BCLR    #5,(A0)            ;ZAP 'CLOSE'
	BSET    #4,(A0)            ;DO OPEN
	PULL_L  D0/A0
60$
	CMP.W   #NCHEST2,D0         ;MAHOG. CHEST?
	BNE.S   70$
	SCN     349                ;MSG ABOUT HOW U DIE DUE TO OPENING CHEST
	MOVE.L  (A7)+,D0
	BRA     DEAD
70$
 
	CMP.W   #NTOMES,D0
	BNE.S   80$
	SCN     350                ;THEY CRUMBLE TO NOTHING"
	CALL    SETOUT  
	MOVE.L  (A7)+,D0           ;POP
	RET
      
80$
	CMP.W   #VNOPEN,VERB(A4)
	BNE.S   90$
	CMP.W   #NLIFTDOOR,D0 
	BNE.S   90$
	MOVE.L  (A7)+,D1
	BRA     YCANT
	                      
90$
	RET
 
*-------------------------------- 


      END

