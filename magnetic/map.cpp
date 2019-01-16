/**
 *
 *    _    __        _      __
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/
 *
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  contact@voidware.com
 */


#include <assert.h>
#include <queue>
#include <algorithm>
#include "map.h"
#include "emux.h"

// room ids for The Guild
#include "roomsguild.h"

// room ids for Jinxter
#include "roomsjinxter.h"

using namespace std;

#define MAP_ROWS 24
#define MAP_COLS 17
#define MAX_ROOMS 125

static int gmaps[10][MAP_ROWS][MAP_COLS] = {
                        { // The Pawn.
                            {  0,  0,  0,  0, 70,  0,  0,  0,  0,  0,  0,  0},
                            {  0,  0,  0,  0, 69,  0, 72,  0,  0,  0,  0,  0},
                            {  0,  0,  0,  0, 68,  0, 71, 73,  0,  0,  0,  0},
                            {  0,  0,  0,  0,  0, 67,  0, 74, 75, 62, 59, 58},
                            {  0,  0,  0,  0,  0, 66,  0,  0, 76, 63, 60,  0},
                            { 37,  0,  0, 34,  0, 65,  0,  0,  0,  0, 61,  0},
                            {  0, 40, 41,  0, 33, 64,  0, 57,  0, 56,  0,  0},
                            {  0,  0, 39,  0,  0,  0,  0, 0 , 55, 0 , 94, 23},
                            {  0, 42, 38,  0, 32, 43, 44, 45, 54,  0, 48, 49},
                            { 36,  0,  0, 35, 31,  0,  0,  0, 46, 47, 52,  0},
                            {  0,  0, 29, 26,  0, 28,  0,  0, 50,  0,  0,  0},
                            {  0,  0, 27, 25, 24, 13, 14, 19, 51,  0,  0,  0},
                            {  0,  0,  9, 11,  0, 12,  0,  0, 53,  0,  0,  0},
                            {  0,  0,  0,  0,  0,  1, 15, 18, 20, 21, 22,  0},
                            {  0,  0,  0,  7,  5,  2,  0,  0, 30,  0,  0,  0},
                            {  0,  0, 10,  8,  6,  3, 16, 17,  0,  0,  0,  0},
                            {  0,  0, 86, 77, 78,  4,  0,  0,  0,  0,  0,  0},
                            {  0, 87, 85,  0, 79, 89,  0,  0,  0,  0,  0,  0},
                            {  0,  0,  0, 81, 80, 88, 90,  0,  0,  0,  0,  0},
                            { 84, 83, 82,  0,  0,  0,  0,  0,  0,  0,  0,  0}
                        },
   { // Guild 0f Thieves
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             ,  0             , RNBATHROOM     , RNROOM3        , RNROOM4    ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             , RNLIBRARY      , RNCORRIDOR1    , RNCORRIDOR2    , RNCORRIDOR3,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             ,  0             , RNCORRIDOR4    ,  0             , RNBEDROOM3 ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             ,  0             , RNBEDROOM2     , RNLAB          ,  0         ,  0      ,  0},
       {  0     , RNBHSE  ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             ,  0             , RNPATH         , RNSTABLE       , RNCELLAR   ,  0      ,  0},
       {  0     ,  0      , RNDRM5 ,  0    , RNDRM1    ,  0         ,  0     ,  0      ,  0            ,  0             , RNLOUNGE       , RNHALL2        , RNHALL1        , RNSWAY5    ,  0      ,  0},
       {  0     ,  0      ,  0     , RNDRM3,  0        ,  0         ,  0     ,  0      ,  0            ,  0             , RNGALLERY      , RNYARD         , RNKITCHEN      , RNPASSAGE1 ,  0      ,  0},
       {  0     ,  0      , RNDRM4 ,  0    , RNDRM2    ,  0         , RNGDN2 , RNTJUNC2, RNAVRE        , RNCAGE         , RNROOM1        , RNGATEHOUSE    , RNQUARTERS     , RNROOM2    ,  0      ,  0},
       {  0     , RNSTPS  , RNFNRM ,  0    ,  0        ,  0         , RNPSG4 , RNTJUNC1,  0            ,  0             , RNPSG7         ,  0             , RNBEDROOM1     , RNPASSAGE3 ,  0      ,  0},
       {  0     , RNSHAFT1,  0     , RNPSG2,  0        ,  0         , RNHSE  , RNTJUNC3,  0            , RNOFFIS        ,  0             ,  0             , RNSCRUB1       ,  0         ,  0      ,  0},
       {  0     , RNCRAB1 , RNRMUD ,  0    , RNROOM9   ,  0         ,  0     ,  0      ,  0            , RNENTR         , RNHGROUND      , RNSCRUB3       , RNSCRUB2       , RNJETTY    , RNBOAT  ,  0},
       { RNFACE , RNCAVE2 , RNCAVE3, RNRRM ,  0        , RNFRK      ,  0     ,  0      ,  0            ,  0             , RNFOREST2      , RNFOREST1      , RNFIELD        ,  0         ,  0      ,  0},
       {  0     , RNCAVE1 , RNTPWFL, RNRVR , RNKBANK   , RNSHOP1    , RNCEMTR,  0      ,  0            ,  0             , RNFOREST4      , RNFOREST3      , RNMILL1        ,  0         ,  0      ,  0},
       {  0     , RNBANK  ,  0     ,  0    ,  0        , RNSHOP2    ,  0     ,  0      ,  0            ,  0             ,  0             ,  0             ,  0             ,  0         ,  0      ,  0},
       {  0     , RNSUMP  ,  0     ,  0    ,  0        , RNSHOP3    ,  0     ,  0      , RNSTAIR1      ,  0             ,  0             , RNSWAY2        ,  0             ,  0         ,  0      ,  0},
       {  0     , RNTUNNEL,  0     ,  0    , RNINBANK  , RNTILL1    ,  0     ,  0      , RNSWAY1       ,  0             , RNACHAM        , RNSTAIR2       ,  0             ,  0         ,  0      ,  0},
       { RNWELL3, RNWELL2 , RNWELL1,  0    , RNMANOFFIS, RNTHREEROOM,  0     ,  0      , RNSWAY4       , RNORGANROOM    , RNTEMPLE       , RNSWAY3        ,  0             ,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        , RNTWOROOM  ,  0     ,  0      ,  0            ,  0             , RNGDN1         , RNSTAIR3       ,  0             ,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        , RNONEROOM  ,  0     ,  0      , RNSTAIR4      ,  0             ,  0             ,  0             ,  0             ,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        , RNSIXROOM  ,  0     ,  0      , RNBLACKSQUARE , RNINDIGOSQUARE1, RNBLUESQUARE1  , RNGREENSQUARE1 , RNREDSQUARE3   ,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        , RNFIVEROOM ,  0     ,  0      , RNREDSQUARE2  , RNVIOLETSQUARE1, RNYELLOWSQUARE2, RNINDIGOSQUARE2, RNYELLOWSQUARE1,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        , RNFOURROOM ,  0     ,  0      , RNORANGESQURE3, RNORANGESQUARE2, RNBLUESQUARE3  , RNVIOLETSQUARE2, RNORANGESQUARE1,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      , RNGREENSQUARE2, RNYELLOWSQUARE3, RNINDIGOSQUARE3, RNREDSQUARE1   , RNWHITESQUARE  ,  0         ,  0      ,  0},
       {  0     ,  0      ,  0     ,  0    ,  0        ,  0         ,  0     ,  0      ,  0            ,  0             ,  0             ,  0             ,  0             , RNCRYPT    , RNSHRINE, RNLIBRARY2}
    },
    { // Jinxter
       {  0            ,  0            , JRCOUNTRYLANE2,  0           ,  0               , JRFIELD1       ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            , JRBACKGARDEN1 ,  0           ,  0               , JRWOOD1        , JRORCHARD       , JRSTREAM1         ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  JRKITCHEN1   ,  0            , JRCONSERVATORY,  0           ,  0               , JRFIELD2       , JRFRONTGARDEN2  , JRBRIDGE1         , JROVER_HATCH        , JRAIRLOCK    , JRCORRIDOR1       , JRTOPOF_LIBRARY2 ,  0              ,  0          ,  0},
       {  0            , JRBATHROOM1   ,  0            ,  0           , JRDININGROOM1    , JRKITCHEN2     ,  0              , JRBOATHOUSE       ,  0                  , JRONLAKE     , JROFFICE          , JRLIBRARY2       ,  0              ,  0          ,  0},
       {  JRBEDROOM1   ,  0            , JRLIBRARY1    ,  0           , JRSTUDY1         , JRCELLAR2      ,  0              , JRCANOE_MOORING   , JRUNDERWATER2       , JRINF_UNDERWATER, 0              , JROVER_WRECK     ,  0              ,  0          ,  0},
       {  0            , JRHALLWAY1    ,  0            , JRBACKGARDEN2,  0               ,  0             ,  0              , JRLAKE1           ,  0                  , JRUNDERWATER ,  0                , JRSHIPWRECK      ,  0              ,  0          ,  0},
       {  0            , JRFRONTGARDEN1,  0            , JRHOLLYBUSH  ,  0               ,  0             , JRWOOD2         , JRPAGODA          ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  JRINFINITE2  , JRCOUNTRYLANE1, JRINFINITE1   ,  0           ,  0               ,  0             ,  0              , JRSTEPS1          , JRCONNECTING_PASSAGE, JRDIVERS_ROOM,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            , JRONBUS       ,  0            ,  0           , JRINOVEN         , JRBAKERY       ,  0              , JRPOSTOFFICE      ,  0                  , JRWITCH_CHAMBER, JRBALCONY       ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           , JRBAKERY_LARDER  , JRBAKERY_OVEN  , JRFAIRGROUND    , JRBEERGARDEN      ,  0                  , JRHALLWAY    ,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           ,  0               , JRCAROUSEL     , JREND_OF_BEAM   , JRPUB             , JRBOTTOM_WELL       , JRSPIRAL_STAIRCASE,  0           ,  0               ,  0              ,  0          ,  0},
       {  0            , JRWATERFALL   ,  0            ,  0           , JRCLOCKMAKERS    , JRCLOCK        , JRON_PLATFORM   ,  0                ,  0                  , JRTOWER_ENTRANCE, JRCHIMNEY      , JRANTECHAMBER    , JRLIBRARY4      , JRJEWEL_ROOM,  0},
       {  0            ,  0            , JRTHEATRE     , JRRAILWAY_TRACK, JRGUARDS_VAN   ,  0             ,  0              ,  0                ,  0                  ,  0           , JRBANQUET_HALL    ,  0               , JRCASTLE_KITCHEN,  0          ,  0},
       {  0            ,  0            , JRPOST_DEPOT  , JRPLATFORM3  , JRTRAIN_ROOF     ,  0             ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               , JRIN_HATCH      ,  0          ,  0},
       {  0            ,  0            ,  0            , JRPLATFORM2  , JRPLATFORM_BRIDGE,JROTHER_PLATFORM,  0              ,  0                ,  0                  , JRCASTLE_PARAPETS, JRARCHWAY     , JRPASSAGE        , JRDUNGEON       ,  0          ,  0},
       {  0            , JROUTSIDE_STATION, JRTRAIN_STATION, JRPLATFORM1, JRON_TRAIN     ,  0             , JRTOP_MOUNTAIN  ,  0                ,  0                  , JRWALLED_GARDEN, JRQUADRANGLE    ,  0               , JRGAOL          ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           ,  0               ,  0             ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           ,  0               ,  0             ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           ,  0               ,  0             ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0},
       {  0            ,  0            ,  0            ,  0           ,  0               ,  0             ,  0              ,  0                ,  0                  ,  0           ,  0                ,  0               ,  0              ,  0          ,  0}
    },
                        { // Corruption
                           {  0, 37,  0, 16,  0, 22,  0,  0,  0,  0, 12, 13},
                           { 35, 36,  0,  0, 24, 23, 26,  0,  0,  2,  3,  4},
                           {  0, 38, 43,  0,  0, 25,  0,  0,  0,  1,  5,  6},
                           { 33,  0, 34,  0, 40,  0, 11,  9,  8, 10,  0,  0},
                           {  0, 29, 27, 20, 39, 44,  0, 15,  7,  0,  0,  0},
                           {  0, 32, 28, 18,  0, 41,  0, 17,  0,  0,  0,  0},
                           {  0, 31,  0, 21,  0, 42,  0, 14, 19,  0,  0,  0},
                           {  0,  0, 30,  0,  0,  0,  0, 45,  0,  0,  0,  0}

                        },
                        { // Fish!
                           {  0,  0, 31, 27, 26,  0,  0,  0,  0,  0,  0},
                           {  0,  0, 29, 24, 30,  0,  0,  0,  0,  0,  0},
                           {  1,  0, 25, 28, 23, 32, 18,  0,  0,  0,  0},
                           {  2,  3, 11,  0,  0,  0, 17, 19,  0,  0,  0},
                           { 35,  0, 12, 13, 14, 15, 16, 20,  0,  0,  0},
                           {  0,  0, 10,  0,  6,  0, 22,  0, 21,  0,  0},
                           {  0,  0,  8,  7,  5,  4,  0,  0,  0,  0,  0},
                           {  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,  0},
                           {  0,  0, 34,  0,  0,  0,  0, 63,  0,  0,  0},
                           {  0,  0, 36,  0,  0,  0,  0, 64,  0,  0,  0},
                           {  0,  0, 37, 38,  0,  0,  0, 65,  0,  0,  0},
                           {  0,  0,  0, 39, 43, 53, 57, 42,  0,  0,  0},
                           {  0,  0,  0,  0,  0,  0, 74,  0, 55,  0,  0},
                           {  0,  0,  0,  0, 73,  0, 56, 75, 76, 54,  0},
                           { 69, 68, 67,  0,  0, 72, 71, 41, 59, 60,  0},
                           {  0,  0, 66,  0,  0, 40, 70,  0,  0,  0, 51},
                           {  0,  0,  0,  0, 58, 44, 45, 46, 49, 50,  0},
                           {  0,  0,  0,  0,  0, 52,  0, 47,  0,  0, 48}
                        },
                        { // Myth
                           {  0, 18, 27,  0,  0 },
                           { 25, 26,  0,  0,  0 },
                           { 24, 23,  0,  0,  0 },
                           {  0, 22,  0,  0,  0 },
                           {  0, 17,  0,  0,  0 },
                           { 16, 15,  0,  0,  0 },
                           { 13, 12, 14,  0,  0 },
                           {  0, 11,  0,  0,  0 },
                           {  0, 10,  0,  0,  0 },
                           {  0, 19,  0,  0,  0 },
                           {  0,  5,  0,  0,  0 },
                           {  0,  4,  9,  0,  0 },
                           {  7,  3,  0,  0,  0 },
                           {  6,  0,  0,  8,  0 },
                           {  0,  1,  2,  0,  0 }
                        },
                        { // Wonderland
                           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  4,  0},
                           {  0,  0,  0,  0,  0,  0,  0, 38,  0,  0,  0,  6,  5,  2,  0,  0,  0},
                           {  0,  0,  0, 47,  0,  0, 36, 35, 37,  0,  0,  7,  0,  3,  0,  0,  0},
                           {  0,  0,  0, 46, 44, 43, 40,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0},
                           {  0,  0,  0,109,  0, 42, 39, 33, 34,  0,  0,  0,  0,  0,  0,  0,  0},
                           {  0,  0,  0, 45,  0, 48, 41, 32,  0, 66, 67,  0,  0, 97, 96, 84, 85},
                           {  0, 27,  0,  0,  0,  0,  0, 31, 51, 65,  0,  0, 78,  0, 95, 86, 87},
                           {  0,  0, 12,  0,  0,  0, 49,  0, 64, 61, 62,  0,  0, 77, 80, 81, 83},
                           {  0, 23,  0, 9, 10, 11, 50,  0, 63, 60,  0,  0, 76,  0, 79, 82, 113},
                           { 24, 26, 25,  0, 14, 15, 13, 30,  0, 52,  0,  0,  0, 75,  0,  0,  0},
                           {  0, 22, 21, 16,  0,  0, 55,  0, 53,  0,  0, 72, 74,  0,  0,  0,  0},
                           { 28,  0, 17,  0,  0,  0,  0, 54,  0,  0, 71, 89,  0, 73,  0, 93,  0},
                           {  0, 29, 19, 18,  0,  0,  0, 56,  0, 70,100,  0, 88,  0, 92, 98,  0},
                           {  0,  0, 20,  0,  0, 68,  0,  0, 69,  0,101,102,  0,  0,  0, 94,  0},
                           {  0,112,  0,  0,  0, 59, 58, 57,  0,  0,103,104,  0,  0,  0, 99,  0},
                           {  0,  0,  0,  0,  0,  0,  0,  0,  0,107,106,105,  0,  0,  0,  0,  0}
                        }
                      };

