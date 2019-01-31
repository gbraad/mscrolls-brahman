*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
* specials s
*
*--------------------------------


    include "equates.i"
    include "verbequ.i"
    include "nounequ1.i"
*   include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"
    include "scenequ.i"


    XDEF    SP.FreezeBottle
    XREF    VALID.NP,PUTD0IND5,P.TNSTOP
    XREF    REMASTER

    XDEF    SP.SPEECH

SP.SPEECH
    XREF    ROOM,WakeUpGaoler.DO

    CMP.W   #RNGAOL,ROOM(A4)        ;wakes up gaoler
    BNE.S   90$
    SCN 539
    SCN 598             ; you get told
    PEA 10$(PC)             ;next routine thinks its 
    CALL    WakeUpGaoler.DO         ;a specials & popsp's
10$
    SETEQ
90$
    RET

    XDEF    SP.SCORE.NOUN

SP.SCORE.NOUN

    RET


*******************************************************
*
* SP.SWIM - in lake?
*
    XDEF    SP.SWIM

SP.SWIM
    RET

*******************************************************
*
* SP.SLEEP
*
    XDEF    SP.SLEEP

SP.SLEEP

    RET

*******************************************************
*
* SP.SET
*
    XDEF    SP.SET
    XREF    SETOUT,V.PUT,COBJ,VERB,FlagTrapSet

SP.SET
    CMP.W   #NMOUSE_TRAP,D0
    BNE 90$
    POPSP               ;you'r mine now, no getting away!
    TEST_B  FlagTrapSet(A4)     ; already done?
    BEQ.S   5$
    SCN 231
    RET
5$
    CMP.W   #NCHEESE,D5     ;set trap wi cheese
    BEQ.S   10$
    TEST_W  D5          ;did we have an indirect object?
    BLS.S   20$
* fake a put x on trap
    MOVE.W  #ON,CONJ(A4)
    MOVE.W  #VNPUT,VERB(A4)
    EXG D0,D5
    EXG A0,A5
    BRA V.PUT
10$
    MOVE.W  D5,D0
    DO  SETOUT
    MOVE.L  #REL.MASK+NMOUSE_TRAP,6(A5) ;put cheese there
20$
    MOVE.W  #NCHEESE,D0
    GETINFO
    CMP.L   #REL.MASK+NMOUSE_TRAP,6(A0)
    BEQ.S   30$             ;ok - can set it
    SCN 231         ;fail to set mouse trap
    RET
30$
    ST  FlagTrapSet(A4)
    SCN 230         ;u succeedin setting the mouse trap
90$
    RET

    XDEF    SP.SHAKE
    XREF    FlagBull,TIMER1,SETOUT,TIMER9,CARRIED,ODT_LIBRARY2
    XREF    WakeXam,SP.GivePostMistress,SP.WAKE
 

SP.SHAKE

; Shaking tablecloth?
    CMP.W   #RNFIELD1,ROOM(A4)  ;Right room for bull
    BNE.S   10$
    CALL    Do_Bull         ;Check bull, won't ret if happens
10$
    CMP.W   #NDEAD_MOUSE,D0     ; waving the dead mouse?
    BNE.S   20$
    CMP.W   #RNPOSTOFFICE,ROOM(A4)  ; are we in the post office
    BNE.S   20$
    PUSH_L  D0/A0/D5
    MOVE.W  #NPOSTMISTRESS,D5
    EXG D0,D5
    GETINFO
    TST_OUT             ; out?
    BNE.S   50$
    EXG D0,D5           ; set up right
    ADD.W   #12,A7          ; to compensate for push
    CALL    SP.GivePostMistress ; WILL NOT RETURN
    BRA.S   80$         ; but just in case it does...
50$
    PULL_L  D0/A0/D5
    RET
20$
    BRA SP.WAKE         ; for wake xam and gaoler
80$:
    POPSP
90$
    RET

*-----------------------

*
* Do_Bull   -   Copes with taunting the bull
*           Called from SP stack level.
*           only returns if it did nothing
*           else POPS back past verb.

    XDEF    Do_Bull
    XREF    TIMER1,ROOM

Do_Bull
    CMP.W   #NTABLECLOTH,D0     ; Tablecloth?
    BNE.S   90$
    TEST_B  FlagBull(A4)        ; Bull is here and ok?
    BNE.S   90$
    TEST_B  TIMER1(A4)      ; timer not yet running?
    BNE.S   90$
    SCN 20          ; 'The bull charges at you'
    MOVE.B  #2,TIMER1(A4)       ; Should expire at end of next move!    
80$
    ADDQ.L  #8,A7           ;Pop back past verb

90$
    RET
*----------------------

    XDEF    SP.SitCloud
    XREF    StartCloud,GetRidOfObjects,P.LIST3,DNOUNBUF,COUNTLIST
    XREF    GoneCloud
    
SP.SitCloud
    
*** copes with sitting on the cloud...
    CMP.W   #NNICECLOUD,D0      ; sitting on the cloud
    BNE.S   99$         ; hmmmm... no bloody way
    PUSH_L  D0-D7/A0-A6
    CMP.W   #RNEND_OF_BEAM,ROOM(A4)
* if we are on the beam we are then above the fairground.
    BNE.S   80$         ; for now...???
    SCN 739         ; the cloud moves off...
    MOVEQ   #RNFAIRGROUND,D1
    MOVE.W  D1,ROOM(A4)     ; above the fairground...
    MOVE.W  #1,SCRPNT(A4)       ; refresh the screen...
    CALL    StartCloud      ; make sure cloud is around
    SCORE   GOCLOUD
80$
**** We must make sure that all the objects are there.
* check that you are taking nothing bigger than size 4 onto the cloud.
    ST  D7      ; flag for size
    MOVE.W  ROOM(A4),D6
    EXT.L   D6      ; for putting the data into ...
    CALL    GetRidOfObjects ; remove objects greater than size 4.
    LEA DNOUNBUF(A4),A1
    MOVE.L  A1,A0
    DO  COUNTLIST
    BEQ.S   90$
    SCN 752     ; whatho says the cloud...
*   LEA DNOUNBUF(A4),A0
    DO  P.LIST3     ; prints the list...
    DO  BSFULLSTOP
90$
    BSET    #1,GoneCloud(A4)
    BNE.S   95$
    SCN 383         ; nice to see you old boy
95$
    PULL_L  D0-D7/A0-A6
    MSGGO   LINE
