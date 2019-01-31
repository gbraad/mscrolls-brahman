******************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSG
*
*--------------------------------


    include "equates.i"
    include "nounequ1.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
    include "scenequ.i"
    include "scoreref.i"
    include "verbequ.i"


;******************************************
    XDEF    SP.GIVE
    XDEF    SP.GivePostMistress
    
*   XREF    SP.MEDDLE
    XREF    SETOUT,SUBJECT
    XREF    AskedNTimes,FIXCONT,P.MHAVSTOP
    XREF    BaseTMess       ;This is an EQUATE in scenrw.i
    XREF    BakerFed,BakerAngry,BakerFollows
    XREF    TIMERAFTER9,GuardSeenTicket,AskAboutJob
    XREF    TIMERAFTERB,V.FEED,DO.SCORE

    XREF    PostMistressGone,LeavePOffice
    XREF    REMASTER

SP.GivePostMistress
* if you show/give/feed/wave/throw the dead mouse at the postmistress
* she abandons the place in horror.
    CMP.W   #NDEAD_MOUSE,D0     ; is it the dead mouse?
    BNE.S   99$         ; nope go away
    CMP.W   #NPOSTMISTRESS,D5   ; and to the postmistress
    BNE.S   99$

*   EXG D5,D0           ; to set the postmistress out.
*   DO  SETOUT

    SCN 766         ;Aaaargh
    CALL    LeavePOffice        ;Fod her and set up safe/scales

    ST  PostMistressGone(A4)    ; so she doesn't enter again.
    SCORE   MUFFLEBELL      ;Effectively the same
    ADDQ.W  #8,A7           ; from where we called and
                    ; the verb routine.
99$
    RET


*------------------------------------

    XREF    Do_Bull,VERB,TIMERAFTER8

SP.GIVE

    CALL_S  SP.GivePostMistress ; for the mouse stuff
* NOTE -this happens before we check for food, as bus ticket is edible.

    ; check for giving food but not ticket ?
    CMP.W   #NTICKET,D0
    BEQ.S   10$
    MOVE.B  11(A0),D1
    AND.B   #$E0,D1     ; Top 3 bits 0 = not edible
    BEQ.S   10$
    POPSP
    BRA V.FEED
10$

* giving stuff to various people
    CaseOf  D5
    When    NBULL,SP.GiveBull-*
    When    NINSPECTOR,SP.GiveInspector-*
    When    NGUARD,SP.GiveGuard-*
    When    NPUBLICAN,SP.GivePublican-*
    When    NPOSTMISTRESS,SP.GivePM-*
    When    NBAKER,SP.GiveBaker-*
    EndCase

    RET

SP.GiveBull
    CMP.W   #VNSHOW,VERB(A4)    ;Dont want GIVE
    BEQ.S   10$
    SCN     826                 ; the bull ignores you.
    POPSP                       ; dont go back to give
    BRA.S   90$
10$    
    CALL    Do_Bull         ;Cope with bull (wont ret if coped)
90$
    RET

SP.GiveInspector:
* Showing/giving ticket to the inspector?

    CMP.W   #NTICKET,D0     ; ticket ?
    BNE.S   90$
    DO  CARRIED         ;Got it?
    BEQ.S   28$
    POPSP   

*(give checks this!)
*!!! NO IT DOESN'T      !!!
*!!! PLEASE check things before !!!
*!!! making edits like this !!!
*Paul.

    DOGO    P.MHAVSTOP      ;Must have it
28$
    TEST_W  AskedNTimes(A4)     ;How to abuse a flag presents...
    BPL.S   32$         ;Already seen it? PL => Nope
    SCN 277
* It's alright sonny, I only need to see it once.

    BRA.S   80$

32$
    CMP.W   #BaseTMess+1,AskedNTimes(A4)    ;Asked for it yet?
    BGT.S   34$         ;=> Yep
    SCN 278
* Hang on, I'll be with you in a tick.
    BRA.S   80$

34$
    TST_WET             ;Been chewing the ticket?
    BEQ.S   35$         ;=> nope
    SCN 274
* I do wish people wouldn't chew their tickets!^
35$
    SCN 275
* Nightingale lane eh? That's another two stops.

    OR.W    #$8000,AskedNTimes(A4)  ;Tell the world we've seen ticket
80$
    POPSP
90$
    RET

; This is a GIVE, as the CONTAINER is given ( the glass!)

