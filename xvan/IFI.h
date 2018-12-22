
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018 Marnix van den Bos.                   */
/*                                                                      */
/* <marnix.home@gmail.com>                                              */
/*                                                                      */
/* This file is part of XVAN, an Interactive Fiction authoring system.  */
/*                                                                      */
/* XVAN is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, version 3.                             */
/*                                                                      */
/* XVAN is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with XVAN.  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                      */
/************************************************************************/


#if !defined(__ifi)
#define __ifi

/* Everything for IFI is in here */

/********************/
/* type definitions */
/********************/

typedef struct {
          short objects;  /* do we need this one? */
          short location;
          short map;
          short picture;
          short items;
          short people;
          short move;
        } ifiStats;

/*****************************/
/*        IFI requests       */
/*****************************/

#define IFI_NO_IFI          100
#define IFI_REQ_COMMAND     101
#define IFI_REQ_CONFIGDIR   102
#define IFI_REQ_DATADIR     103
#define IFI_REQ_ITEMS       104
#define IFI_REQ_LOADDATA    105
#define IFI_REQ_LOCATION    106
#define IFI_REQ_MAP         107
#define IFI_REQ_META        108
#define IFI_REQ_MOVES       109
#define IFI_REQ_OBJECTS     110
#define IFI_REQ_PICTURE     111
#define IFI_REQ_SAVEDATA    112
#define IFI_REQ_STORY       113
#define IFI_REQ_RANDOM_SEED 114
#define IFI_REQ_PEOPLE      115


/*****************************/
/*        IFI replies        */
/*****************************/

#define IFI_RPLY_EXITS      200
#define IFI_RPLY_ITEMS      201
#define IFI_RPLY_LOCATION   202
#define IFI_RPLY_MAP        203
#define IFI_RPLY_META       204
#define IFI_RPLY_MOVES      205
#define IFI_RPLY_OBJECTS    206
#define IFI_RPLY_PEOPLE     207
#define IFI_RPLY_PICTURE    208
#define IFI_RPLY_SCORE      209
#define IFI_RPLY_SCORE_MAX  210
#define IFI_RPLY_TITLE      211
#define IFI_RPLY_TEXT       212
#define IFI_RPLY_VALUES     213

/*************************/
/* External declarations */
/*************************/

extern ifiStats  ifi_stats;                                 /* init.c */
extern storyInfo story_info;                                /* init.c */

extern int32_t   nr_of_locs;
extern int32_t   nr_of_objs;

extern int32_t   GetNextKVpair(char*, int*, kvPair*);
extern char      *AddToString(char*, char*);
extern char      *ResetString(char*);
extern int32_t   Play(char*);
extern short     fire_timers;
extern void      ifi_emitResponse(const char*);
extern char      *AddKV(char*, char*, jsonValue*);
extern char      *NewJsonObject(char*);                     /* jsonbld.c   */
extern char      *NewJsonArray(char *);                     /* jsonbld.c   */
extern char      *CloseObject(char*);                       /* jsonbld.c   */
extern char      *CloseArray(char*);                        /* jsonbld.c   */
extern char      *GetId(int32_t, int);                      /* descr2str.c */
extern int32_t   *Base64Restore(char*);                     /* restore.c   */
extern char      *Base64Save(char*);                        /* save.c      */
extern void      PrintError(int16_t, resultStruct*, char*);

#endif