99$
    RET

    XDEF    SP.SIT
    XREF    RIDING,RIDFLG,CanoeLaunched
    XREF    BRKQ,DESITTABLE,SEATED,SEATED_RELATOR

SP.SIT
    CALL_S  SP.SitCloud
; Cope with 'sit on canoe'
    CMP.W   #NCANOE,D0
    BNE.S   20$
    TEST_B  CanoeLaunched(A4)   ; Is canoe already launched?
    BNE.S   10$
    SCN 62          ; 'You must launch canoe first.'
    BRA 80$

10$:
    MOVE.W  #1,RIDING(A4)       ; now riding the canoe!
    BSET    #0,RIDFLG(A4)
    MOVE.W  #IN,D1          ;change to in
    BRA.S   90$
20$
    CMP.W   #NWICKERTABLE,D0    ;sit on flimsy table?
    BNE.S   30$         ;no
    DO  DESITTABLE      ;all items fall off
    CALL    BRKQ            ;break the table
    SCN 319         ;you fall off
    BRA.S   80$

30$
    CMP.W   #NSEAT,D0       ;Trying to sit down on the bus?
    BNE.S   40$
    SCN 339
* But they're all occupied.^
    BRA.S   80$
40$
    CMP.W   #NSCALES,D0
    BNE.S   50$
    SCN 507         ; 'You'd break them!'
    BRA.S   80$

* cope with 'sit on mousetrap'
50$
    CMP.W   #NMOUSE_TRAP,D0
    BNE.S   60$
    SCN 583
* That's an incredibly stupid idea.

    BRA.S   80$

60$
    CMP.W   #NSTOOL,D0
    BEQ.S   SitStool
    BRA.S   90$
80$

    POPSP
90$
    RET
SitStool
    TEST_W  SEATED(A4)          ;Already seated?
    BEQ.S   10$             ;=> Nope
    SCN 816
* But you are already!

    BRA.S   30$
10$
    MOVE.B  3(A0),D1            ;Get texture etc
    AND.B   #$F,D1              ;Lose etc
    CMP.B   #TEXTURE.HOT,D1         ;Is it hot?
    BNE.S   20$
    SCN 818
* It's too hot!

    BRA.S   30$
20$ 
    SCN 815             ;Poxy verb FORCES VNSIT yuk!
* You clamber onto the stool.

    MOVE.W  #ON,SEATED_RELATOR(A4)      ;Force to ON
    MOVE.W  D0,SEATED(A4)           ;u are now on the stool
30$
    POPSP_RET

*----------------------------------------






    XDEF    SP.SHINE
    XDEF    SP.SPLIT

SP.SHINE
SP.SPLIT
    RET


    XDEF    SP.SLIDE
    XREF    OPEN,CLOSE
*   XREF    SP.MEDDLE

SP.SLIDE

; Sliding the sliding door?
    CMP.W   #NSLIDINGDOOR,D0
    BNE.S   10$
    POPSP
    TST_OPEN            ;OPEN?
    BNE     CLOSE               
    BRA     OPEN 
10$
    CMP.W   #NBLOCK,D0      ;slide a block
    BEQ DoSlidingBlocks
    TEST_W  D0          ;any such?
    BPL.S   20$         ;yes
    MOVE.W  #NBLOCK,D0
    DO  VALID.NP        ;around?
    BEQ DoSlidingBlocks     ;yes, then assume a block move
    MOVEQ   #-1,D0          ;set back
20$
90$: 
    RET


*--------------------------------
*
* SP.SCORE - ADD TO SCORE  IF
*               WE GET TO A SECIAL
*            PLACE  - IE PAST ROCKS
*
*--------------------------------

***********************************************************************
* !!!!!!!!!!    Gonna try and do this game without this     !!!!!!!
* !!!!!!!!!!    being called every move coz of overhead - Paul  !!!!!!!
***********************************************************************

*   XDEF    SP.SCORE
*   XREF    ROOM,DOSCORE2
* 
* 
*SP.SCORE
*   
*   LEA     ScoreList_ROOMS(PC),A1
*   MOVE.W  ROOM(A4),D1
* 
*10$
*   MOVE.L  (A1)+,D0
*   BEQ.S   20$
*   CMP.W   D1,D0           ;LOW WORD IS ROOM
*   BNE.S   10$
*   SWAP    D0              ;HI WORD IS SCORE  ADDR
*   LEA -4(A1,D0.W),A2
*   CALL    DOSCORE2       ;OK IF IN ONE ROOM CAN'T BE ANYWHERE ELSE
* 
*20$
*   LEA     ScoreList_NOUNS(PC),A1
*30$
*   MOVE.L  (A1)+,D0
*   BEQ.S   40$
*   DO      CARRIED
*   BNE.S   30$              ;NOPE
*   SWAP    D0
*   LEA -4(A1,D0.W),A2
*   CALL    DOSCORE2
*   BRA.S   30$              ;NEXT
*40$
*   RET
* 
* list of nouns you get points for - DC.W SC.fred,Nfred
 
*ScoreList_NOUNS
*
*SL_NOUN    MACRO   * noun
*   DC.W    SC.\1,N\1
*   ENDM
*
*   DC.L    0
*   
** list of rooms you get points for
* 
*ScoreList_ROOMS
*
*SL_ROOM    MACRO   * room
*   DC.W    SC.\1,RN\1
*   ENDM
*
*   SL_ROOM hmmmm
*   
*   DC.L    0
* 
*
*---------------------

    XDEF    SP.SMELL

*
* This looks like a real NASTY frig - all it does - it seems - is ensure
* that there's a D0 set up for the return to V.SMELL.
* If wantSP.SMELL is undefined then this obviously doesn't happen
* and V.SMELL ends up in EDTS.OK with no -1 in D0, if the command line
* was a simple 'smell'. It doesn't bother to check and happily tests
* the data via (A0)! For all I know (A0) could point somewhere that
* copes with this instance in which case it isn't so bad.
* The main point is that I don't see why the special does what it does
* rather than the verb routine???????
* It's not too nice like this anyway since a 'smell' ends up doing
* a DBS if there's no special code to cope  - which isn't really nice.
*
* Paul
 
