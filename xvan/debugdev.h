
/************************************************************************/
/* Copyright (c) 2016 - 2020 Marnix van den Bos.                        */
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


#if !defined(__debugdev)
#define __debugdev

/*************************/
/* External declarations */
/*************************/

extern verbDir    *verb_dir;                 /* fileio.c   */
extern dirInfo    *loc_dir;                  /* dirs.c     */
extern dirInfo    *obj_dir;                  /* dirs.c     */
extern objectInfo **objects;                 /* fileio.c   */
extern attrInfo   *c_loc_attrs;              /* fileio.c   */
extern attrInfo   *c_obj_attrs;              /* fileio.c   */
extern attrInfo   *local_attrs;              /* fileio.c   */
extern timerInfo  *timers;                   /* fileio.c   */
extern int32_t    *com_loc_flags;            /* fileio.c   */
extern int32_t    *com_obj_flags;            /* fileio.c   */
extern int32_t    *local_flags;              /* fileio.c   */
extern int32_t    *exit_data;                /* fileio.c   */
extern int32_t    nr_of_verbs;               /* fileio.c   */
extern int32_t    nr_of_locs;                /* fileio.c   */
extern int32_t    nr_of_objs;                /* fileio.c   */
extern int32_t    nr_of_cflags;              /* fileio.c   */
extern int32_t    loc_flags_string_len;      /* fileio.c   */
extern int32_t    nr_of_words;               /* dirs.c     */
extern int32_t    nr_of_directions;          /* fileio.c   */
extern int32_t    first_direction_id;        /* fileio.c   */
extern int32_t    nr_of_cattrs;              /* fileio.c   */
extern int32_t    nr_of_lattrs;              /* fileio.c   */
extern int32_t    nr_of_timers;              /* fileio.c   */
extern wordTable  *word_table;               /* dirs.c     */
extern int32_t    undo_sp;                   /* undo.c     */  /* @!@ */
extern undoItem   *undo_stack;               /* undo.c     */  /* @!@ */
extern int32_t    undo_stack_size;           /* undo.c     */  /* @!@ */

extern int32_t    curr_loc;                  /* letsplay.c */
extern int32_t    direction;                 /* letsplay.c */
extern int32_t    prepos;                    /* letsplay.c */
extern int32_t    action;                    /* letsplay.c */
extern int32_t    actor;                     /* letsplay.c */
extern int32_t    subject;                   /* letsplay.c */
extern int32_t    specifier;                 /* letsplay.c */

extern void       PrintString(char*, int);   /* output.c   */
extern void       PrintNumber(int32_t, int); /* output.c   */
extern char       *outputline;               /* init.c     */
extern void       Output(void);              /* output.c   */
extern int32_t    IsLocId(int32_t);          /* execute.c  */
extern int32_t    IsObjId(int32_t);          /* execute.c  */
extern void       PrintWord(int32_t, int);   /* output.c   */
extern void       PrintId(int32_t, int);     /* output.c   */

char              *AddToString(char*, char*);
extern void       PrintExtendedSysDescr(extendedSysDescr*, int); /* output.c  */

#endif