// show only visited rooms that are in the same area as the current room
static int mlevels[10][MAX_ROOMS] =
                      {
                        // The Pawn
                        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,\
                          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,\
                          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,\
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 2, 0},
                        // Guild
                        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,\
                          0, 0, 0, 0, 0, 0, 3, 8, 8, 8, 8, 3, 3, 3, 4, 5, 6, 5, 5, 5,\
                          6, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 9, 9, 9, 3, 3, 3,\
                          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 0, 10, 10,\
                          10, 10, 0, 0, 0, 0 },
                        // Jinxter WIP
                        { 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,\
                          2, 2, 2, 2, 2, 2, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1,\
                          1, 1, 1, 1, 1, 1, -1, 1, 1, 1, 2, 2, 2, 2, 2, -1 },
                        // Corruption
                        { 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 3, 0, 6, 0, 0, 4, 0,\
                          0, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                          0, 0, 0, 0, 3 },
                        // Fish! TODO - what are the 24 additional rooms on level 14?
                        { 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 4, 6, 6,\
                          7, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1, 8, 15, 12, 12, 13, 13, 13,\
                          13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,\
                          -1, -1, 13, 13, 13, 13, 13, 13, 13, 13, 11, 10, 9, 13, 13, 13, 14, 14, 14, 14,\
                          14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
                        // Myth
                        { 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1,\
                          -1, 2, 2, 2, 2, 2, 2, -1 },
                        // Wonderland
                        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,\
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 1, 1, 1, 1,\
                          1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6,\
                          6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,\
                          7, 7, 7, 1, 1, -1, -1, 1, 1, 1, 1, 8, 8, 1, 1, 1, 5, 5, 5, 5,\
                          5, 5, 5, -1, 4, -1, -1, 1, 1 }
                      };