; What it is meant to do is convert a simple 'smell' to 'smell current room',
; so that you can have a smell in a room. I've inserted the GETINFO, which
; fixes up the A0 bug. It's not in the verb 'cos I couldn't modify the verb,
; and I 'spose it is scenario dependent. 
;
; The return to the verb is so that the TBL.SMELL stuff will then work.
;
; I reckon that the DBS is V.SMELLs problem. It should reply with something
; sensible like 'the xyzzy smells like an xyzzy' or whatever.
;
; Roddy
;
; N.B SP.LISTEN probably does just the same!
;
SP.SMELL

    TEST_W  D0
    BPL.S   05$
    DO  R.NOUN
    GETINFO
05$:

    RET
 
*---------------------

 
    XDEF    SP.SMOKE

SP.SMOKE

    RET
 

********************************************************************************
    XDEF    SP.RainManOut,SP.SunManOut
    XREF    FGT_Platform,FGT_Beam
    XREF    SCRPNT,SEATED,STOPRON,StartClock


; Move the rain weatherman to the OUTSIDE

SP.RainManOut:
; Set up first glances.

    MOVE.W  #HI+281,FGT_Platform(A4)
    MOVE.W  #HI+284,FGT_Beam(A4)

; Now move 'real' weathermen around
    MOVE.W  #NSUN_WEATHERMAN,D0
    GETINFO
    MOVE.W  #NON_PLATFORM,8(A0)

    MOVE.W  #NRAIN_WEATHERMAN,D0
    GETINFO
    MOVE.W  #NEND_OF_BEAM,8(A0)

; ... and swap back their 'distant' counterparts.
    MOVE.W  #NDISTANT_SUNMAN,D0
    GETINFO
    MOVE.W  #NEND_OF_BEAM,8(A0)
    MOVE.W  #NDISTANT_RAINMAN,D0
    GETINFO
    MOVE.W  #NON_PLATFORM,8(A0)

    MOVE.W  #NRAIN_WEATHERMAN,STOPRON(A4)
; This prints out messages if we see beam moving
    MOVE.W  #NSUN_WEATHERMAN,D0
MoveBeam:
    MOVE.W  ROOM(A4),D1
    CMP.W   #RNFAIRGROUND,D1
    BEQ.S   10$
    CMP.W   #RNCAROUSEL,D1
    BNE.S   20$
10$:
; From fairground or carousel...
    SCN 286         ; 'You see movement from the clock'
    RET
20$
    CMP.W   #RNEND_OF_BEAM,D1   ; On far end of beam??
    BEQ.S   50$
    CMP.W   #RNON_PLATFORM,D1   ; Or on platform?
    BNE.S   99$

; He's on the platform, so...
    SCN 287         ; 'Beam swings...'
    CMP.W   #NBEAM,SEATED(A4)   ; is he riding beam?
    BNE.S   40$
    SCN 290         ; ...',taking you '
    CLR.W   SEATED(A4)      ; Put player at end of beam.


    MOVEQ   #RNEND_OF_BEAM,D1
    CALL    StartClock      ;Coz we don't go through SETMODE...
                    ;yet we need to spot this room change
    DO  MovePlayer

    CALL_S  SP.CloudReach       ; makes cloud reachable
40$
    SCN 289         ;...' outside.'
    RET

; Messages from far end of beam
50$:
    MOVE.W  D0,STOPRON(A4)
    MOVE.W  #RNON_PLATFORM,ROOM(A4) ; Change room.
    MOVE.W  #NBEAM,SEATED(A4)   ; ... and sitting on beam
    SCN 287         ; Beam pivots, taking man
    SCN 290         ; and yourself
    SCN 288         ; inside
    CALL_S  SP.CloudUnreach
    MOVE.W  #1,SCRPNT(A4)       ; Force new desc.   
99$:
    RET
    XREF    NRT_NICECLOUDLOOK
    XDEF    SP.CloudReach,SP.CloudUnreach
    
SP.CloudReach
*** Makes the cloud reachable
    BSET    #7,NRT_NICECLOUDLOOK(A4)    ; look
    RET
    
SP.CloudUnreach
*** Makes the cloud not reachable
    TEST_B  FrozenCloud(A4)             ; if cloud is
    BNE.S   99$
    BCLR    #7,NRT_NICECLOUDLOOK(A4)    ; look
99$
    RET
    
    
; Just like SP.RainManOut, but for the sun man.
SP.SunManOut
; Set up first glances.

    MOVE.W  #HI+282,FGT_Platform(A4)
    MOVE.W  #HI+283,FGT_Beam(A4)

; move real weathermen
    MOVE.W  #NRAIN_WEATHERMAN,D0
    GETINFO
    MOVE.W  #NON_PLATFORM,8(A0)

    MOVE.W  #NSUN_WEATHERMAN,D0
    GETINFO
    MOVE.W  #NEND_OF_BEAM,8(A0)

; Move distant ones.
    MOVE.W  #NDISTANT_RAINMAN,D0
    GETINFO
    MOVE.W  #NEND_OF_BEAM,8(A0)
    MOVE.W  #NDISTANT_SUNMAN,D0
    GETINFO
    MOVE.W  #NON_PLATFORM,8(A0)

; Set up for MoveBeam
    MOVE.W  #NSUN_WEATHERMAN,STOPRON(A4)
    MOVE.W  #NRAIN_WEATHERMAN,D0
    BRA MoveBeam

********************************************************************************
********************************************************************************
*
*
* Magic Square puzzle - useful stuff - NOT YET BOUND TO OBJECTS.
*
* Generally, everything is data driven from an ASCII chunk of data.
* GapPos is index byte of the current gap.
*
********************************************************************************
********************************************************************************
 
    XDEF    SlideBlock
    XDEF    PrintSquareEntry

    XREF    SquareData,RowData,RowSize,GapPos,SquareDataEnd
    XREF    CHAROUT,GETCHAR2

GapChar     EQU '_'
MagicNumber EQU '4'+'5'+'6'

; Slideblock is called with D0 = ascii char of block to be slid.
SlideBlock
************************
* Temporary fetching of parameters from keyboard. When bound to objects,this
* can go.
*   PRINT   <'Which number do you want to slide? '>
*   JSR GETCHAR2
*   MOVE.B  D1,D0
************************

    LEA SquareData(A4),A0   ; Address of square data representation
    MOVE.B  GapPos(A4),D1       ; point at 'gap'
    EXT.W   D1

    LEA -2(A0,D1.W),A1      ; block LEFT of gap..
    CMP.B   (A1),D0         ; Check if match found
    BEQ.S   SlideFound

    LEA 2(A0,D1.W),A1       ; block RIGHT of gap
    CMP.B   (A1),D0         ; Check match
    BEQ.S   SlideFound

    LEA 0(A0,D1.W),A1       ; block UP from gap
    SUB.W   #RowSize,A1     ; Frig, as asm. can't cope.
    LEA SquareData(A4),A2   ; check not out of square
    CMP.L   A1,A2
    BHI.S   1$
    CMP.B   (A1),D0         ; check for match
    BEQ.S   SlideFound
