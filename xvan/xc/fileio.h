
/************************************************************************/
/* Copyright (c) 2016, 2017, 2018, 2019 Marnix van den Bos.             */
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

#if !defined(__fileio)
#define __fileio

#define CR              (char) 10
#define TAB             (char) 9
#define SPACE           ' '
#define STRING_MARKER   '"'
#define COMMENT_MARKER  '#'

/*************/
/* externals */
/*************/

extern FILE *source_file;
extern FILE *datafile;

extern char file_path[];  /* holds absolute path to working directory */
extern char current_filename[];

extern int32_t line_num;
extern int32_t nr_of_directions;
extern int32_t first_direction_id;
extern int16_t  debug;            /* init.c   */

extern int32_t nr_of_verbs;       /* tables.c */
extern int32_t nr_of_syn_verbs;   /* tables.c */
extern int32_t nr_of_words;       /* tables.c */
extern int32_t nr_of_locations;   /* tables.c */
extern int32_t nr_of_objects;     /* tables.c */
extern int32_t nr_of_cflags;      /* tables.c */
extern int32_t nr_of_lflags;      /* tables.c */
extern int32_t nr_of_cattrs;      /* tables.c */
extern int32_t nr_of_lattrs;      /* tables.c */
extern int32_t nr_of_ctrigs;      /* tables.c */
extern int32_t nr_of_ltrigs;      /* tables.c */
extern int32_t nr_of_ldescrs;     /* tables.c */
extern int32_t nr_of_timers;      /* tables.c */

extern wordInfo     *word_list;
extern wordTable    *word_table;
extern locationData *loc_table;   /* tables.c */
extern objectData   *obj_table;   /* tables.c */
extern flagData     *flag_table;  /* tables.c */
extern attrData     *attr_table;  /* tables.c */
extern triggerData  *trigg_table; /* tables.c */
extern descrData    *descr_table; /* tables.c */
extern verbDir      *verb_dir;    /* tables.c */

extern timerInfo *md_start;
extern timerInfo *md_end;

extern int32_t *exit_table;

extern void     PrintError(int16_t, resultStruct*, char*);      /* errors.c   */

extern uint16_t xv_htons(uint16_t);                             /* endian.c   */
extern uint32_t xv_htonl(uint32_t);                             /* endian.c   */
extern uint64_t xv_htonll(uint64_t);                            /* endian.c   */
extern int32_t  CheckIfKeyword(char*);                          /* keyword.c  */
extern int32_t  IsLocId(int32_t);                               /* tables.c   */
extern int32_t  IsLDescrId(int32_t);                            /* tables.c   */
extern int32_t  IsSpecId(int32_t);                              /* tables.c   */
extern int32_t  IsTestFun(int32_t);                             /* tables.c   */
extern int32_t  IsIntAct(int32_t);                              /* tables.c   */

extern int32_t  LookUpId(char*);                                       /* prsesntc.c */
extern int32_t  GetLocationId(char*, int32_t*, int32_t, int64_t);      /* tables.c   */
extern int32_t  GetObjectId(char*, int32_t*, int32_t, int64_t);        /* tables.c   */
extern int32_t  GetDescrId(char*, int32_t*, int32_t, int32_t);         /* tables.c   */
extern int32_t  GetTimerId(char*, int32_t*, int32_t);                  /* tables.c   */
extern int32_t  GetAttrId(char*, int32_t, int32_t, int32_t*, int32_t, int32_t, int32_t, int32_t); /* tables.c  */ /* 10march2017 */
extern void     ErrHdr(void);                               /* output.c   */

extern int32_t CreateLocDebugInfo(debugInfo**);
extern int32_t CreateObjDebugInfo(debugInfo**);
extern int32_t CreateAttrDebugInfo(debugInfo**, debugInfo**);
extern int32_t CreateFlagDebugInfo(debugInfo**, debugInfo**);
extern int32_t CreateTimerDebugInfo(debugInfo**);
extern int32_t CreateTriggerDebugInfo(debugInfo**, debugInfo**);

#endif