SP.GivePublican:
; Giving drink to publican...?
    CMP.W   #NDRINK,D0          ; Give drink to publican?   
    BNE.S   90$
    MOVE.L  #REL.MASK+NPUBLICAN,6(A0)   ; publican takes it
    SCN 123             ; 'Thanks a lot'
                        ; but no other effect!
    MOVE.L  D0,-(SP)
    MOVE.W  #NBEER,D0
    DO  SETOUT          ;that loony Roddy forgot to do this!
    MOVE.L  (SP)+,D0
    POPSP
90$
    RET

********************************************************************************

SP.GiveGuard
    CMP.W   #NTRAIN_TICKET,D0
    BNE.S   10$         ;no
    TEST_B  GuardSeenTicket(A4) ;already happened?
    BNE.S   20$
    SCN 497         ;sees ticket,and gives it back
    CLR.B   TIMERAFTER9(A4)     ;and stops asking
    TEST_B  TIMERAFTER8(A4)     ;train left yet?
    BEQ.S   5$
    MOVE.B  #1,TIMERAFTER8(A4)  ;make it leave soon
5$
    MOVE.B  #5,TIMERAFTERB(A4)  ;the train will arrive now
    ST  GuardSeenTicket(A4)
    TEST_B  REMASTER(A4)
    BEQ.S   06$
    SCORE CATCHTRAIN            ;if you've paid for the ticket
06$
    BRA.S   80$         ;and exit
20$
    SCN 501         ;already seen it
    BRA.S   80$
10$
    CMP.W   #NTICKET,D0     ;the other ticket
    BNE.S   90$
    SCN 498         ;whats this
    DO  P.TNSTOP        ;noun '.'
    MSG LINE
80$
    POPSP
90$
    RET

    XREF    PostItem,Postage,PostDest,STOPRON,GetValue,TIMERAFTERE
; Give item to postmistress... to pay for postage??
SP.GivePM
    TST_MONEY           ; is object money
    BEQ.S   90$
    TEST_W  PostItem(A4)        ; Something posted?
    BEQ.S   10$
    TEST_B  TIMERAFTERE(A4)     ; IS she bending down??
    BNE.S   10$
    PUSH_L  D0/A0
    MOVE.W  PostItem(A4),D0     ; Check that the posted object...
    GETINFO
    CMP.L   #REL.MASK+NPOSTMISTRESS,6(A0)   ; Is still with her..
    PULL_L  D0/A0
    BEQ.S   20$
10$:
    SCN 479         ; I don't want the money.
    BRA.S   80$
20$:
    DO  GetValue        ; How much is it worth?
    CMP.W   Postage(A4),D1      ; Is it worth enough
    BEQ.S   40$
    BGT.S   30$         ; branch if not enough
    SCN 504         ; not enough!
    BRA.S   80$
30$:
    SCN 505         ; A tip! How nice!
    BRA.S   45$
40$:    
    SCN 499         ; 'Exact money. Thankyou'
45$:
    DO  SETOUT          ; Lose the coin
    SCN 511         ; She disappears below counter...
    MOVE.B  #2,TIMERAFTERE(A4)  ; Set up timer for her reappearance
80$
    POPSP
90$
    RET

    XREF    BakerEject,SP.FeedBaker2,GetAllIn

SP.GiveBaker    ; trying to give/show charm4 to baker ?
    CMP.W   #NCHARM4,D0
    BNE.S   10$
    POPSP               ; don't return to verb rtn
    BRA BakerEject      ; prints 'Thief!' & chucks u out
10$
    PUSH_L  D1          ; give container wi bread in it to baker
    MOVE.B  10(A0),D1       ; get container vol
    AND.B   #7,D1
    PULL_L  D1
    BEQ.S   90$         ; EQ => not a container
    PUSH_L  D0/D2
    CALL_S  GiveContainerToBaker
    PULL_L  D0/D2
    BNE.S   90$
; fall through to...
80$
    POPSP
90$
    RET

    XREF    RELOBJ
 
; returns EQ if SP.FeedBaker2 was called
GiveContainerToBaker
    MOVE.W  #NPUDDING,D2        ; good bread in container ?
    CALL    RELOBJ
    BEQ.S   10$
    MOVE.W  #NPUDDING2,D2       ; bad bread in it ?
    CALL    RELOBJ
    BNE.S   90$
10$
    MOVE.W  D2,D0           ; check it is really local
    DO  VALID.NP
    BNE.S   90$         ; not valid => ignore & ret with NE
    EXG D0,D5           ; SP.FeedBaker2 expects D5=obj,D0=baker
    EXG A0,A5
    CALL    SP.FeedBaker2       ; which will POPSP,RET with EQ