1$
    LEA 0(A0,D1.W),A1
    ADD.W   #RowSize,A1
    LEA SquareDataEnd(A4),A2    ; outside puzzle?
    CMP.L   A2,A1
    BHI.S   NoSlide
    CMP.B   (A1),D0         ; check match
    BNE.S   NoSlide         ; If it won't slide, print a message

; Come here when block can be slid into gap. A1 points to block pos
SlideFound:
    MOVE.B  #GapChar,(A1)       ; Make block into gap
    MOVE.B  D0,0(A0,D1.W)       ; Make gap into block!
    SUBA.L  A0,A1           ; Convert to index...
    MOVE.W  A1,D1           
    MOVE.B  D1,GapPos(A4)       ; Save new Gap position.

; Now handle how many lights are lit, etc.
CheckLights:
    LEA RowData(A4),A1      ; Rowdata says which squares make rows
    LEA SquareData(A4),A0
    CLR.W   D0
    CLR.W   D2
10$:
    MOVE.B  (A1)+,D0        ; End of row list?
    BMI.S   PrintSquare
    LEA 0(A0,D0.W),A2       ; Build address of a light
    MOVE.B  #'+',(A2)       ; '+' is an OFF light

    MOVE.B  (A1)+,D0        ; Get first pos of row
    MOVE.B  0(A0,D0.W),D1       ; Get value
    MOVE.B  (A1)+,D0        ; Get second,
    ADD.B   0(A0,D0.W),D1       ; ... add it to value
    MOVE.B  (A1)+,D0
    ADD.B   0(A0,D0.W),D1       ; ... ditto with third
    CMP.B   #MagicNumber,D1     ; does value == the MAGIC NUMBER?
    BNE.S   10$
    ADDQ.W  #1,D2           ; Increment number of lights now on.
    MOVE.B  #'*',(A2)       ; Turn the lamp ON
    BRA.S   10$

PrintSquare:
PrintSquareEntry
    SCN 295     ; 'The square looks like this...' 
    LEA SquareData(A4),A0   ; Print the square
10$:
    MOVE.B  (A0)+,D1    ; One byte at a time!
    BEQ.S   20$     ; Zero terminated message
    IFNE    SPECTRUM    ; speccy wants a weird char instead of '_'
    CMP.B   #'_',D1
    BNE.S   15$
    MOVE.B  #'`',D1
15$
    CMP.B   #':',D1
    BNE.S   16$
    MOVE.B  #'|',D1
16$
    ENDC    ;SPECTRUM
    DO  CHAROUT     ; IS THERE A CALL TO DO THIS??
    BRA.S   10$
20$
    CMP.W   #8,D2       ; Are all lights lit?
    BNE.S   90$
    SCN 296     ; 'All lights flash'
    BRA DomeShatters    ;and return
90$:
    RET


NoSlide:
    SCN 294     ; 'That block won't budge'
    RET 

*---------------------------

    XDEF    SP.THINGY

SP.THINGY

    CALL    Do_Sol              ;Coz CAST needs to call it too
    BEQ.S   90$
    POPSP
90$ 
    RET

*-------------------------

    XDEF    Do_Sol
    XREF    TypeOfWeather,WT_WRAIN,EndRain

* This has been factored from SP.SOL coz of CAST hassles

Do_Sol

    MOVE.W  #NCHARM1,D0
    CLR.W   D5              ;Hmm, this is a SINGV
    CALL    SpellWorks          ;Got it?
    BNE.S   80$             ;No chance matey!
    BCLR    #WT_WRAIN,TypeOfWeather(A4) ;Stop the rain
    BEQ.S   90$             ;=> already stopped
    MOVE.W  #NWRAIN,D0
    GETINFO                 ;EndRain expects this
    CALL    EndRain
    TEST_W  D6              ;Did we see the effect?
    BNE.S   80$             ;=> yep
    SCN 373
* Nothing obvious happens.

80$
    SETNE_RET
90$
*   SETEQ                   ;EQ if we get here anyhow
    RET

*-------------------------
    IfVerb  SUCK
    XDEF    SP.SUCK
SP.SUCK
    RET
    ENDC
*--------------------------

**************************************************************************
*
* This section takes care of all spells in the game.
*
* There are two basic routines and a set of macros.
* The routines are:
*
*   SpellWorks  - This says wether the spell will work
*
*   SpellControl    - This handles all special code when casting
*             a spell on a specific object.
*
*   The macros simply allow the definition of tables which SpellControl
*   references to suss what it's gonna do.
*
**************************************************************************



*
*
* SpellWorks    -   Checks if relevant charm is carried and
*           ensures a valid useage
*
*   Takes:      D0 as noun # of charm to suss.
*
*   Returns:    NE=>    Summit wrong, and I handled it
*           EQ=>    All okay, do it

    XDEF    SpellWorks
    XREF    CONJ,P.MHAVSTOP,ROOMTODANCE

SpellWorks
*   CMP.W   #NCHARM5,D0     ;Sol and Aqua allowed even if no....
*   BEQ.S   5$          ;'roomtodance'
*   CMP.W   #NCHARM1,D0
*   BEQ.S   5$
*   ARGW    #0
*   CALLS   2,ROOMTODANCE       ;Else, is there room
*   BEQ.S   5$          ;=> Yep
*   SCN 467
* Not enough room here!
*   BRA.S   80$
*5$
    DO  CARRIED
    BNE.S   20$         ;Must have it.

    TEST_W  D5          ;Indirect object?
    BEQ.S   90$         ;=> nope, exit ok ( => SINGV )
    CMP.W   #WITH,CONJ(A4)      ;With summit?
    BEQ.S   10$         ;Yep but with what?
    MSG IDFY            ;'I don't follow you.'
    BRA.S   80$

10$
    CMP.W   D0,D5           ;Right charm?
    BEQ.S   90$         ;=> Yep, all okay
    MSG NHAP            ;'Nothing happens.'
    BRA.S   80$