static const char* dirs[] = { "n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d" };

GameMap GameMap::theMap;

void GameMap::build_map_poslist(int gameid)
{
     int room, level;

     if (!gameid) return; // not supported

     room_mpos.reserve(MAX_ROOMS);

     for (int y=0; y < MAP_ROWS; ++y)
         for (int x=0; x < MAP_COLS; ++x)
         {
             room = gmaps[gameid-1][y][x];
             if (room != 0)
             {
                 level = mlevels[gameid-1][room-1];
                 room_mpos.push_back(MapPos(room,x,y,level));
             }
         }
     sort(room_mpos.begin(), room_mpos.end());
}

string GameMap::find_path(int src, int dest)
{
    vector<vector<int> >graph(MAX_ROOMS*12);
    vector<int> path;
    queue<vector<int> >q;

    int debug=0;

    if (debug)
          cout << "Connections:" << endl;


    for (size_t i = 1; i <= maxRoom; ++i)
    {
        IItem ri = IItem::getRoom(i);
        if (!ri) continue;

        if (ri.isExplored())
        {
            int room_no = ri.loc_data();
            for (int j = 0; j < IItem::dir_count; ++j)
            {
                IItem ej = ri.getExit(j);
                if (ej)
                {
                    graph[room_no].push_back(ej.loc_data());
                    if (debug)
                       cout << "\t" << room_no << " -> " << ej.loc_data() << endl;
                }
            }
        }
    }

    path.push_back(src);
    q.push(path);

    while(!q.empty())
    {
        path=q.front();
        q.pop();

        int last=path[path.size()-1];
        if(last==dest)
        {
            if (debug)
            {
                cout << "Path from " << src << " to " << dest << ": ";
                for(size_t i=0;i<path.size();++i)
                {
                    cout <<  path[i]<<" ";
                }
                cout << endl;
            }

            // Build movement commands
            string movecmd="";
            for (int i=0; i< (int)path.size()-1;++i)
            {
                IItem ri = IItem::getRoom(path[i]);
                //cout << "Room: " << path[i] << endl;
                assert(ri);
                for (int j = 0; j < IItem::dir_count; ++j)
                {
                    IItem ej = ri.getExit(j);
                    if (ej)
                    {
                        //cout << "\tTesting room: " << ej.loc_data() << " with target " << path[i+1] << endl;
                        if ((int)ej.loc_data()==path[i+1])
                        {
                            //cout << "Match at " << dirs[j] << endl;
                            movecmd += dirs[j];
                            if (i < (int)path.size()-2)
                                movecmd += ".";
                            break;
                        }
                    }
                }
            }

            if (debug)
                cout << movecmd << endl;

            return movecmd;
        }

        for(size_t i=0;i<graph[last].size();++i)
        {
            if(_not_visited(graph[last][i],path))
            {
                vector<int>new_path(path.begin(),path.end());
                new_path.push_back(graph[last][i]);
                q.push(new_path);
            }
        }
    }
    return "";
}
