*****************************************************
*
*   %W%
*   %G%
*
******************************************************

        SECTION     "ADV",CODE

*--------------------------------
*
*   SRC.ADV.SPECIALSN
*
*--------------------------------


    include "equates.i"
    include "verbequ.i"
    include "nounequ1.i"
    include "nounequ2.i"
    include "roomequ.i"
    include "macros.i"
    include "scenmacros.i"
    include "scoreref.i"
***********************************************
*
*   SP.NothingDoing
*
* Deals with situations when WHATEVER you do no notice is taken
*
* DO NOT MESS WITH THIS CODE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*
* Ken 15/9/87
* 
***********************************************


    XDEF    SP.NothingDoing
    XREF    FlagNothingDoing
    XREF    GuardianHere,FlagRunover,SETIN,VERB,SETPRON,TIMER12
    XREF    NPCINRM,ROOM,SINGV.TB,VLN.SING,UseWhere,DROPHERE
    XREF    INVENT,FlagGameOver,GameFinished


*
*
* RunOver   -   Handles the fixed two moves after picking
*           up the keyring at the start of the game.


SP.NothingDoing


    BSET    #1,FlagRunover(A4)  ;Been run over yet?
    BNE.S   SecondTime      ;=> yup
                    ;Else, first call to this routine....
    TEST_B  FlagGameOver(A4)    ;End?
    BEQ.S   02$         ;=> Nope
    CMP.W   #VNINVENTORY,VERB(A4)   ;Trying an I?
    BEQ INVENT          ;if so, do it

02$
    SCN 376
* You are about to die. Your job is to come to terms with the situation.
* Pointless to <VERB>.

* bit to suss how to phrase verb

    MOVE.W  VERB(A4),D1
    BPL.S   05$
    MOVEQ   #VNGET,D1       ;fool rest! (ok, UseWhere)
    SCN 759         ; 'do '
    BRA.S   15$
05$

    DO  P.VERB
    MOVEQ   #VLN.SING,D0        ;no of verbs in list -1
    LEA SINGV.TB(A4),A0
    TEST_B  D1          ;look?
    BEQ.S   90$
10$
    CMP.B   (A0)+,D1
    DBEQ    D0,10$
    BEQ.S   90$         ;EQ=> is a singv (like inv,look)
15$
    SCN 760         ; 'any'
    CALL    UseWhere        ;returns D7 0=> D.O. in noun
    CMP.W   #1,D7           ;1=>place, 2=>npc
    BEQ.S   20$         ;place
    BMI.S   30$         ;noun
    SCN 761         ;'one ' npc
    BRA.S   90$
20$
    SCN 762         ;'where '
    BRA.S   90$
30$
    SCN 763         ;'thing '

90$
    DOGO    BSFULLSTOP


    XREF    GETWOSSNAME,DO.SCORE,LUCK_,REMASTER

SecondTime

    TEST_B  FlagGameOver(A4)    ;Game finished?
    BEQ.S   10$         ;=> nope
    TEST_B  REMASTER(A4)
    BEQ.S   08$
    CMP.W   #4,LUCK_(A4)
    BEQ.S   03$
    CMP.W   #3,LUCK_(A4)
    BEQ.S   04$
    CMP.W   #2,LUCK_(A4)
    BEQ.S   05$
    CMP.W   #1,LUCK_(A4)
    BEQ.S   06$
    BRA.S   08$ 
03$
    SCN     831
    BRA.S   09$ 
04$
    SCN     832
    BRA.S   09$     
05$
    SCN     833
    BRA.S   09$     
06$
    SCN     834
    BRA.S   09$     

08$
    SCN 817
* Big spiel about getting run over and dying. Congrats, you have died.
09$
    SCORE   WINNING         ;And the final point!
    ST  GameFinished(A4)    ;That's it - you win!
    BRA.S   20$         ;Tidy up and exit

10$
    SCN 375
* Big spiel about getting snatched to safety etc

    MOVE.W  #NDOCUMENT,D0
    DO  SETIN           ;Bring document in...
    DO  SETPRON         ;So you can 'read it'
    CALL    GETWOSSNAME     ;And Guardian
    MOVE.B  #2,TIMER12(A4)      ;Init random spiel
    ST  GuardianHere(A4)    ;Tell the world - esp setmode
20$
    SF  FlagNothingDoing(A4)    ;let decode work again

90$
    RET

    XDEF    SP.NWAY
    XREF    SEATED,SP.CloudUnreach
SP.NWAY
***** help. well
***** this is called before go prints anything like. : you can't go that 
* way etc... IT IS HIGHLY DANGEROUS to use. and you should be incredibly
* wary of using it. YOU CANNOT POP THE STACK AND EXPECT IT TO WORK.
* what you must do is set EQ meaning something happened or NE meaning
* nothing did....okay.
* arrives with  D2 = room to go to (or nway or zero)
*       D5 = Direction going (DIR_E etc..)
*       A0 = Pointer to the calcnet place.
**** PLEASE LEAVE EVERYTHING ALONE.. (sniff ajs)
    CMP.W   #NNICECLOUD,SEATED(A4)  ; sitting on the cloud
    BNE.S   99$
    CMP.B   #DIR_D,D5       ; if we are going down we are inters.
    BNE.S   99$
    CMP.W   #RNWATERFALL,ROOM(A4)
    BNE.S   10$
    SETNE
    BRA.S   99$
10$
    
* okay we are on the cloud and we are going down well we won't be on
* the cloud for much longer.
    CLR.W   SEATED(A4)      ; no longer seated
    CALL    SP.CloudUnreach     ; make it unreachable.  
    SCN 298         ; land on ground with a thwack.
    SETEQ
99$
    RET

*----------------------------------------------------------
    
    END