20$
* !!!!!!!! Force adj on charm gives the game away !!!!!!!!!!
    DO  P.MHAVSTOP      ;'You must have the <charm>.'

80$
    SETNE_RET
90$
*   SETEQ               EQ if we get here anyhow
    RET

*------------------

*
* Flyzzable macro allows the definition of nouns that are special
* cases and some appropriate code to cope with it.
*

Flyzzable   MACRO   *SP.xyzzy,NOUN

    DC.W    \1-*
    DC.W    N\2

    ENDM

*
* FlyzzSCN macro allows the definition of nouns that are special
* cases and a scenario message # to print.
*

FlyzzSCN    MACRO   *SCN#,NOUN

    DC.W    \1+HI
    DC.W    N\2

    ENDM


*
* Dracable macro allows the definition of nouns that are special
* cases and some appropriate code to cope with it.
*

Dracable    MACRO   *SP.xyzzy,NOUN

    DC.W    \1-*
    DC.W    N\2

    ENDM

*
* DracSCN macro allows the definition of nouns that are special
* cases and a scenario message # to print.
*

DracSCN     MACRO   *SCN#,NOUN

    DC.W    \1+HI
    DC.W    N\2

    ENDM

*
* Freezable macro allows the definition of nouns that are special
* cases and some appropriate code to cope with it.
*

Freezable   MACRO   *SP.xyzzy,NOUN

    DC.W    \1-*
    DC.W    N\2

    ENDM

*
* FreezeSCN macro allows the definition of nouns that are special
* cases and a scenario message # to print.
*

FreezeSCN   MACRO   *SCN#,NOUN

    DC.W    \1+HI
    DC.W    N\2

    ENDM



*
*
* SpellControl  -   Copes with all the 'special' happenings
*           associated with the spells.
*
*   TAKES:  A1      -> table for this spell
*       D0  Noun # to match
*
*   RETURNS:    NE => All taken care of
*           EQ => Didn't do anything
*

    XDEF    FlyzzControl,DracmazzaControl,FreezeControl
    XREF    P.SCN

FlyzzControl
    LEA FLYZZ.LST(PC),A1
    BRA.S   SpellControl

DracmazzaControl
    LEA DRACMAZZA.LST(PC),A1
    BRA.S   SpellControl

FreezeControl
    LEA FREEZE.LST(PC),A1
*   BRA.S   SpellControl            ;Fall through

SpellControl
10$
    MOVE.L  (A1)+,D1        ;Get address,noun/message
    BEQ.S   90$         ;=> End of table

    TEST_W  D1          ;Global match?
    BEQ.S   15$
    CMP.W   D1,D0           ;Specific match?
    BNE.S   10$         ;No, try next
15$
    SWAP    D1          ;Get addr/msg
    BCLR    #15,D1          ;Hi-bit?
    BEQ.S   20$         ;=> no, must be code
    DO  P.SCN           ;is a msg, do it
    BRA.S   80$
20$
    JSR -4(A1,D1.W)     ;Go there

80$
    SETNE_RET           ;=> We handled it
90$
*   SETEQ               ;EQ if we get here anyhow
    RET


FLYZZ.LST

    Flyzzable   SP.Unicorn,UNICORN
    Flyzzable   SP.Dragon,KERULGIAN_DRAGON
    Flyzzable   SP.Pelican,PELICAN
    Flyzzable   SP.tapdance_shoes,TAPDANCE_SHOES
    Flyzzable   SP.FireEngine,FIRE_ENGINE
    Flyzzable   SP.Carriage,STATE_CARRIAGE
    FlyzzSCN    610,MAILBOX
    FlyzzSCN    610,MAILBOX1
    Flyzzable   SP.MoveCloud,NICECLOUD
    Flyzzable   FlyzzBall,CRYSTALBALL
    Flyzzable   SP.Flies,FLIES

    DC.L    0           ;LONG word 0 terminator

DRACMAZZA.LST

    DracSCN     369,TWOFERG
    DracSCN     447,CHARM1
    DracSCN     447,CHARM2
    DracSCN     447,CHARM3
    DracSCN     447,CHARM4
    DracSCN     447,CHARM5
    DracSCN     447,SADDLE

* This will map to ANY noun. Should be safe since you need to be
* carrying the noun before we even get here.
    DC.W    SP.EverythingElse-*,0

    DC.L    0           ;LONG word 0 terminator

FREEZE.LST

    Freezable   SP.FreezeBottle,SOLUTION
    Freezable   SP.FreezeCloud,NICECLOUD
    Freezable   SP.FreezeStool,STOOL
    Freezable   SP.FreezeWaterBeer,WATER
    Freezable   SP.FreezeWaterBeer,BEER
    Freezable   SP.FreezeWaterBeer,LAKE1
    Freezable   SP.FreezeWaterBeer,ICEWATER ;hmm
    Freezable   SP.FreezePubHearth,PUBHEARTH
    Freezable   SP.FreezePubHearth,PUBFIRE
    Freezable   SP.FreezePubHearth,ASH
    Freezable   SP.FreezeFireInClockmakers,CMAKER_FIRE

    DC.L    0

*-----------------------

*
* Put all the special code to handle what happens when you cast a spell
* on an object here and define it in the appropriate spell table above.
*
* Note this is NOT standard SP.xyzzy stuff so NO stack popping!
*

* Flyzz code.....

    XREF    SETIN,OLDBUF,SETPRON,DROPHERE,TIMERAFTER10
    XREF    UnicornFlyzzed,SEATED,FodLongLook,UnicornFriendly
    XREF    SETPRON

SP.Unicorn

    TEST_B  UnicornFlyzzed(A4)      ;already alive before?
    BEQ.S   10$             ;no, then 1st time
    SCN 259             ;unicorn alive again
    CMP.W   #NUNICORN,SEATED(A4)        ;sitting on wood unicorn?
    BNE 11$             ;no, too bad
    DO  SETOUT              ;for last time
    MOVE.W  #NUNICORN2,D0           ;live one
    DO  SETPRON
    GETINFO
    MOVE.L  A0,A1               ;save
    BCLR    #7,6(A0)            ;set in, grazing
    CALL    FodLongLook         ;stops grazing
    ST  UnicornFriendly(A4)     ;lets you ride him now
    TEST_B  REMASTER(A4)
    BEQ.S   02$
    SCORE POSTSADDLE            ;if you didn't post the saddle here.