90$
    RET

;******************************************
    XDEF    SP.GO.N

    XREF    ROOM,SEATED,TIMERAFTER9
    XREF    GO,SP.SitCloud
    XREF    LadderOrientation,LadderLeantAgainst,SEATED

SP.GO.N
    CMP.W   #NCARRIAGE_WINDOW2,D0
    BNE.S   02$
    CMP.W   #RNTRAIN_ROOF,ROOM(A4)  ;r u on train roof?
    BNE.S   02$         ;nope
    SCN 242         ;climb in
    MOVE.W  #RNON_TRAIN,D1      ;into the train
    DO  MovePlayer
    MOVE.B  #1,TIMERAFTER9(A4)  ;begin train guard
    BRA 80$         ;and exit
02$
    CMP.W   #NLOCOMOTIVE,D0
    BNE.S   01$
    CALL    Go.TrainRoof
    BNE 80$         ;quit mode
01$
    CMP.W   #NBEAM,D0       ; Trying to stand on the beam??
    BNE.S   10$
    CMP.W   #RNON_PLATFORM,ROOM(A4) ; check for right room
    BNE 99$
    TEST_W  SEATED(A4)      ; and not already seated
    BNE 99$
    MOVE.W  #NBEAM,SEATED(A4)   ; so sit him on the beam
    SCN 291
    BRA 80$

10$:
; Going onto the cloud?
    CALL    SP.SitCloud
20$

    CMP.W   #NYOUR_HOUSE,D0     ;go house
    BNE.S   30$
    CMP.B   #RNFRONTGARDEN1,ROOM+1(A4)
    BNE.S   21$
    MOVE.W  #DIR_NW,D5
    BRA GO
21$
    CMP.B   #RNBACKGARDEN1,ROOM+1(A4)
    BNE.S   22$
    MOVE.W  #DIR_S,D5
    BRA GO
22$
    SCN 119     ;you are already
    BRA 80$
30$
* Go hole?
    CMP.W   #NHOLE,D0
    BNE.S   50$
    SCN 107
* In case you hadn't noticed, there's this incredibly annoyed bull
* down there and I'm not going to extend the definition of adventurous
* to include foolhardy.

    BRA 80$

50$
* Go oven?

    CMP.W   #NOVEN,D0
    BNE.S   60$
    TST_OPEN                ;open?
    BNE.S   52$
    SCN 65
* But the oven is closed.
    BRA 80$
52$
    ESCAPE  OVEN
    BRA 80$         ;fall through

60$
    CMP.W   #NCMAKERS_WINDOW,D0
    BNE.S   70$
    DO  P.TN            ;The window
    SCN 793
* is too small for you to climb through.

    BRA 80$

70$
    CMP.W   #NLADDER,D0     ;Go,climb etc ladder?
    BNE.S   99$
    MOVE.W  LadderLeantAgainst(A4),D1   ;Ladder position
    BNE.S   72$
    SCN 808
* It would help if the ladder was leaning against something!
    BRA 80$
72$
    TEST_B  LadderOrientation(A4)   ;Which way up is it?
    BEQ.S   74$         ;=>right way
    SCN 809
* You scramble up the ladder. Nearing the top, you come across a minor
* problem: there are no rungs to allow further climbing.
    SCN 810
* There is no alternative; you climb back down.

    BRA.S   80$
74$
    MOVE.W  SEATED(A4),D2
    BEQ.S   78$
    CMP.W   #NSTOOL,D2          ;Standing on stool?
    BNE.S   78$
    SCN 811
* From your vantage point on the stool, you easily manage to reach the
* rungs. A short climb later, you find yourself at the top of the ladder.

    CMP.W   #NGIRDER,D1         ;Ladder leant against girder?
    BNE.S   76$
    SCN 812
* As you step from the ladder to the girder, your foot slips and the
* ladder falls to the floor with a crash.

    CLR.B   LadderOrientation(A4)       ;Fod relationships
    CLR.W   LadderLeantAgainst(A4)
    MOVE.W  #RNGIRDER_ROOM,D1
*   BSET    #7,NRT_GIRDERPUT(A4)        ;it's local these days
                        ; but they stil cant HEHE
    DO  MovePlayer          ;Put player on girder
    SCORE   ONGIRDER            ; get some points
    BRA.S   80$

76$
    SCN 813
* Unfortunately, there's nowhere to go.
    SCN 810
* There is no alternative; you climb back down.

    BRA.S   80$

