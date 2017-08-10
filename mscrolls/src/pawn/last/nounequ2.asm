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
*	@(#)nounequ2.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
*  SRC.ADV.NOUNEQUATES2
*
*--------------------------------
 
 
NDEVIL         EQU  176
*NTHRONE        EQU  177
NEYES          EQU  178
NDEMONS        EQU  179
*NCORPSES       EQU  180
*NNOOSES        EQU  181
*NFRIDGE        EQU  182
*NHEADS         EQU  183
NWKEY          EQU  184
*NCRYSTALS      EQU  185
NPIANO         EQU  186
NLEWIS         EQU  187
*NSHAFT3        EQU  188
*NLADDER        EQU  189
*NCLOUD         EQU  190
*NLEDGE5        EQU  191
NBRIDGE5       EQU  192
*NPLANKS        EQU  193
NDRAGON        EQU  194
*NTREASURE      EQU  195
NSHADOWS       EQU  196
NMKEY          EQU  197
NLAMP          EQU  198
*NTABLE         EQU  199
NHAT           EQU  200     ;TOP HAT
NRABBIT        EQU  201
*NBOOKCASES     EQU  202
NTOMES         EQU  203
*NNICHE         EQU  204
NPLATFORM2     EQU  205      ;NOUN IN KRONOS'S WORKSHOP
NKNOCKER       EQU  206
*NPOSTS         EQU  207
NCUPBOARD      EQU  208   
NHOOK          EQU  209
*NPASTE         EQU  210
*NWALLPAPER     EQU  211
NDDOORS        EQU  212   ;PORTDDOORS
NCULLIS        EQU  NDDOORS  - HMM
NPORTER        EQU  213
NSCREAMS       EQU  214  
NLWALL         EQU  215
NRIVER2        EQU  216           ;AS SEEN FROM RIVERSIDE CHAMBER
NRWATER        EQU  217
NSNOWMAN       EQU  218
*NPOCKET        EQU  219
NPRINCESS      EQU  220
NGRAFFITTI     EQU  221
*NPAPER2        EQU  222 
NCHIT          EQU  223
NWATER         EQU  224
NSHAPES        EQU  225
NAEROSOUL      EQU  226  
NMAGICIAN      EQU  227
NKRONOS        EQU  227 
NHAT2          EQU  228     ;POINTY HAT
NCLOAK         EQU  229
*NRACK2         EQU  230
NWAND          EQU  231
NCHEST2        EQU  232
NNICHE2        EQU  233
*NRATS          EQU  234
*NTABLE2        EQU  235
NALCH          EQU  236
NROCKS         EQU  237
NSTUMP         EQU  238
*NWANDS         EQU  239
NNOTE          EQU  240
*NRAVINE2       EQU  241
NWHISKY        EQU  242
*NCUPBOARD2     EQU  243
NERIK          EQU  244
NPRISM         EQU  245
NKEY           EQU  246
NPERRIER       EQU  247      ;PERRIER WATER
*NSYMBOLS       EQU  248
NBELT          EQU  265
NSEAL          EQU  250
NRATIONS       EQU  251
NLUNCH         EQU  251
NBUTTON1       EQU  252    ;LIFT BUTTON (FIRST)
NARMOUR2       EQU  253
NBOTTLE2       EQU  254    ;WHISKEY
NROSE2         EQU  255  
NCOIN          EQU  256
NALE           EQU  257
NFLASK         EQU  258
NPOSTER        EQU  259
NHAND          EQU  260
*NFEET          EQU  261
NARMS           EQU  262
NFIST          EQU  263
NHEAD          EQU  264
NDRESS         EQU  249
NBOTTLE3       EQU  266    ;CLEAR
*NQUILT         EQU  267
NBOTTLE4       EQU  268    ;BEER
*NDUST          EQU  269
NTREE          EQU    270
*NUP THE TREE   EQU    271
NTREEDOOR      EQU    272
NSHEDDOOR      EQU    273
*NTREE TRUNK    EQU    274
*NSTAIRCASE R   EQU    275
*NLOW CEILING   EQU    276
*NCROSSROADS    EQU    277
*NPASSAGE       EQU    278
*NLOUNGE        EQU    279
*NKITCHEN       EQU    280
*NPASSAGE       EQU    281
NLIFT          EQU    282
*NROCKFACE      EQU    283
*NWHITE ROOM    EQU    284
*NOFFICE        EQU    285
*NVOTINGBOOTH   EQU    286
*NDEATH CHAMB   EQU    287
NFLOORBOARD    EQU    288
NSTAIRCASE     EQU    289
*NPIPE          EQU    290
NWHITE         EQU  290  ;HMMM
NETREE         EQU    291         ;SO U CAN ENTER TREE
NWRITING2      EQU    292
NFRONTDOOR     EQU    293
NSETTEE        EQU    294
*NCUSHIONS      EQU    295
NHARDHAT       EQU    296
*NHOOKS         EQU    297
*NSTOVE         EQU    298
*NTEAPOT        EQU    299
NCARROT        EQU    300
NMESSAGE       EQU    301
NLIFTDOOR      EQU    302
*NWORKTOP       EQU    303
NPLAQUE        EQU    304
NBUTTON2       EQU  305
NLUMPS         EQU    306 
NLEAD          EQU    NLUMPS
*NDESK          EQU    307
NSAFE          EQU    308
NBALPAPER      EQU    309   ;BALLOT PAPER -WAS PAPER3
NLBALBX        EQU    310   ;LARGEE BALLOT BOX - WAS VENT
*NTABLE         EQU    311
NRUG           EQU    312  ;WAS TERMINAL
NLANTERN       EQU    313  
*NTREES         EQU    314   
*NLEDGE         EQU    315
NPLAYER        EQU    316
NSPELL         EQU    317
NPALACE        EQU    318
*NINPALACE     EQU    319
*NGURUHEAD      EQU    320
NSBALBX        EQU    321   ;SAMLL BALLOT BOX - WAS VDU
NAEROBUTTON    EQU  322    ;AEROSOUL BUTTON
NHANDLES       EQU    323
NSWALL         EQU    324
NPWALL         EQU    325
NWINDOW1       EQU  326  ;WINDOW IN PRINCESS' ROOM
NWINDOW2       EQU  327  ;OUT SIDE ROOM
NLIFTBUTTON    EQU  328
NVOICE         EQU  329
*NTRAPDOOR      EQU  330
NGOOD          EQU  331
NEVIL          EQU  NGOOD+1
NLIFE          EQU  NGOOD+2
NDEATH         EQU  NGOOD+3
NGRINGO        EQU  NGOOD+4
NGNOMES        EQU  NGOOD+5
NKEROVNIA      EQU  NGOOD+6
NPERSON        EQU  NGOOD+7
NFERGS         EQU  NGOOD+8
NGOLD          EQU  NGOOD+9
NALCHEMY       EQU  NGOOD+10
*
NMAXNOUN       EQU  NALCHEMY+1
NOUNNUM        EQU  NGOOD-1