02$    
; The saddle now gets inseparably related to unicorn, so this code won't happen
;   MOVE.W  #NSADDLE,D0
;   GETINFO
;   CMP.L   #$0100<<16+NUNICORN,6(A0)   ;saddle on wooden?
;   BEQ.S   09$             ;yes, then u dont fall off
;   SCN 819             ;you fall off
;   CLR.W   SEATED(A4)          ;not sitting anymore
;   CLR.B   UnicornFriendly(A4)     ;not tame yet..
;   MOVE.W  #NUNICORN,D0            ;set back
;   GETINFO
;   BRA.S   11$             ;and unicorn flued away
;09$
    MOVE.W  #NUNICORN2,8(A0)        ;moves to real unicorn then
    SCN 820             ;across ravine
    MOVE.L  #RNCASTLE_PARAPETS,D1
    MOVE.W  D1,ROOM(A4)         ;u r there now (on unicorn)
    MOVE.L  D1,6(A1)            ;so is the unicorn
    SCORE   ARRIVECASTLE            ; yip yip yip
    POPSP_RET               ;fine
10$
    MOVE.W  #NUNICORN2,STOPRON(A4)
    SCN 252             ;uncorn springs to life
    PUSH_L  D0/A0
    MOVE.W  #NSADDLE,D0
    CALL    DROPHERE            ;in room
    PULL_L  D0/A0
    CALL    SP.TurfOff
11$
    SCN 372             ;flies into sky
    DO  DESITTABLE          ;desit d0
    BSET    #3,5(A0)            ;sittable again
    DO  SETOUT
    MOVE.W  #NUNICORN2,D0           ; set other unicorn IN
    CALL    FodLongLook         ;remove grazing (if exists)
    MOVE.W  D0,OLDBUF(A4)           ; Frig to lose the 'it'
    GETINFO
    MOVE.L  #$1100<<16+NOUTSIDE,6(A0)   ;insep rel to outside.
    ST  UnicornFlyzzed(A4)
    MOVE.B  #20,TIMERAFTER10(A4)        ;20 moves until unicorn on mountain
    BRA SETPRON             ; make it 'him?'

SP.Dragon

    SCN 579
* Dragon springs to life and does whatever dragons do before sodding
* off into the distance.

*   MOVE.W  #NKERULGIAN_DRAGON,D0       ;D0 undisturbed by caller
    BRA.S   LoseD0

SP.FireEngine

    SCN 581
* Fire engine starts up and heads off into the distance leaving behind
* a broken ladder

    CALL    LoseD0              ;fix up SEATED if necessary
    MOVE.W  #NLADDER,D0
    GETINFO
    MOVE.L  #RNCAROUSEL,6(A0)       ;Drop ladder in room
    RET

SP.Carriage
    SCN 582
* Walrusses come to life and drag the carriage away.

*   MOVE.W  #NSTATE_CARRIAGE,D0     ;D0 undisturbed by caller
    BRA.S   LoseD0

SP.Pelican

    SCN 580
* Pelican springs to life and  does whatever pelicans do before soddin
* off into the distance.

*   MOVE.W  #NPELICAN,D0            ;D0 undisturbed by caller
    BRA.S   LoseD0

SP.Flies

    SCN 27
*   BRA.S   LoseD0              ;Fall through

LoseD0
    CMP.W   SEATED(A4),D0           ;Sitting on it?
    BNE.S   10$
    CLR.W   SEATED(A4)
    CLR.W   SEATED_RELATOR(A4)
10$
    DO  SETOUT              ;Fod it
    RET

*---------------------------------

SP.FreezeFireInClockmakers

    CMP.W   #RNCAROUSEL,ROOM(A4)        ;Outside?
    BNE.S   10$             ;else must be in shop
    SCN 631
* You can't reach the fire from here.

    POPSP_RET

10$
    DO  SETOUT              ;Lose fire
    SCN 630
* The fire sputters and dies.

    MOVE.W  #NSTOOL,D0
    GETINFO
    CMP.L   #REL.MASK+NCLOCKMAKERS,6(A0)    ;Stool still there?
    BNE.S   90$             ;Nope
    MOVE.B  3(A0),D1            ;Get texture etc
    AND.B   #$F,D1              ;Lose etc
    CMP.B   #TEXTURE.HOT,D1         ;Still hot?
    BEQ CmakerCommonBit         ;Same code as stool special
90$
    RET



*---------------------------------


SP.tapdance_shoes
    XREF    TapDancing,BeginBusking,TIMERAFTER3
    XREF    BuskOnTrain

    TEST_B  TapDancing(A4)  ;already
    BEQ.S   01$
    SCN 461     ;already
    RET         ;quit
01$
    SCN 458     ;shoes begin to move
    MOVE.W  #NTAPDANCE_SHOES,D0
    GETINFO
    TST_WORNBYPLAYER    ;worn?
    BNE.S   10$     ;yes
    SCN 459     ;they dance off
    DOGO    SETOUT
10$
    SCN 460     ;you dance
    ST  TapDancing(A4)  ;ok
    MOVE.B  #10,TIMERAFTER3(A4) ;tapdance duration
    BRA BuskOnTrain ;incase you are onboard


*--------------------------

* Dracmazza code....


*
* SP.EverythingElse -   This gets called whenever you dracmazza
*               an object you are carrying.
*               UNLESS it was an object which was higher
*               up the Dracmazza table.

    XREF    DracItem


SP.EverythingElse

    TST_LIGHTABLE           ;Lightable?
    BEQ.S   5$          ;=> nope, so ok
    SCN 447
* No way, Jose!
    BRA.S   90$

5$
    BSET    #7,(A0)         ;Make it glow
    DO  P.TN
    SCN 198
* shimmers slightly.

    MOVE.W  D0,-(A7)
    MOVE.W  DracItem(A4),D0     ;Get the old one...
    BEQ.S   10$         ;...if any
    GETINFO
    BCLR    #7,(A0)         ;!lit anymore
10$
    MOVE.W  (A7)+,DracItem(A4)  ;And... this is the new one
90$
    RET

*--------------------------------

* Freeze code..............

SP.FreezeStool

    MOVE.B  3(A0),D1        ;Get texture etc
    AND.B   #$F,D1          ;Lose etc
    CMP.B   #TEXTURE.COLD,D1    ;Already cold?
    BNE.S   10$
    MSGGO   NHAP            ;Nothing happens