78$
    SCN 814
* The rungs at the bottom of the ladder are missing and you're not
* high enough to reach them.

80$:
    POPSP
99$:
    RET

;******************************************

    XDEF    SP.GET
    XREF    SP.PICKUP,CONJ

SP.GET

    XREF    Do_Dirt

    CALL    Do_Dirt         ;Fiddling with dirt?
    BEQ.S   80$         ;=> coped

10$
    CALL    SP.PICKUP
    BEQ.S   90$         ; nothing happened

80$
    POPSP
90$
    RET


*------------------------


*
*
* GETWOSSNAME   -   Locates guardian in current room
*           and sets him up for conversation.
*
*   Returns     EQ  => Same room as last call
*           NE  => Room changed
*

        XDEF    GETWOSSNAME

        XREF    OLDROOM,NPCINRM,Underwater_x,Underwater_y
        XREF    OldUnderwater_x,OldUnderwater_y

GETWOSSNAME

    PUSH_L  D0-D3           ;Scratch
    CLR.L   D1
    MOVE.W  ROOM(A4),D1     ;Get current room and...
    MOVE.W  Underwater_x(A4),D2 ;...underwater pos's.
    MOVE.W  Underwater_y(A4),D3
    MOVE.W  #NGUARDIAN,D0       ;Get guardian into current room
    GETINFO
    MOVE.L  D1,6(A0)        ;Regardless of whether already here
    MOVE.W  D0,NPCINRM(A4)      ;So you can talk to him
    CMP.W   OLDROOM(A4),D1      ;Same Room as before?
    BNE.S   10$         ;=> nope, update and exit NE
    CMP.W   OldUnderwater_x(A4),D2  ;Changed?
    BNE.S   10$         ;=> yep, update and exit NE
    CMP.W   OldUnderwater_y(A4),D3  ;How about this one?
    BEQ.S   90$         ;=> not moved at all
10$
    MOVE.W  D1,OLDROOM(A4)      ;Update all our 'lastroom' stuff
    MOVE.W  D2,OldUnderwater_x(A4)
    MOVE.W  D3,OldUnderwater_y(A4)
80$
    SETNE               ;Flag to caller - room change
90$
* NB: is EQ if we branch here

    PULL_L  D0-D3           ;Preserves flags
    RET

*-----------------------------


*
*
* SP.GET.BurnFingers    -   Stops player picking up objects which
*               are on fire.
*
*   On Entry:   A0 -> Noun data of object in question
*           D0 =  Noun # of object
*   On Exit:    A1.L , D1.W corrupted
*

*   XREF    STOPRON
*
*SP.GET.BurnFingers
*
*
*   LEA ExceptionTable(PC),A1
*10$
*   MOVE.W  (A1)+,D1
*   BEQ.S   20$             ;=> Hit end of table
*   CMP.W   D0,D1               ;Is it an exception?
*   BNE.S   10$             ;=> Nope, check next
*15$
*   RET
*20$
*   BTST    #7,(A0)             ;Is it lit?
*   BEQ.S   15$             ;=> Nope, exit EQ
*   MOVE.W  D0,STOPRON(A4)
* Realising that you are not wearing a flame-proof suit, you think better
* of handling the burning xyzzy .^
*   SCN 278
*   SETNE                   ;We handled it
*   RET


*
* This is a list of objects which the player is allowed to get even
* if they are lit.
* 

*ExceptionTable
*
*   DC.W    NMATCH
*   DC.W    NCANDLE
*   DC.W    0               ; Word terminator

*------------------------------------------

    XREF    ISIN

Go.TrainRoof

    CMP.W   #RNPLATFORM_BRIDGE,ROOM(A4)
    BNE.S   70$
    MOVE.W  #NLOCOMOTIVE,D0
    DO  ISIN
    BNE.S   31$         ;no, oh dear!
    SCN 239         ;jump onto roof
    MOVE.W  #RNTRAIN_ROOF,D1    ;u r there
    DO  MovePlayer
    TEST_B  REMASTER(A4)
    BEQ.S   80$                 ; remaster makes train leave as soon as roof
    TEST_B  TIMERAFTER8(A4)     ;train left yet?
    BEQ.S   80$
    MOVE.B  #1,TIMERAFTER8(A4)  ;make it leave soon
    BRA.S   80$         ;and exit
31$
    ESCAPE  FALLBRIDGE
80$
    SETNE               ;quit
90$
    RET
70$
    SETEQ_RET           ;and exit

*--------------------------------------
    END
