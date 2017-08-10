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
*	@(#)nounequ1.asm	2.1
*	9/19/86
*
******************************************************

		SECTION		"ADV",CODE

*--------------------------------
*
* SRC.ADV.NOUNEQUATES1
*
*--------------------------------
 
NONPATH        EQU  1 
*NONPATH2       EQU  2
*NONPATH3       EQU  3
*NVOID          EQU  4
*NFOREST2       EQU  5
*NFOREST3       EQU  6
NCLEARING      EQU  7
*NFOREST4       EQU  8
*NFOREST5       EQU  9
*NFOREST6       EQU  10
*NFOREST7       EQU  11
*NONPATH4       EQU  12
*NONPATH5       EQU  13
NPLAIN2        EQU  14
NWILDERNESS    EQU  15
NPLAIN3        EQU  16
NSCLIFF        EQU  17
NBRIDGE        EQU  18
NNCLIFF        EQU  19
NONBRIDGE      EQU  20
NGARDENS       EQU  21
NGATEWAY       EQU  22
NMAZE          EQU  23
*NHILLS         EQU  24
*NHILLS2        EQU  25
NHILL          EQU  26
*NHILLS3        EQU  27
NFOOTHILLS     EQU  28
NINHUT         EQU  29
NINSHED        EQU  30
NSHIRT         EQU  31
*NLINE          EQU  32
NBOARD         EQU  33
NWRITING       EQU  34
NBRIDGE2       EQU  35
NPOT          EQU  36   
NSHED          EQU  37
*NTERRAIN       EQU  38
*NARCHWAY       EQU  39
NGUARDS        EQU  40
NEMBLEMS       EQU  41
NPIKES         EQU  42
NHUT           EQU  43
*NMUD           EQU  44
NPLANT         EQU  45
NJEANS         EQU  46
NARMOUR        EQU  47   
NPONCHO        EQU  48
NSTETSON       EQU  49
NCHEROOT       EQU  50
NMAGNUM        EQU  51
NJOHN          EQU  52
NCART          EQU  53
NDONKEY        EQU  54
NWALL          EQU  55
*NWIRE          EQU  56
NFOUNTAIN      EQU  57
NADVENT        EQU  58
NHORSE         EQU  59
NGRAVEL        EQU  60
NHOE           EQU  61
NPOUCH         EQU  62
NTROWEL        EQU  63
NRAKE          EQU  64
NCLIFF2        EQU  65
*NBARROW        EQU  66
NBLUE          EQU  67
NGREEN         EQU  68
NRED           EQU  69
NGURU          EQU  70
NROBE          EQU  71
NBOWL          EQU  72
NBOULD         EQU  73
*NBORDERS       EQU  74
NBUSHES        EQU  75
*NBENCH         EQU  76
NBAND          EQU  77
*NWATERFALL     EQU  78
*NFOG           EQU  79
*NPATH          EQU  80
NRICE          EQU  81
*NPOTION        EQU  82
*NTRACK         EQU  83
*NTRACK2        EQU  84
*NTRACK3        EQU  85
*NPLATEAU       EQU  86
*NPLATEAU2      EQU  87
NPLATEAU3      EQU  88
*NPLATEAU4      EQU  89
NINTOWER       EQU  90
NLANDING       EQU  91
NJAIL          EQU  92
NWALL2         EQU  93
*NSTORE         EQU  94
*NCAVEMOUTH     EQU  95
*NCAVERN        EQU  96
*NCAVERN2       EQU  97
*NCAVE          EQU  98
*NCORRIDOR      EQU  99
NVSHAFT        EQU  100
NCAVERN4       EQU  101
*NENTRANCE      EQU  102
*NCORRIDOR2     EQU  103 
NRIVER         EQU  104           ;LAVA RIVER (ROOM)
NROOM          EQU  105
NPASSAGE       EQU  106  
NLAB           EQU  107
*NROOM2         EQU  108
*NCAVERN5       EQU  109
NHELL          EQU  110
*NCAVERN6       EQU  111
*NCHAMBER       EQU  112
NROOM3         EQU  113
*NROOM4         EQU  114
NSHAFT         EQU  115 
NLEDGE         EQU  116
NBRIDGE3       EQU  117
NBRIDGE4       EQU  118
NLEDGE2        EQU  119
*NCHAMBER2      EQU  120
*NWORKSHOP      EQU  121
NPLATFORM      EQU  122       ;ROOM
*NTUNNEL        EQU  123
NROOM5         EQU  124
NLEDGE3        EQU  125
*NPASSAGE2      EQU  126
NSHAFT2        EQU  127
NLEDGE4        EQU  128
*NCHAMBER3      EQU  129
*NTRACK4        EQU  130
NROCKFALL      EQU  131
NCAVE2         EQU  132
NSTAIRWAY      EQU  133
*NPLATEAU5      EQU  134
NTOWER         EQU  135
NSNOW          EQU  136
NSLOPE         EQU  137
NDOOR          EQU  138
*NSTRAPS        EQU  139
NBED           EQU  140
NBOOTS         EQU  141
NROPE          EQU  142
NBATS          EQU  143
*NPOOL          EQU  144
*NTITES         EQU  145
*NMITES         EQU  146
NREM           EQU  147
NLRIVER        EQU  148           ;LAVA RIVER (OBJ)
NLAVA          EQU  149
NPEDESTAL      EQU  150
NNOTICE        EQU  151
NDOORS         EQU  152
NLAMPS         EQU  153
*NWINDOWS       EQU  154
NDOOR2         EQU  155
NPAPER         EQU  156
NLISTING       EQU  157
*NFAN           EQU  158
*NASH           EQU  159
*NCOMPUTERS     EQU  160
*NPERIPH        EQU  161
NMAT           EQU  162
NLIMBECKS      EQU  163
NFLASKS        EQU  164
*NLIQUID        EQU  165
*NTUBES         EQU  166
*NVAPOUR        EQU  167
NBOXES         EQU  168
*NJARS          EQU  169
NCHEST         EQU  170
*NTURF          EQU  171
NBOTTLE1       EQU  172    ;POTION
*NGEYSERS       EQU  173
*NVITRIOL       EQU  174
*NSCYS          EQU  175