10$
CmakerCommonBit

    MOVE.B  #TEXTURE.COLD!$10,3(A0) ;Make it cold (and moveable!)
    DO  P.TN            ;The stool
    MSG IS
    SCN 798
* covered by a frosty sheen. It looks much colder.

    RET

***************************************************************************
    
    XDEF    SP.STAND_Before
    XREF    DBS
SP.STAND_Before
* Stand on stool?
    CMP.W   #NSTOOL,D0
    BNE.S   90$
    DO  CARRIED     ; is it carried

    BNE SitStool
    POPSP
    BRA DBS
90$
    RET

    XDEF    SP.STAND
    XREF    LieDownOnTrain,SEATED,SEATED_RELATOR
    XREF    TrainOnRoute,ROOM,SEATED,Plummet

SP.STAND

    CMP.W   #RNLAKE1,ROOM(A4)
    BNE.S   01$
    CMP.W   #NCANOE,SEATED(A4)  ;in canoe?
    BNE.S   01$         ;nope
    SCN 161         ;trying to stand in canoe
    BRA.S   80$
01$
    TEST_B  LieDownOnTrain(A4)
    BEQ.S   30$
    MOVE.B  TrainOnRoute(A4),D1 ;get journey state
    SUBQ.B  #4,D1           ;4 & 5 are in tunnel.
    CMP.B   #1,D1
    BHI.S   10$         ;>1 unsigned to no
    SCN 182         ;stand up in tunnel
    ESCAPE  HITTUNNEL
    BRA.S   20$
10$
    SCN 521         ;you stand up daringly
20$
    CLR.W   SEATED(A4)
    CLR.W   SEATED_RELATOR(A4)
    CLR.B   LieDownOnTrain(A4)
    BRA.S   80$
30$
    CMP.W   #NNICECLOUD,SEATED(A4)  ; sitting on the cloud?
    BEQ Plummet         ; wheeeee.... (bang
    CMP.W   #RNTHEATRE,ROOM(A4) ;Trying to stand in op theatre
    BNE.S   90$
    SCN 448
* The straps restrain you.

80$
    POPSP
90$
    RET

*--------------------------------

    XREF    GetNumber
    XREF    PRTNUM,BlockToSlide

DoSlidingBlocks

    POPSP
    CALL    GetNumber   ;-> d2,d3
    MOVE.W  D2,D0
    TEST_W  D2      ;any number?
    BEQ.S   10$     ;no pick one.
    CMP.W   #9,D2       ;<=9?
    BHI.S   20$     ;too high
05$
    MOVE.B  D0,BlockToSlide(A4)
    ADD.W   #'0',D0     ;adjust to ascii
    BRA SlideBlock  ;slide d0
10$
    SCN 560     ;decide to slide block
    MOVE.B  BlockToSlide(A4),D6     ;test number
    EXT.W   D6
    MSG QUOTE
    CALL    PRTNUM
    MSG QUOTE
    DO  FULLSTOP
    MOVE.W  D6,D0
    BRA.S   05$
20$
    SCN 561
    MOVE.W  D0,D6
    MSG QUOTE
    CALL    PRTNUM
    MSG QUOTE
    SCN 562
    RET

*--------------------------------------

    XREF    EntryLongLook,DomeHasShattered,DO.SCORE,P.CONJ

DomeShatters

    ST  DomeHasShattered(A4)
    MOVE.W  #NGLASS_DOME,D0
    DO  SETOUT          ;gone.
    MOVE.W  #NBRACELET,D0
    GETINFO
    MOVE.L  #RNJEWEL_ROOM,6(A0) ;is in room now
    MOVE.W  #NGLASS_FRAGMENTS,D0
    DO  SETIN           ;set these in room
    SCORE   OPENDOME
    MOVE.W  #563,D1         ;message 'surrounded in glass'
    SWAP    D1
    MOVE.W  #NBRACELET,D1       ;set up longlook entry
    DOGO    EntryLongLook       ;into table

SP.FreezeBottle

* It will get here if you have either frozen the oil using a spell
* or have left the bottle in the fridge for a reasonably long time.

    PUSH_L  D0/A0
    MOVE.W  #NSOLUTION,D0       ; make sure it is still around
    GETINFO
    CMP.L   #CONT.MASK+NJAR,6(A0)   ; is it still in the jar?
    BNE 99$         ; ne => didn't work
*** NE => didn't work.
    BSET    #7,6(A0)    ; not calling setout because size of
                ; container doesn't change
    MOVE.W  #NICE,D0    ; what is comming to replace it.
    GETINFO
    BCLR    #7,6(A0)    ; and it is all okay now chaps. (ICE is in)
* so what is to happen to the oil....?
    MOVE.W  #NJAR,D0
    GETINFO
    DO  VALID.NP    ; things around?
    BNE.S   15$
    SCN 621     ; the water turns into ice and ...
15$
* now is it related to anything?
    BTST    #0,6(A0)    ; related....
    BEQ.S   30$     ; okay it isn't
    MOVE.L  6(A0),D3    ; where the oil will end up.
* is it a room ...
    EXG D0,D3
    GETINFO
    TST_ISAROOM     ; NE => it is a room
    BNE.S   40$     ; seeps away ....GONE
    EXG D0,D3
    MOVE.W  #NOIL,D0
    GETINFO
    MOVE.L  D3,6(A0)    ; the oil ends up there... (on it's own)
    MOVE.W  D3,D0
    MOVEQ   #ONTO,D1
20$
    SCORE   GETOIL
    TEST_B  D7
    BNE.S   90$
    DO  P.CONJ
    DO  P.TNSTOP    ; the x ...
    BRA.S   90$     ; pull/seteq and return
30$
* is it contained by anything...?
    TST_CONTAINED       ; contained?
    BEQ.S   40$     ; ohoh the oil will seep away
* is there enough room for the oil in the container where the plastic bottle
* was?
32$
    MOVE.W  8(A0),D0
    GETINFO
* no clothing containers...? (PLEASE)
    CMP.B   #1,10(A0)   ; EQ => full...
    BNE.S   35$     ; seeps away....
*** however is this container similarly contained?
    TST_CONTAINED       ; is it contained?
    BNE.S   32$     ; do the stuff big boy.
    TEST_B  D7      ; local?
    BNE.S   45$
    DO  P.TN        ; the x
    SCN 622     ; but because it is full it seeps away.
    BRA.S   45$
35$
    MOVE.W  #NOIL,D5
    EXG D0,D5
    GETINFO
    CLR.L   6(A0)       ; where're it is now...
    CALL    PUTD0IND5   ; and put it into it's place...
    EXG D5,D0       ; get them back
    MOVEQ   #INTO,D1
    BRA.S   20$     ; print the stuff
40$
    SCN 625     ; away
45$
    SETNE   
    BRA.S   99$
90$
    SETEQ
99$
    PULL_L  D0/A0
    RET

    XREF    DRAWPIC,GRAPHICS,SEATED,SEATED_RELATOR,WaterPicture
    XREF    CloudGone,FrozenCloud

SP.MoveCloud:
* Flyzzing the cloud when we are on it....
    CMP.W   #NNICECLOUD,SEATED(A4)      ; are we on it.?
    BEQ.S   10$
    TEST_B  FrozenCloud(A4)     ; frozen don't go nowhere.
    BEQ.S   05$
    SCN 751         ; no ta I'll wait for you
    RET
05$
    SCN 735         ; bye bye...
    BRA CloudGone
10$
    MOVE.W  #1,SCRPNT(A4)   ; update the screen.
    CMP.W   #RNFAIRGROUND,ROOM(A4)  ; are we in the fairground?
    BNE.S   20$         ; nope.... so.
    SCN 399         ; whatho says the cloud...
* Pausing briefly over a waterfall.
**********************************************************************
* Now factored so clockmaker's pic special can use it too
*       IFNE    Graphics
*   TEST_B  GRAPHICS(A4)    ; do they want graphics EQ => NO.
*   BEQ.S   15$
*   PUSH_L  D0-D7/A0-A6 ; just in case
*   MOVE.W  #WaterPicture,D0
*   CALL    DRAWPIC     ; go do drawpic.
*   PULL_L  D0-D7/A0-A6
*15$
*       ENDC
**********************************************************************

    ;; waterfall picture is now in a room
*    IFNE Graphics
*    IFEQ ST520!IBM!MAC512!PCW8256!ATARI
*
*    MOVE.W  D0,-(A7)        ;Stash
*    MOVE.W  #WaterPicture,D0
*    CALL    Do_PictureD0        ;Display Waterfall pic
*    MOVE.W  (A7)+,D0        ;recover
*
*    ENDC
*    ENDC             ; wanting graphics

    MOVE.W  #RNWATERFALL,D1
    BRA.S   16$

20$
**** we must be outside the station...
    SCN 193     ; comming back fast.
    MOVEQ   #RNFAIRGROUND,D1
16$    
    MOVE.W  D1,ROOM(A4)
    GETINFO
    MOVE.L  D1,6(A0)    ; the cloud is in the  room...
    RET 

    XREF    FrozenCloud

SP.FreezeCloud
** freezes the cloud...
    TEST_B  FrozenCloud(A4) ; is the cloud already frozen?
    BEQ.S   10$     ; nope
    MSG NHAP        ; nothing happens
    RET
10$
* are we seated on it.?
    ST  FrozenCloud(A4)     
    GETINFO
    MOVE.B  #$E,3(A0)       ; ice cold...
    CMP.W   SEATED(A4),D0   ; we are seated.
    BEQ.S   20$
* the only place we can reach it is when it is in the fairground....
* so we shall place this large frozen object in the fairground. and god
* help you...

    MOVE.W  #RNFAIRGROUND,D1
    EXT.L   D1
    MOVE.L  D1,6(A0)    ; put the cloud there
    SCN 736     ; plummets to the ground...
    RET
20$
* we are seated on it...
    SCN 737     ; i'll make the effort....
    RET

    XREF    TIMERAFTER18,FlyzzedFuture,CurrentFuture,TIMER17
FlyzzBall:
* simply triggers off the events..
* Have we managed to look into the future yet?
    TEST_W  CurrentFuture(A4)   ; got a future?
    BNE.S   10$
5$
    DO  P.TN            ; the ball.
    SCN 777         ; dunno chaps.
    RET
10$
* already flyzzed the future?
    BSET    #0,FlyzzedFuture(A4)
    BNE.S   5$          ; already done.
    MOVE.B  #1,TIMERAFTER18(A4) ; set's it going.
    SCN 778
    CLR.B   TIMER17(A4)     ; clear any next time appears
    RET

*------------------------------------------

    IFNE Graphics
    XDEF    Do_PictureD0
    XREF    PICTNUM

Do_PictureD0

    TEST_B  GRAPHICS(A4)    ; do they want graphics EQ => NO.
    BEQ.S   10$
    PUSH_L  D1-D7/A0-A6 ; just in case

    ;;  save the original pictnum
    MOVE.B  PICTNUM(A4),D1
    MOVE.W  D1,-(A7)
    
    MOVEQ   #0,D1       ; request original version
    CALL    DRAWPIC     ; go do drawpic.

    ;; restore original pictnum so room code does not replace picture
    MOVE.W  (A7)+,D1
    MOVE.B  D1,PICTNUM(A4)
    
    PULL_L  D1-D7/A0-A6
10$
    RET
    ENDC

*--------------------------
    XREF    SEATED,ROOM

SP.TurfOff

* Turfs the player of the D0 if here is sitting on it

    CMP.W   SEATED(A4),D0       ;is the player on it?
    BNE.S   90$         ;no, exit
    MOVE.W  ROOM(A4),D1     ;destination
    DO  MovePlayer      ;put player simply in room
    SCN 254         ;you are thrown off @ in the process.
90$
    RET
SP.FreezeWaterBeer
    MOVE.W  D0,STOPRON(A4)      ; just in case...
    SCN 822
    RET

    XDEF    SP.FreezePubHearth
    XREF    FlagPubHearthCold,SETIN,ODT_PUBHEARTH
SP.FreezePubHearth
    TEST_B  FlagPubHearthCold(A4)   ; done already ?
    BEQ.S   10$
    MSGGO   NHAP            ; nothing happens
10$
    ST  FlagPubHearthCold(A4)
    SCN 71          ; fire goes out leaving cold ashes
    MOVE.W  #NPUBFIRE,D0        ; no more fire
    DO  SETOUT
    BCLR    #7,ODT_PUBHEARTH(A4)    ; other desc for hearth
    MOVE.W  #NASH,D0
    GETINFO
    BCLR    #7,(A0)         ; no longer glowing
    AND.B   #$F0,3(A0)      ; fod hot texture
    OR.B    #2,3(A0)        ; insert texture rough
    RET

*------------------------------------------
    
    END
